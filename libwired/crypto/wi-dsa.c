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

#ifndef WI_DSA

int wi_dsa_dummy = 0;

#else

#include <wired/wi-data.h>
#include <wired/wi-private.h>
#include <wired/wi-dsa.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

#ifdef HAVE_OPENSSL_SHA_H
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/dsa.h>
#endif

struct _wi_dsa {
    wi_runtime_base_t                   base;
    
    DSA                                 *dsa;
    wi_data_t                           *public_key;
    wi_data_t                           *private_key;
};

static void                             _wi_dsa_dealloc(wi_runtime_instance_t *);
static wi_runtime_instance_t *          _wi_dsa_copy(wi_runtime_instance_t *);
static wi_string_t *                    _wi_dsa_description(wi_runtime_instance_t *);

static wi_runtime_id_t                  _wi_dsa_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_dsa_runtime_class = {
    "wi_dsa_t",
    _wi_dsa_dealloc,
    _wi_dsa_copy,
    NULL,
    _wi_dsa_description,
    NULL
};



void wi_dsa_register(void) {
    _wi_dsa_runtime_id = wi_runtime_register_class(&_wi_dsa_runtime_class);
}



void wi_dsa_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_dsa_runtime_id(void) {
    return _wi_dsa_runtime_id;
}



#pragma mark -

wi_dsa_t * wi_dsa_alloc(void) {
    return wi_runtime_create_instance(_wi_dsa_runtime_id, sizeof(wi_dsa_t));
}



wi_dsa_t * wi_dsa_init_with_bits(wi_dsa_t *dsa, wi_uinteger_t size) {
    dsa->dsa = DSA_generate_parameters(512, NULL, 0, NULL, NULL, NULL, NULL);

    if(!dsa->dsa) {
        wi_error_set_openssl_error();
        
        wi_release(dsa);
        
        return NULL;
    }
    
    if(DSA_generate_key(dsa->dsa) != 1) {
        wi_error_set_openssl_error();
        
        wi_release(dsa);
        
        return NULL;
    }
    
    return dsa;
}



wi_dsa_t * wi_dsa_init_with_pem_file(wi_dsa_t *dsa, wi_string_t *path) {
    FILE    *fp;
    
    fp = fopen(wi_string_utf8_string(path), "r");
    
    if(!fp) {
        wi_error_set_errno(errno);
        
        wi_release(dsa);
        
        return NULL;
    }
    
    dsa->dsa = PEM_read_DSAPrivateKey(fp, NULL, NULL, NULL);
    
    fclose(fp);
    
    if(!dsa->dsa) {
        wi_error_set_openssl_error();
        
        wi_release(dsa);
        
        return NULL;
    }
    
    return dsa;
}



wi_dsa_t * wi_dsa_init_with_private_key(wi_dsa_t *dsa, wi_data_t *data) {
    const unsigned char     *buffer;
    long                    length;
    
    buffer = wi_data_bytes(data);
    length = wi_data_length(data);
    
    dsa->dsa = d2i_DSAPrivateKey(NULL, &buffer, length);

    if(!dsa->dsa) {
        wi_error_set_openssl_error();
        
        wi_release(dsa);
        
        return NULL;
    }
    
    dsa->private_key = wi_retain(data);
    
    return dsa;
}



wi_dsa_t * wi_dsa_init_with_public_key(wi_dsa_t *dsa, wi_data_t *data) {
    const unsigned char     *buffer;
    long                    length;
    
    buffer = wi_data_bytes(data);
    length = wi_data_length(data);
    
    dsa->dsa = d2i_DSAPublicKey(NULL, (const unsigned char **) &buffer, length);

    if(!dsa->dsa) {
        wi_error_set_openssl_error();
        
        wi_release(dsa);
        
        return NULL;
    }
    
    dsa->public_key = wi_retain(data);
    
    return dsa;
}



static void _wi_dsa_dealloc(wi_runtime_instance_t *instance) {
    wi_dsa_t    *dsa = instance;
    
    if(dsa->dsa)
        DSA_free(dsa->dsa);
    
    wi_release(dsa->public_key);
    wi_release(dsa->private_key);
}



static wi_runtime_instance_t * _wi_dsa_copy(wi_runtime_instance_t *instance) {
    wi_dsa_t    *dsa = instance;
    
    return wi_dsa_init_with_private_key(wi_dsa_alloc(), wi_dsa_private_key(dsa));
}



static wi_string_t * _wi_dsa_description(wi_runtime_instance_t *instance) {
    wi_dsa_t    *dsa = instance;
    
    return wi_string_with_format(WI_STR("<%@ %p>{key = %p, bits = %lu}"),
        wi_runtime_class_name(dsa),
        dsa,
        dsa->dsa,
        wi_dsa_bits(dsa));
}



#pragma mark -

wi_data_t * wi_dsa_public_key(wi_dsa_t *dsa) {
    unsigned char   *buffer;
    int             length;

    if(!dsa->public_key) {
        buffer = NULL;
        length = i2d_DSAPublicKey(dsa->dsa, &buffer);
        
        if(length <= 0) {
            wi_error_set_openssl_error();
            
            return NULL;
        }
        
        dsa->public_key = wi_data_init_with_bytes(wi_data_alloc(), buffer, length);

        OPENSSL_free(buffer);
    }
    
    return dsa->public_key;
}



wi_data_t * wi_dsa_private_key(wi_dsa_t *dsa) {
    unsigned char   *buffer;
    int             length;

    if(!dsa->private_key) {
        buffer = NULL;
        length = i2d_DSAPrivateKey(dsa->dsa, &buffer);
        
        if(length <= 0) {
            wi_error_set_openssl_error();
            
            return NULL;
        }
        
        dsa->private_key = wi_data_init_with_bytes(wi_data_alloc(), buffer, length);

        OPENSSL_free(buffer);
    }
    
    return dsa->private_key;
}



wi_uinteger_t wi_dsa_bits(wi_dsa_t *dsa) {
    return DSA_size(dsa->dsa) * 8;
}



#pragma mark -

void * wi_dsa_openssl_dsa(wi_dsa_t *dsa) {
    return dsa->dsa;
}

#endif
