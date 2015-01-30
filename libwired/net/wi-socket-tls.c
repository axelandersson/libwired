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

#ifdef HAVE_OPENSSL_SSL_H
#include <openssl/err.h>
#include <openssl/ssl.h>
#endif

#include <wired/wi-assert.h>
#include <wired/wi-date.h>
#include <wired/wi-dh.h>
#include <wired/wi-macros.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-rsa.h>
#include <wired/wi-socket-tls.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>
#include <wired/wi-x509.h>

struct _wi_socket_tls {
    wi_runtime_base_t                   base;
    
    SSL_CTX                             *ssl_ctx;
    
    wi_x509_t                           *certificate;
    wi_rsa_t                            *private_key;
    wi_dh_t                             *dh;
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
    
    wi_release(tls->certificate);
    wi_release(tls->private_key);
    wi_release(tls->dh);
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




void wi_socket_tls_set_dh(wi_socket_tls_t *tls, wi_dh_t *dh) {
    wi_release(tls->dh);
    tls->dh = wi_retain(dh);
}



wi_dh_t * wi_socket_tls_dh(wi_socket_tls_t *tls) {
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
