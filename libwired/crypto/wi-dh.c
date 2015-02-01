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

#ifndef WI_DH

int wi_dh_dummy = 0;

#else

#include <wired/wi-data.h>
#include <wired/wi-private.h>
#include <wired/wi-dh.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

#include <openssl/dh.h>

struct _wi_dh {
    wi_runtime_base_t                   base;
    
    DH                                  *dh;
};

static void                             _wi_dh_dealloc(wi_runtime_instance_t *);
static wi_runtime_instance_t *          _wi_dh_copy(wi_runtime_instance_t *);
static wi_string_t *                    _wi_dh_description(wi_runtime_instance_t *);

static wi_runtime_id_t                  _wi_dh_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_dh_runtime_class = {
    "wi_dh_t",
    _wi_dh_dealloc,
    _wi_dh_copy,
    NULL,
    _wi_dh_description,
    NULL
};



void wi_dh_register(void) {
    _wi_dh_runtime_id = wi_runtime_register_class(&_wi_dh_runtime_class);
}



void wi_dh_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_dh_runtime_id(void) {
    return _wi_dh_runtime_id;
}



#pragma mark -

wi_dh_t * wi_dh_alloc(void) {
    return wi_runtime_create_instance(_wi_dh_runtime_id, sizeof(wi_dh_t));
}



wi_dh_t * wi_dh_init_with_bits(wi_dh_t *dh, wi_uinteger_t size) {
    dh->dh = DH_new();
    
    if(!dh->dh) {
        wi_error_set_openssl_error();
        
        wi_release(dh);
        
        return NULL;
    }
    
    if(DH_generate_parameters_ex(dh->dh, size, DH_GENERATOR_2, NULL) != 1) {
        wi_error_set_openssl_error();
        
        wi_release(dh);
        
        return NULL;
    }
    
    return dh;
}



wi_dh_t * wi_dh_init_with_data(wi_dh_t *dh, wi_data_t *p, wi_data_t *g) {
    dh->dh = DH_new();
    
    if(!dh->dh) {
        wi_error_set_openssl_error();
        
        wi_release(dh);
        
        return NULL;
    }
    
    dh->dh->p = BN_bin2bn(wi_data_bytes(p), wi_data_length(p), NULL);
    dh->dh->g = BN_bin2bn(wi_data_bytes(g), wi_data_length(g), NULL);
    
    if(!dh->dh->p || !dh->dh->g) {
        wi_error_set_openssl_error();
        
        wi_release(dh);
        
        return NULL;
    }
    
    return dh;
}



static void _wi_dh_dealloc(wi_runtime_instance_t *instance) {
    wi_dh_t    *dh = instance;
    
    if(dh->dh)
        DH_free(dh->dh);
}



static wi_runtime_instance_t * _wi_dh_copy(wi_runtime_instance_t *instance) {
    wi_dh_t     *dh = instance;
    wi_data_t   *p, *g;
    
    wi_dh_get_data(dh, &p, &g);
    
    return wi_dh_init_with_data(wi_dh_alloc(), p, g);
}



static wi_string_t * _wi_dh_description(wi_runtime_instance_t *instance) {
    wi_dh_t     *dh = instance;
    wi_data_t   *p, *g;
    
    wi_dh_get_data(dh, &p, &g);
    
    return wi_string_with_format(WI_STR("<%@ %p>{p = %@, g = %@}"),
        wi_runtime_class_name(dh),
        dh,
        p,
        g);
}



#pragma mark -

void wi_dh_get_data(wi_dh_t *dh, wi_data_t **p, wi_data_t **g) {
    unsigned char   *p_buffer, *g_buffer;
    int             p_length, g_length;
    
    p_length = BN_num_bytes(dh->dh->p);
    g_length = BN_num_bytes(dh->dh->g);
    
    p_buffer = wi_malloc(p_length);
    g_buffer = wi_malloc(g_length);
    
    BN_bn2bin(dh->dh->p, p_buffer);
    BN_bn2bin(dh->dh->g, g_buffer);
    
    *p = wi_data_with_bytes_no_copy(p_buffer, p_length, true);
    *g = wi_data_with_bytes_no_copy(g_buffer, g_length, true);
}



#pragma mark -

void * wi_dh_openssl_dh(wi_dh_t *dh) {
    return dh->dh;
}

#endif
