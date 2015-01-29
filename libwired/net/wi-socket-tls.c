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

#ifndef WI_SSL

int wi_socket_tls_dummy = 1;

#else

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
#include <wired/wi-macros.h>
#include <wired/wi-lock.h>
#include <wired/wi-private.h>
#include <wired/wi-rsa.h>
#include <wired/wi-socket.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>
#include <wired/wi-thread.h>
#include <wired/wi-x509.h>

struct _wi_socket_tls {
    wi_runtime_base_t                   base;
    
    SSL_CTX                             *ssl_ctx;
    DH                                  *dh;
    
    wi_x509_t                           *certificate;
    wi_rsa_t                            *private_key;
    wi_string_t                         *ciphers;
};


static void                             _wi_socket_tls_dealloc(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_socket_tls_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_socket_tls_runtime_class = {
    "wi_socket_tls_t",
    _wi_socket_tls_dealloc,
    NULL,
    NULL,
    NULL,
    NULL
};



void wi_socket_tls_register(void) {
    _wi_socket_tls_runtime_id = wi_runtime_register_class(&_wi_socket_tls_runtime_class);
}



void wi_socket_tls_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_socket_tls_runtime_id(void) {
    return _wi_socket_tls_runtime_id;
}



#pragma mark -

wi_socket_tls_t * wi_socket_tls_alloc(void) {
    return wi_runtime_create_instance(_wi_socket_tls_runtime_id, sizeof(wi_socket_tls_t));
}



wi_socket_tls_t * wi_socket_tls_init_with_type(wi_socket_tls_t *tls, wi_socket_tls_type_t type) {
    SSL_METHOD  *method;
    
    switch(type) {
        default:
        case WI_SOCKET_TLS_CLIENT:
            method = TLSv1_client_method();
            break;

        case WI_SOCKET_TLS_SERVER:
            method = TLSv1_server_method();
            break;
    }
    
    tls->ssl_ctx = SSL_CTX_new(method);
    
    if(!tls->ssl_ctx) {
        wi_error_set_openssl_error();
        
        wi_release(NULL);
        
        return NULL;
    }
    
    SSL_CTX_set_mode(tls->ssl_ctx, SSL_MODE_AUTO_RETRY);
    SSL_CTX_set_quiet_shutdown(tls->ssl_ctx, 1);
    
    return tls;
}



static void _wi_socket_tls_dealloc(wi_runtime_instance_t *instance) {
    wi_socket_tls_t     *tls = instance;
    
    if(tls->ssl_ctx)
        SSL_CTX_free(tls->ssl_ctx);
    
    if(tls->dh)
        DH_free(tls->dh);
    
    wi_release(tls->certificate);
    wi_release(tls->private_key);
    wi_release(tls->ciphers);
}



#pragma mark -

void * wi_socket_tls_ssl_context(wi_socket_tls_t *tls) {
    return tls->ssl_ctx;
}



#pragma mark -

wi_boolean_t wi_socket_tls_set_certificate(wi_socket_tls_t *tls, wi_x509_t *certificate) {
    wi_release(tls->certificate);
    tls->certificate = NULL;
    
    if(SSL_CTX_use_certificate(tls->ssl_ctx, wi_x509_x509(certificate)) != 1) {
        wi_error_set_openssl_error();

        return false;
    }
    
    tls->certificate = wi_retain(certificate);
    
    return true;
}



wi_x509_t * wi_socket_tls_certificate(wi_socket_tls_t *tls) {
    return tls->certificate;
}



wi_boolean_t wi_socket_tls_set_private_key(wi_socket_tls_t *tls, wi_rsa_t *rsa) {
    wi_release(tls->private_key);
    tls->private_key = NULL;
    
    if(SSL_CTX_use_RSAPrivateKey(tls->ssl_ctx, wi_rsa_rsa(rsa)) != 1) {
        wi_error_set_openssl_error();

        return false;
    }
    
    tls->private_key = wi_retain(rsa);
    
    return true;
}



wi_rsa_t * wi_socket_tls_private_key(wi_socket_tls_t *tls) {
    return tls->private_key;
}




wi_boolean_t wi_socket_tls_set_dh(wi_socket_tls_t *tls, const unsigned char *p, wi_uinteger_t p_size, const unsigned char *g, wi_uinteger_t g_size) {
    tls->dh = DH_new();
    
    if(!tls->dh) {
        wi_error_set_openssl_error();

        return false;
    }

    tls->dh->p = BN_bin2bn(p, p_size, NULL);
    tls->dh->g = BN_bin2bn(g, g_size, NULL);

    if(!tls->dh->p || !tls->dh->g) {
        wi_error_set_openssl_error();

        DH_free(tls->dh);
        tls->dh = NULL;
        
        return false;
    }
    
    return true;
}



void * wi_socket_tls_dh(wi_socket_tls_t *tls) {
    return tls->dh;
}


wi_boolean_t wi_socket_tls_set_ciphers(wi_socket_tls_t *tls, wi_string_t *ciphers) {
    wi_release(tls->ciphers);
    tls->ciphers = NULL;
    
    if(SSL_CTX_set_cipher_list(tls->ssl_ctx, wi_string_utf8_string(ciphers)) != 1) {
        wi_error_set_libwired_error(WI_ERROR_SOCKET_NOVALIDCIPHER);
        
        return false;
    }
    
    tls->ciphers = wi_retain(ciphers);
    
    return true;
}



wi_string_t * wi_socket_tls_ciphers(wi_socket_tls_t *tls) {
    return tls->ciphers;
}

#endif
