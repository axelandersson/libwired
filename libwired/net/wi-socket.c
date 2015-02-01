/*
 *  Copyright (c) 2015 Axel Andersson
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef HAVE_NETINET_IN_SYSTM_H
#include <netinet/in_systm.h>
#endif

#ifdef HAVE_NETINET_IP_H
#include <netinet/ip.h>
#endif

#include <netinet/tcp.h>
#include <netdb.h>
#include <net/if.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

#ifdef HAVE_IFADDRS_H
#include <ifaddrs.h>
#endif

#include <wired/wi-array.h>
#include <wired/wi-assert.h>
#include <wired/wi-address.h>
#include <wired/wi-date.h>
#include <wired/wi-dh.h>
#include <wired/wi-macros.h>
#include <wired/wi-lock.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-rsa.h>
#include <wired/wi-socket.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>
#include <wired/wi-thread.h>
#include <wired/wi-x509.h>

#define _WI_SOCKET_BUFFER_MAX_SIZE      262144


struct _wi_socket {
    wi_runtime_base_t                   base;
    
    wi_mutable_address_t                *address;
    wi_socket_type_t                    type;
    wi_uinteger_t                       direction;
    int                                 sd;

#ifdef HAVE_OPENSSL_SSL_H
    SSL_CTX                             *ssl_ctx;
    SSL                                 *ssl;
#endif
    
#ifdef WI_SSL
    wi_x509_t                           *tls_x509;
    wi_rsa_t                            *tls_rsa;
    wi_dh_t                             *tls_dh;
    wi_string_t                         *tls_ciphers;
#endif
    
    void                                *data;
    
    wi_boolean_t                        interactive;
    wi_boolean_t                        close;
};


#if defined(HAVE_OPENSSL_SSL_H) && defined(WI_PTHREADS)
static unsigned long                    _wi_socket_ssl_id_function(void);
static void                             _wi_socket_ssl_locking_function(int, int, const char *, int);
#endif

static void                             _wi_socket_dealloc(wi_runtime_instance_t *);
static wi_string_t *                    _wi_socket_description(wi_runtime_instance_t *);

static wi_boolean_t                     _wi_socket_set_option_int(wi_socket_t *, int, int, int);
static wi_boolean_t                     _wi_socket_get_option_int(wi_socket_t *, int, int, int *);

static wi_integer_t                     _wi_socket_read_bytes(wi_socket_t *, wi_time_interval_t, void *, wi_uinteger_t);


#if defined(HAVE_OPENSSL_SSL_H) && defined(WI_PTHREADS)
static wi_mutable_array_t               *_wi_socket_ssl_locks;
#endif


static wi_runtime_id_t                  _wi_socket_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_socket_runtime_class = {
    "wi_socket_t",
    _wi_socket_dealloc,
    NULL,
    NULL,
    _wi_socket_description,
    NULL
};



void wi_socket_register(void) {
    _wi_socket_runtime_id = wi_runtime_register_class(&_wi_socket_runtime_class);
}



void wi_socket_initialize(void) {
#ifdef HAVE_OPENSSL_SSL_H
#ifdef WI_PTHREADS
    wi_lock_t       *lock;
    wi_uinteger_t   i, count;
#endif

    SSL_library_init();

#ifdef WI_PTHREADS
    count = CRYPTO_num_locks();
    _wi_socket_ssl_locks = wi_array_init_with_capacity(wi_mutable_array_alloc(), count);
    
    for(i = 0; i < count; i++) {
        lock = wi_lock_init(wi_lock_alloc());
        wi_mutable_array_add_data(_wi_socket_ssl_locks, lock);
        wi_release(lock);
    }

    CRYPTO_set_id_callback(_wi_socket_ssl_id_function);
    CRYPTO_set_locking_callback(_wi_socket_ssl_locking_function);
#endif
#endif
}



#pragma mark -

#if defined(HAVE_OPENSSL_SSL_H) && defined(WI_PTHREADS)

static unsigned long _wi_socket_ssl_id_function(void) {
    return ((unsigned long) wi_thread_current_thread());
}



static void _wi_socket_ssl_locking_function(int mode, int n, const char *file, int line) {
    wi_lock_t   *lock;
    
    lock = WI_ARRAY(_wi_socket_ssl_locks, n);
    
    if(mode & CRYPTO_LOCK)
        wi_lock_lock(lock);
    else
        wi_lock_unlock(lock);
}

#endif



#pragma mark -

void wi_socket_exit_thread(void) {
#ifdef HAVE_OPENSSL_SSL_H
    ERR_remove_state(0);
#endif
}



#pragma mark -

wi_runtime_id_t wi_socket_runtime_id(void) {
    return _wi_socket_runtime_id;
}



#pragma mark -

wi_socket_t * wi_socket_with_address(wi_address_t *address, wi_socket_type_t type) {
    return wi_autorelease(wi_socket_init_with_address(wi_socket_alloc(), address, type));
}



#pragma mark -

wi_socket_t * wi_socket_alloc(void) {
    return wi_runtime_create_instance(_wi_socket_runtime_id, sizeof(wi_socket_t));
}



wi_socket_t * wi_socket_init_with_address(wi_socket_t *socket_, wi_address_t *address, wi_socket_type_t type) {
    int     family;
    
    socket_->address    = wi_mutable_copy(address);
    socket_->close      = true;
    socket_->type       = type;
    family              = wi_address_family(address) == WI_ADDRESS_IPV4 ? AF_INET : AF_INET6;
    socket_->sd         = socket(family, socket_->type, 0);
    
    if(socket_->sd < 0) {
        wi_error_set_errno(errno);
        
        wi_release(socket_);
        
        return NULL;
    }
    
    if(!_wi_socket_set_option_int(socket_, SOL_SOCKET, SO_REUSEADDR, 1)) {
        wi_release(socket_);
        
        return NULL;
    }
    
#ifdef SO_REUSEPORT
    if(!_wi_socket_set_option_int(socket_, SOL_SOCKET, SO_REUSEPORT, 1)) {
        wi_release(socket_);
        
        return NULL;
    }
#endif

    return socket_;
}



wi_socket_t * wi_socket_init_with_descriptor(wi_socket_t *socket, int sd) {
    socket->sd = sd;
    
    return socket;
}



static void _wi_socket_dealloc(wi_runtime_instance_t *instance) {
    wi_socket_t     *socket = instance;
    
    wi_socket_close(socket);
    
    wi_release(socket->address);
    
#ifdef WI_SSL
    wi_release(socket->tls_x509);
    wi_release(socket->tls_rsa);
    wi_release(socket->tls_dh);
    wi_release(socket->tls_ciphers);
#endif
}



static wi_string_t * _wi_socket_description(wi_runtime_instance_t *instance) {
    wi_socket_t     *socket = instance;

    return wi_string_with_format(WI_STR("<%@ %p>{sd = %d, address = %@}"),
        wi_runtime_class_name(socket),
        socket,
        socket->sd,
        socket->address);
}



#pragma mark -

static wi_boolean_t _wi_socket_set_option_int(wi_socket_t *socket, int level, int name, int option) {
    if(setsockopt(socket->sd, level, name, &option, sizeof(option)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



static wi_boolean_t _wi_socket_get_option_int(wi_socket_t *socket, int level, int name, int *option) {
    socklen_t   length;
    
    length = sizeof(*option);
    
    if(getsockopt(socket->sd, level, name, option, &length) < 0) {
        wi_error_set_errno(errno);
        
        *option = 0;
        
        return false;
    }
    
    return true;
}



#pragma mark -

wi_address_t * wi_socket_address(wi_socket_t *socket) {
    return socket->address;
}



int wi_socket_descriptor(wi_socket_t *socket) {
    return socket->sd;
}



int wi_socket_error(wi_socket_t *socket) {
    int     error;
    
    WI_ASSERT(socket->type == WI_SOCKET_TCP, "%@ is not a TCP socket", socket);
    
    if(!_wi_socket_get_option_int(socket, SOL_SOCKET, SO_ERROR, &error))
        return errno;
    
    return error;
}



#pragma mark -

void wi_socket_set_port(wi_socket_t *socket, wi_uinteger_t port) {
    wi_mutable_address_set_port(socket->address, port);
}



wi_uinteger_t wi_socket_port(wi_socket_t *socket) {
    return wi_address_port(socket->address);
}



void wi_socket_set_direction(wi_socket_t *socket, wi_uinteger_t direction) {
    socket->direction = direction;
}



wi_uinteger_t wi_socket_direction(wi_socket_t *socket) {
    return socket->direction;
}



void wi_socket_set_data(wi_socket_t *socket, void *data) {
    socket->data = data;
}



void * wi_socket_data(wi_socket_t *socket) {
    return socket->data;
}




wi_boolean_t wi_socket_set_blocking(wi_socket_t *socket, wi_boolean_t blocking) {
    int     flags;
    
    flags = fcntl(socket->sd, F_GETFL);
    
    if(flags < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    if(blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;
    
    if(fcntl(socket->sd, F_SETFL, flags) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



wi_boolean_t wi_socket_blocking(wi_socket_t *socket) {
    int     flags;
    
    flags = fcntl(socket->sd, F_GETFL);
    
    if(flags < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return !(flags & O_NONBLOCK);
}



wi_boolean_t wi_socket_set_timeout(wi_socket_t *socket, wi_time_interval_t interval) {
    struct timeval  tv;
    
    tv = wi_dtotv(interval);
    
    if(setsockopt(socket->sd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    if(setsockopt(socket->sd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



wi_time_interval_t wi_socket_timeout(wi_socket_t *socket) {
    struct timeval  tv;
    socklen_t       length;
    
    length = sizeof(tv);
    
    if(getsockopt(socket->sd, SOL_SOCKET, SO_RCVTIMEO, &tv, &length) < 0) {
        wi_error_set_errno(errno);
        
        return 0.0;
    }
    
    return wi_tvtod(tv);
}



void wi_socket_set_interactive(wi_socket_t *socket, wi_boolean_t interactive) {
    _wi_socket_set_option_int(socket, IPPROTO_TCP, TCP_NODELAY, interactive ? 1 : 0);
    
#if defined(IPTOS_LOWDELAY) && defined(IPTOS_THROUGHPUT)
    if(wi_address_family(socket->address) == WI_ADDRESS_IPV4)
        _wi_socket_set_option_int(socket, IPPROTO_IP, IP_TOS, interactive ? IPTOS_LOWDELAY : IPTOS_THROUGHPUT);
#endif
    
    socket->interactive = interactive;
}



wi_boolean_t wi_socket_interactive(wi_socket_t *socket) {
    return socket->interactive;
}



#pragma mark -


#ifdef WI_SSL

wi_string_t * wi_socket_tls_remote_cipher_version(wi_socket_t *socket) {
#ifdef HAVE_OPENSSL_SSL_H
    return socket->ssl ? wi_string_with_utf8_string(SSL_get_cipher_version(socket->ssl)) : NULL;
#endif
}



wi_string_t * wi_socket_tls_remote_cipher_name(wi_socket_t *socket) {
#ifdef HAVE_OPENSSL_SSL_H
    return socket->ssl ? wi_string_with_utf8_string(SSL_get_cipher_name(socket->ssl)) : NULL;
#endif
}



wi_uinteger_t wi_socket_tls_remote_cipher_bits(wi_socket_t *socket) {
#ifdef HAVE_OPENSSL_SSL_H
    return socket->ssl ? SSL_get_cipher_bits(socket->ssl, NULL) : 0;
#endif
}



wi_x509_t * wi_socket_tls_remote_certificate(wi_socket_t *socket) {
#ifdef HAVE_OPENSSL_SSL_H
    X509    *x509;

    x509 = SSL_get_peer_certificate(socket->ssl);

    if(!x509)
        return NULL;
    
    return wi_autorelease(wi_x509_init_with_openssl_x509(wi_x509_alloc(), x509));
#endif
}

#endif



#pragma mark -

#ifdef WI_SSL

void wi_socket_set_tls_certificate(wi_socket_t *socket, wi_x509_t *x509) {
    wi_release(socket->tls_x509);
    socket->tls_x509 = NULL;
    
    socket->tls_x509 = wi_retain(x509);
}



wi_x509_t * wi_socket_tls_certificate(wi_socket_t *socket) {
    return socket->tls_x509;
}



void wi_socket_set_tls_private_key(wi_socket_t *socket, wi_rsa_t *rsa) {
    wi_release(socket->tls_rsa);
    socket->tls_rsa = NULL;
    
    socket->tls_rsa = wi_retain(rsa);
}



wi_rsa_t * wi_socket_tls_private_key(wi_socket_t *socket) {
    return socket->tls_rsa;
}




void wi_socket_set_tls_dh(wi_socket_t *socket, wi_dh_t *dh) {
    wi_release(socket->tls_dh);
    socket->tls_dh = wi_retain(dh);
}



wi_dh_t * wi_socket_tls_dh(wi_socket_t *socket) {
    return socket->tls_dh;
}



void wi_socket_set_tls_ciphers(wi_socket_t *socket, wi_string_t *ciphers) {
    wi_release(socket->tls_ciphers);
    socket->tls_ciphers = NULL;
    
    socket->tls_ciphers = wi_retain(ciphers);
}



wi_string_t * wi_socket_tls_ciphers(wi_socket_t *socket) {
    return socket->tls_ciphers;
}

#endif



#pragma mark -

wi_socket_t * wi_socket_wait_multiple(wi_array_t *array, wi_time_interval_t timeout) {
    wi_enumerator_t     *enumerator;
    wi_socket_t         *socket, *waiting_socket = NULL;
    struct timeval      tv;
    fd_set              rfds, wfds;
    int                 state, max_sd;

    tv = wi_dtotv(timeout);
    max_sd = -1;

    FD_ZERO(&rfds);
    FD_ZERO(&wfds);

    enumerator = wi_array_data_enumerator(array);
    
    while((socket = wi_enumerator_next_data(enumerator))) {
        if(socket->direction & WI_SOCKET_READ)
            FD_SET(socket->sd, &rfds);

        if(socket->direction & WI_SOCKET_WRITE)
            FD_SET(socket->sd, &wfds);

        if(socket->sd > max_sd)
            max_sd = socket->sd;
    }
    
    state = select(max_sd + 1, &rfds, &wfds, NULL, (timeout > 0.0) ? &tv : NULL);
    
    if(state < 0) {
        wi_error_set_errno(errno);

        return NULL;
    }
    
    enumerator = wi_array_data_enumerator(array);
    
    while((socket = wi_enumerator_next_data(enumerator))) {
        if(FD_ISSET(socket->sd, &rfds) || FD_ISSET(socket->sd, &wfds)) {
            waiting_socket = socket;

            break;
        }
    }
    
    return waiting_socket;
}



wi_socket_state_t wi_socket_wait(wi_socket_t *socket, wi_time_interval_t timeout) {
    return wi_socket_wait_descriptor(socket->sd,
                                     timeout,
                                     (socket->direction & WI_SOCKET_READ),
                                     (socket->direction & WI_SOCKET_WRITE));
}



wi_socket_state_t wi_socket_wait_descriptor(int sd, wi_time_interval_t timeout, wi_boolean_t read, wi_boolean_t write) {
    struct timeval  tv;
    fd_set          rfds, wfds;
    int             state;
    
    tv = wi_dtotv(timeout);
    
    FD_ZERO(&rfds);
    FD_ZERO(&wfds);
    
    WI_ASSERT(sd >= 0, "%d should be positive", sd);
    WI_ASSERT(sd < (int) FD_SETSIZE, "%d should be less than %d", sd, FD_SETSIZE);
    WI_ASSERT(read || write, "read and write can't both be false");
    
    if(read)
        FD_SET(sd, &rfds);
    
    if(write)
        FD_SET(sd, &wfds);
    
    state = select(sd + 1, &rfds, &wfds, NULL, (timeout > 0.0) ? &tv : NULL);
    
    if(state < 0) {
        wi_error_set_errno(errno);
        
        return WI_SOCKET_ERROR;
    }
    
    if(state == 0)
        return WI_SOCKET_TIMEOUT;
    
    return WI_SOCKET_READY;
}



#pragma mark -

wi_boolean_t wi_socket_listen(wi_socket_t *socket) {
    struct sockaddr             *sa;
    struct sockaddr_storage     ss;
    wi_uinteger_t               port;
    socklen_t                   length;
    
    port    = wi_address_port(socket->address);
    sa      = wi_address_sa(socket->address);
    length  = wi_address_sa_length(socket->address);
    
    if(socket->type == WI_SOCKET_TCP) {
        if(wi_address_family(socket->address) == WI_ADDRESS_IPV6)
            _wi_socket_set_option_int(socket, IPPROTO_IPV6, IPV6_V6ONLY, 1);
    }
    
    if(bind(socket->sd, sa, length) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }

    if(socket->type == WI_SOCKET_TCP) {
        if(listen(socket->sd, SOMAXCONN) < 0) {
            wi_error_set_errno(errno);

            return false;
        }
    }
    
    if(port == 0) {
        length = sizeof(ss);
        
        if(getsockname(socket->sd, (struct sockaddr *) &ss, &length) == 0) {
            wi_release(socket->address);
            socket->address = wi_address_init_with_sa(wi_address_alloc(), (struct sockaddr *) &ss);
        }
    }
    
    socket->direction = WI_SOCKET_READ;
    
    return true;
}



wi_boolean_t wi_socket_connect(wi_socket_t *socket, wi_time_interval_t timeout) {
    struct sockaddr     *sa;
    wi_socket_state_t   state;
    wi_uinteger_t       length;
    int                 err;
    wi_boolean_t        blocking;
    
    sa      = wi_address_sa(socket->address);
    length  = wi_address_sa_length(socket->address);

    if(timeout > 0.0) {
        blocking = wi_socket_blocking(socket);

        if(blocking)
            wi_socket_set_blocking(socket, false);
        
        err = connect(socket->sd, sa, length);
        
        if(err < 0) {
            if(errno != EINPROGRESS) {
                wi_error_set_errno(errno);
                
                return false;
            }
            
            do {
                state = wi_socket_wait_descriptor(socket->sd, 1.0, true, true);
                timeout -= 1.0;
            } while(state == WI_SOCKET_TIMEOUT && timeout >= 0.0);
            
            if(state == WI_SOCKET_ERROR)
                return false;
            
            if(timeout <= 0.0) {
                wi_error_set_errno(ETIMEDOUT);
                
                return false;
            }
            
            err = wi_socket_error(socket);
            
            if(err != 0) {
                wi_error_set_errno(err);
                
                return false;
            }
        }

        if(blocking)
            wi_socket_set_blocking(socket, true);
    } else {
        if(connect(socket->sd, sa, length) < 0) {
            wi_error_set_errno(errno);
                
            return false;
        }
    }

    socket->direction = WI_SOCKET_READ;
    
    return true;
}



wi_socket_t * wi_socket_accept_multiple(wi_array_t *array, wi_time_interval_t timeout, wi_address_t **address) {
    wi_socket_t     *socket;
    
    *address = NULL;
    socket = wi_socket_wait_multiple(array, 0.0);
    
    if(!socket)
        return NULL;
    
    return wi_socket_accept(socket, timeout, address);
}



wi_socket_t * wi_socket_accept(wi_socket_t *accept_socket, wi_time_interval_t timeout, wi_address_t **address) {
    wi_socket_t                 *socket;
    struct sockaddr_storage     ss;
    socklen_t                   length;
    int                         sd, err = 0;
    
    length  = sizeof(ss);
    sd      = accept(accept_socket->sd, (struct sockaddr *) &ss, &length);
    
    if(sd < 0)
        err = errno;

    *address = (length > 0) ? wi_autorelease(wi_address_init_with_sa(wi_mutable_address_alloc(), (struct sockaddr *) &ss)) : NULL;

    if(sd < 0) {
        wi_error_set_errno(err);
        
        return NULL;
    }

    socket                  = wi_socket_init_with_descriptor(wi_socket_alloc(), sd);
    socket->close           = true;
    socket->address         = wi_retain(*address);
    socket->type            = accept_socket->type;
    socket->direction       = WI_SOCKET_READ;
    socket->interactive     = accept_socket->interactive;
    
    return wi_autorelease(socket);
}



void wi_socket_close(wi_socket_t *socket) {
#ifdef HAVE_OPENSSL_SSL_H
    int     result;
#endif
    
#ifdef HAVE_OPENSSL_SSL_H
    if(socket->ssl_ctx) {
        SSL_CTX_free(socket->ssl_ctx);
        
        socket->ssl_ctx = NULL;
    }
    
    if(socket->ssl) {
        ERR_clear_error();
        
        result = SSL_shutdown(socket->ssl);
        
        if(result == 0)
            SSL_shutdown(socket->ssl);

        SSL_free(socket->ssl);
        
        ERR_clear_error();
        
        socket->ssl = NULL;
    }
#endif

    if(socket->close && socket->sd >= 0) {
        close(socket->sd);
        
        socket->sd = -1;
    }
}



#pragma mark -





#ifdef WI_SSL

wi_boolean_t wi_socket_connect_tls(wi_socket_t *socket, wi_time_interval_t timeout) {
#ifdef HAVE_OPENSSL_SSL_H
    wi_socket_state_t   state;
    int                 err, result;
    wi_boolean_t        blocking;
    
    socket->ssl_ctx = SSL_CTX_new(TLSv1_client_method());
    
    if(!socket->ssl_ctx) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    SSL_CTX_set_mode(socket->ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_quiet_shutdown(socket->ssl_ctx, 1);
    
    if(SSL_CTX_set_cipher_list(socket->ssl_ctx, socket->tls_ciphers ? wi_string_utf8_string(socket->tls_ciphers) : "ALL") != 1) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    socket->ssl = SSL_new(socket->ssl_ctx);
    
    if(!socket->ssl) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    if(SSL_set_fd(socket->ssl, socket->sd) != 1) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    if(timeout > 0.0) {
        blocking = wi_socket_blocking(socket);
        
        if(blocking)
            wi_socket_set_blocking(socket, false);
        
        ERR_clear_error();
        
        result = SSL_connect(socket->ssl);
        
        if(result != 1) {
            do {
                err = SSL_get_error(socket->ssl, result);
                
                if(err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    wi_error_set_openssl_ssl_error_with_result(socket->ssl, result);
                    
                    ERR_clear_error();
                    
                    return false;
                }
                
                state = wi_socket_wait_descriptor(socket->sd, 1.0, (err == SSL_ERROR_WANT_READ), (err == SSL_ERROR_WANT_WRITE));
                
                if(state == WI_SOCKET_ERROR)
                    break;
                else if(state == WI_SOCKET_READY) {
                    result = SSL_connect(socket->ssl);
                    
                    if(result == 1)
                        break;
                }
                
                timeout -= 1.0;
            } while(timeout >= 0.0);
            
            if(state == WI_SOCKET_ERROR)
                return false;
            
            if(timeout <= 0.0) {
                wi_error_set_errno(ETIMEDOUT);
                
                return false;
            }
        }
        
        if(blocking)
            wi_socket_set_blocking(socket, true);
    } else {
        ERR_clear_error();
        
        result = SSL_connect(socket->ssl);
        
        if(result != 1) {
            wi_error_set_openssl_ssl_error_with_result(socket->ssl, result);
            
            ERR_clear_error();
            
            return false;
        }
    }
    
    return true;
#endif
}



wi_boolean_t wi_socket_accept_tls(wi_socket_t *socket, wi_time_interval_t timeout) {
#ifdef HAVE_OPENSSL_SSL_H
    wi_socket_state_t   state;
    int                 err, result;
    wi_boolean_t        blocking;
    
    socket->ssl_ctx = SSL_CTX_new(TLSv1_server_method());
    
    if(!socket->ssl_ctx) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    SSL_CTX_set_mode(socket->ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_quiet_shutdown(socket->ssl_ctx, 1);
    
    if(SSL_CTX_set_cipher_list(socket->ssl_ctx, socket->tls_ciphers ? wi_string_utf8_string(socket->tls_ciphers) : "ALL") != 1) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    if(socket->tls_x509) {
        if(SSL_CTX_use_certificate(socket->ssl_ctx, wi_x509_openssl_x509(socket->tls_x509)) != 1) {
            wi_error_set_openssl_error();
            
            return false;
        }
    }
    
    if(socket->tls_rsa) {
        if(SSL_CTX_use_RSAPrivateKey(socket->ssl_ctx, wi_rsa_openssl_rsa(socket->tls_rsa)) != 1) {
            wi_error_set_openssl_error();
            
            return false;
        }
    }
    
    socket->ssl = SSL_new(socket->ssl_ctx);
    
    if(!socket->ssl) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    if(SSL_set_fd(socket->ssl, socket->sd) != 1) {
        wi_error_set_openssl_error();
        
        return false;
    }
    
    if(!socket->tls_rsa && socket->tls_dh) {
        if(SSL_set_tmp_dh(socket->ssl, wi_dh_openssl_dh(socket->tls_dh)) != 1) {
            wi_error_set_openssl_error();
            
            return false;
        }
    }
    
    if(timeout > 0.0) {
        blocking = wi_socket_blocking(socket);
        
        if(blocking)
            wi_socket_set_blocking(socket, false);
        
        ERR_clear_error();
        
        result = SSL_accept(socket->ssl);
        
        if(result != 1) {
            do {
                err = SSL_get_error(socket->ssl, result);
                
                if(err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
                    wi_error_set_openssl_ssl_error_with_result(socket->ssl, result);
                    
                    ERR_clear_error();
                    
                    return false;
                }
                
                state = wi_socket_wait_descriptor(socket->sd, 1.0, (err == SSL_ERROR_WANT_READ), (err == SSL_ERROR_WANT_WRITE));
                
                if(state == WI_SOCKET_ERROR)
                    break;
                else if(state == WI_SOCKET_READY) {
                    result = SSL_accept(socket->ssl);
                    
                    if(result == 1)
                        break;
                }
                
                timeout -= 1.0;
            } while(timeout >= 0.0);
            
            if(state == WI_SOCKET_ERROR)
                return false;
            
            if(timeout <= 0.0) {
                wi_error_set_errno(ETIMEDOUT);
                
                return false;
            }
        }
        
        if(blocking)
            wi_socket_set_blocking(socket, true);
    } else {
        ERR_clear_error();
        
        result = SSL_accept(socket->ssl);
        
        if(result != 1) {
            wi_error_set_openssl_ssl_error_with_result(socket->ssl, result);
            
            ERR_clear_error();
            
            return false;
        }
    }
    
    return true;
#endif
}

#endif



#pragma mark -

wi_integer_t wi_socket_sendto_data(wi_socket_t *socket, wi_data_t *data) {
    return wi_socket_sendto_bytes(socket, wi_data_bytes(data), wi_data_length(data));
}



wi_integer_t wi_socket_sendto_bytes(wi_socket_t *socket, const char *buffer, wi_uinteger_t length) {
    wi_address_t    *address;
    wi_integer_t    bytes;
    
    address     = wi_socket_address(socket);
    bytes       = sendto(socket->sd, buffer, length, 0, wi_address_sa(address), wi_address_sa_length(address));
    
    if(bytes < 0) {
        wi_error_set_errno(errno);
        
        return -1;
    }
    
    return bytes;
}



wi_data_t * wi_socket_recvfrom_multiple_data(wi_array_t *array, wi_uinteger_t length, wi_address_t **address) {
    wi_integer_t    bytes;
    char            *buffer;
    
    buffer = wi_malloc(length);
    bytes = wi_socket_recvfrom_multiple_bytes(array, buffer, length, address);
    
    if(bytes <= 0) {
        wi_free(buffer);
        
        if(bytes == 0)
            return wi_data();
        else
            return NULL;
    }
    
    return wi_data_with_bytes_no_copy(buffer, bytes, length);
}



wi_integer_t wi_socket_recvfrom_multiple_bytes(wi_array_t *array, char *buffer, wi_uinteger_t length, wi_address_t **address) {
    wi_socket_t     *socket;
    
    *address    = NULL;
    socket      = wi_socket_wait_multiple(array, 0.0);
    
    if(!socket)
        return -1;
    
    return wi_socket_recvfrom_bytes(socket, buffer, length, address);
}



wi_data_t * wi_socket_recvfrom_data(wi_socket_t *socket, wi_uinteger_t length, wi_address_t **address) {
    wi_integer_t    bytes;
    char            *buffer;
    
    buffer = wi_malloc(length);
    bytes = wi_socket_recvfrom_bytes(socket, buffer, length, address);
    
    if(bytes <= 0) {
        wi_free(buffer);
        
        if(bytes == 0)
            return wi_data();
        else
            return NULL;
    }

    return wi_data_with_bytes_no_copy(buffer, bytes, length);
}



wi_integer_t wi_socket_recvfrom_bytes(wi_socket_t *socket, char *buffer, wi_uinteger_t length, wi_address_t **address) {
    struct sockaddr_storage     ss;
    socklen_t                   sslength;
    wi_integer_t                bytes;
    
    sslength    = sizeof(ss);
    bytes       = recvfrom(socket->sd, buffer, length, 0, (struct sockaddr *) &ss, &sslength);
    *address    = (sslength > 0) ? wi_autorelease(wi_address_init_with_sa(wi_address_alloc(), (struct sockaddr *) &ss)) : NULL;

    if(bytes < 0) {
        wi_error_set_errno(errno);
        
        return -1;
    }

    return bytes;
}



#pragma mark -

wi_integer_t wi_socket_write_data(wi_socket_t *socket, wi_time_interval_t timeout, wi_data_t *data) {
    return wi_socket_write_bytes(socket, timeout, wi_data_bytes(data), wi_data_length(data));
}



wi_integer_t wi_socket_write_bytes(wi_socket_t *socket, wi_time_interval_t timeout, const void *buffer, wi_uinteger_t length) {
    wi_socket_state_t   state;
    wi_uinteger_t       offset;
    wi_integer_t        bytes;
    
    WI_ASSERT(buffer != NULL, "buffer of length %u should not be NULL", length);
    WI_ASSERT(socket->sd >= 0, "socket %@ should be valid", socket);
    
#ifdef HAVE_OPENSSL_SSL_H
    if(socket->ssl) {
        while(true) {
            if(timeout > 0.0) {
                state = wi_socket_wait_descriptor(socket->sd, timeout, false, true);

                if(state != WI_SOCKET_READY) {
                    if(state == WI_SOCKET_TIMEOUT)
                        wi_error_set_errno(ETIMEDOUT);
                    
                    return -1;
                }
            }

            ERR_clear_error();
            
            bytes = SSL_write(socket->ssl, buffer, length);

            if(bytes > 0) {
                break;
            } else {
                if(bytes < 0 && SSL_get_error(socket->ssl, bytes) == SSL_ERROR_WANT_WRITE)
                    continue;

                wi_error_set_openssl_ssl_error_with_result(socket->ssl, bytes);
                
                break;
            }
        }
        
        ERR_clear_error();

        return bytes;
    } else {
#endif
        offset = 0;
        
        while(offset < length) {
            if(timeout > 0.0) {
                state = wi_socket_wait_descriptor(socket->sd, timeout, false, true);

                if(state != WI_SOCKET_READY) {
                    if(state == WI_SOCKET_TIMEOUT)
                        wi_error_set_errno(ETIMEDOUT);
                    
                    return -1;
                }
            }

            bytes = write(socket->sd, buffer + offset, length - offset);
            
            if(bytes > 0) {
                offset += bytes;
            } else {
                if(bytes < 0)
                    wi_error_set_errno(errno);
                else
                    wi_error_set_libwired_error(WI_ERROR_SOCKET_EOF);
                
                return bytes;
            }
        }
        
        return offset;
#ifdef HAVE_OPENSSL_SSL_H
    }
#endif
    
    return 0;
}



wi_data_t * wi_socket_read_data(wi_socket_t *socket, wi_time_interval_t timeout, wi_uinteger_t length) {
    wi_integer_t    bytes;
    char            *buffer;
    
    buffer = wi_malloc(length);
    bytes = wi_socket_read_bytes(socket, timeout, buffer, length);
    
    if(bytes <= 0) {
        wi_free(buffer);
        
        if(bytes == 0)
            return wi_data();
        else
            return NULL;
    }
    
    return wi_data_with_bytes_no_copy(buffer, bytes, true);
}



wi_integer_t wi_socket_read_bytes(wi_socket_t *socket, wi_time_interval_t timeout, void *buffer, wi_uinteger_t length) {
#ifdef HAVE_OPENSSL_SSL_H
    wi_socket_state_t   state;
#endif
    wi_uinteger_t       offset;
    wi_integer_t        bytes;
    
    WI_ASSERT(buffer != NULL, "buffer of length %u should not be NULL", length);
    WI_ASSERT(socket->sd >= 0, "socket %@ should be valid", socket);
    
#ifdef HAVE_OPENSSL_SSL_H
    if(socket->ssl) {
        while(true) {
            if(timeout > 0.0 && SSL_pending(socket->ssl) == 0) {
                state = wi_socket_wait_descriptor(socket->sd, timeout, true, false);

                if(state != WI_SOCKET_READY) {
                    if(state == WI_SOCKET_TIMEOUT)
                        wi_error_set_errno(ETIMEDOUT);
                    
                    return -1;
                }
            }
            
            ERR_clear_error();
            
            bytes = SSL_read(socket->ssl, buffer, length);
            
            if(bytes > 0) {
                break;
            } else {
                if(bytes < 0 && SSL_get_error(socket->ssl, bytes) == SSL_ERROR_WANT_READ)
                    continue;

                wi_error_set_openssl_ssl_error_with_result(socket->ssl, bytes);
                
                break;
            }
        }
        
        ERR_clear_error();
        
        return bytes;
    } else {
#endif
        offset = 0;
        
        while(offset < length) {
            bytes = _wi_socket_read_bytes(socket, timeout, buffer + offset, length - offset);
            
            if(bytes <= 0)
                return bytes;
            
            offset += bytes;
        }
        
        return offset;
#ifdef HAVE_OPENSSL_SSL_H
    }
#endif

    return 0;
}



static wi_integer_t _wi_socket_read_bytes(wi_socket_t *socket, wi_time_interval_t timeout, void *buffer, wi_uinteger_t length) {
    wi_socket_state_t   state;
    wi_integer_t        bytes;
    
    if(timeout > 0.0) {
        state = wi_socket_wait_descriptor(socket->sd, timeout, true, false);
        
        if(state != WI_SOCKET_READY) {
            if(state == WI_SOCKET_TIMEOUT)
                wi_error_set_errno(ETIMEDOUT);
            
            return -1;
        }
    }
    
    bytes = read(socket->sd, buffer, length);
    
    if(bytes <= 0) {
        if(bytes < 0)
            wi_error_set_errno(errno);
        else
            wi_error_set_libwired_error(WI_ERROR_SOCKET_EOF);
    }
    
    return bytes;
    
    return 0;
}
