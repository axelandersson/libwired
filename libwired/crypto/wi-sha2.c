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

#ifndef WI_SHA2

int wi_sha2_dummy = 1;

#else

#include <wired/wi-assert.h>
#include <wired/wi-data.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-sha2.h>
#include <wired/wi-string.h>

#ifdef HAVE_COMMONCRYPTO_COMMONDIGEST_H
#define WI_SHA2_COMMONCRYPTO                1
#else
#define WI_SHA2_OPENSSL                     1
#endif

#include <string.h>

#ifdef HAVE_OPENSSL_SHA_H
#include <openssl/sha.h>
#endif

#ifdef HAVE_COMMONCRYPTO_COMMONDIGEST_H
#include <CommonCrypto/CommonDigest.h>
#endif


#define _WI_SHA2_ASSERT_OPEN(sha2) \
    WI_ASSERT(!(sha2)->closed, "%@ has been closed", (sha2))

#define _WI_SHA2_ASSERT_CLOSED(sha2) \
    WI_ASSERT((sha2)->closed, "%@ is open", (sha2))


struct _wi_sha2_ctx {
    wi_sha2_bits_t                          bits;
    
#ifdef WI_SHA2_OPENSSL
    SHA256_CTX                              openssl_256_ctx;
    SHA512_CTX                              openssl_512_ctx;
#endif
    
#ifdef WI_SHA2_COMMONCRYPTO
    CC_SHA256_CTX                           commondigest_256_ctx;
    CC_SHA512_CTX                           commondigest_512_ctx;
#endif
};
typedef struct _wi_sha2_ctx                 _wi_sha2_ctx_t;


struct _wi_sha2 {
    wi_runtime_base_t                       base;
    
    _wi_sha2_ctx_t                          ctx;
    
    unsigned char                           buffer[WI_SHA2_MAX_LENGTH];
    wi_uinteger_t                           length;
    
    wi_boolean_t                            closed;
};


static void                                 _wi_sha2_ctx_init(wi_sha2_bits_t, _wi_sha2_ctx_t *);
static void                                 _wi_sha2_ctx_update(_wi_sha2_ctx_t *, const void *, unsigned long);
static void                                 _wi_sha2_ctx_final(unsigned char *, _wi_sha2_ctx_t *);


static wi_runtime_id_t                      _wi_sha2_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t                   _wi_sha2_runtime_class = {
    "wi_sha2_t",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};



void wi_sha2_register(void) {
    _wi_sha2_runtime_id = wi_runtime_register_class(&_wi_sha2_runtime_class);
}



void wi_sha2_initialize(void) {
}



#pragma mark -

static void _wi_sha2_ctx_init(wi_sha2_bits_t bits, _wi_sha2_ctx_t *ctx) {
    ctx->bits = bits;
    
#ifdef WI_SHA2_OPENSSL
    switch(ctx->bits) {
        case WI_SHA2_224:
            SHA224_Init(&ctx->openssl_256_ctx);
            break;
            
        case WI_SHA2_256:
            SHA256_Init(&ctx->openssl_256_ctx);
            break;
            
        case WI_SHA2_384:
            SHA384_Init(&ctx->openssl_512_ctx);
            break;
            
        case WI_SHA2_512:
            SHA512_Init(&ctx->openssl_512_ctx);
            break;
    }
#endif

#ifdef WI_SHA2_COMMONCRYPTO
    switch(ctx->bits) {
        case WI_SHA2_224:
            CC_SHA224_Init(&ctx->commondigest_256_ctx);
            break;

        case WI_SHA2_256:
            CC_SHA256_Init(&ctx->commondigest_256_ctx);
            break;

        case WI_SHA2_384:
            CC_SHA384_Init(&ctx->commondigest_512_ctx);
            break;

        case WI_SHA2_512:
            CC_SHA512_Init(&ctx->commondigest_512_ctx);
            break;
    }
#endif
}



static void _wi_sha2_ctx_update(_wi_sha2_ctx_t *ctx, const void *data, unsigned long length) {
#ifdef WI_SHA2_OPENSSL
    switch(ctx->bits) {
        case WI_SHA2_224:
            SHA224_Update(&ctx->openssl_256_ctx, data, length);
            break;
            
        case WI_SHA2_256:
            SHA256_Update(&ctx->openssl_256_ctx, data, length);
            break;
            
        case WI_SHA2_384:
            SHA384_Update(&ctx->openssl_512_ctx, data, length);
            break;
            
        case WI_SHA2_512:
            SHA512_Update(&ctx->openssl_512_ctx, data, length);
            break;
    }
#endif

#ifdef WI_SHA2_COMMONCRYPTO
    switch(ctx->bits) {
        case WI_SHA2_224:
            CC_SHA224_Update(&ctx->commondigest_256_ctx, data, length);
            break;
            
        case WI_SHA2_256:
            CC_SHA256_Update(&ctx->commondigest_256_ctx, data, length);
            break;
            
        case WI_SHA2_384:
            CC_SHA384_Update(&ctx->commondigest_512_ctx, data, length);
            break;
            
        case WI_SHA2_512:
            CC_SHA512_Update(&ctx->commondigest_512_ctx, data, length);
            break;
    }
#endif
}



static void _wi_sha2_ctx_final(unsigned char *buffer, _wi_sha2_ctx_t *ctx) {
#ifdef WI_SHA2_OPENSSL
    switch(ctx->bits) {
        case WI_SHA2_224:
            SHA224_Final(buffer, &ctx->openssl_256_ctx);
            break;
            
        case WI_SHA2_256:
            SHA256_Final(buffer, &ctx->openssl_256_ctx);
            break;
            
        case WI_SHA2_384:
            SHA384_Final(buffer, &ctx->openssl_512_ctx);
            break;
            
        case WI_SHA2_512:
            SHA512_Final(buffer, &ctx->openssl_512_ctx);
            break;
    }
#endif

#ifdef WI_SHA2_COMMONCRYPTO
    switch(ctx->bits) {
        case WI_SHA2_224:
            CC_SHA224_Final(buffer, &ctx->commondigest_256_ctx);
            break;
            
        case WI_SHA2_256:
            CC_SHA256_Final(buffer, &ctx->commondigest_256_ctx);
            break;
            
        case WI_SHA2_384:
            CC_SHA384_Final(buffer, &ctx->commondigest_512_ctx);
            break;
            
        case WI_SHA2_512:
            CC_SHA512_Final(buffer, &ctx->commondigest_512_ctx);
            break;
    }
#endif
}



#pragma mark -

void wi_sha2_digest(wi_sha2_bits_t bits, const void *data, wi_uinteger_t length, unsigned char *buffer) {
    _wi_sha2_ctx_t  c;

    _wi_sha2_ctx_init(bits, &c);
    _wi_sha2_ctx_update(&c, data, length);
    _wi_sha2_ctx_final(buffer, &c);
}



wi_string_t * wi_sha2_digest_string(wi_sha2_bits_t bits, wi_data_t *data) {
    wi_sha2_t   *sha2;
    
    sha2 = wi_sha2_with_bits(bits);
    
    wi_sha2_update(sha2, wi_data_bytes(data), wi_data_length(data));
    wi_sha2_close(sha2);
    
    return wi_sha2_string(sha2);
}



#pragma mark -

wi_runtime_id_t wi_sha2_runtime_id(void) {
    return _wi_sha2_runtime_id;
}



#pragma mark -

wi_sha2_t * wi_sha2_with_bits(wi_sha2_bits_t bits) {
    return wi_autorelease(wi_sha2_init_with_bits(wi_sha2_alloc(), bits));
}



#pragma mark -

wi_sha2_t * wi_sha2_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_sha2_runtime_id, sizeof(wi_sha2_t), 0);
}



wi_sha2_t * wi_sha2_init_with_bits(wi_sha2_t *sha2, wi_sha2_bits_t bits) {
    _wi_sha2_ctx_init(bits, &sha2->ctx);
    
    switch(bits) {
        case WI_SHA2_224:
            sha2->length = WI_SHA2_224_LENGTH;
            break;

        case WI_SHA2_256:
            sha2->length = WI_SHA2_256_LENGTH;
            break;

        case WI_SHA2_384:
            sha2->length = WI_SHA2_384_LENGTH;
            break;

        case WI_SHA2_512:
            sha2->length = WI_SHA2_512_LENGTH;
            break;
    }
    
    return sha2;
}



#pragma mark -

void wi_sha2_update(wi_sha2_t *sha2, const void *data, wi_uinteger_t length) {
    _WI_SHA2_ASSERT_OPEN(sha2);
    
    _wi_sha2_ctx_update(&sha2->ctx, data, length);
}



void wi_sha2_close(wi_sha2_t *sha2) {
    _WI_SHA2_ASSERT_OPEN(sha2);
    
    _wi_sha2_ctx_final(sha2->buffer, &sha2->ctx);
    
    sha2->closed = true;
}



#pragma mark -

void wi_sha2_get_data(wi_sha2_t *sha2, unsigned char *buffer) {
    _WI_SHA2_ASSERT_CLOSED(sha2);
    
    memcpy(buffer, sha2->buffer, sha2->length);
}



wi_data_t * wi_sha2_data(wi_sha2_t *sha2) {
    _WI_SHA2_ASSERT_CLOSED(sha2);
    
    return wi_data_with_bytes(sha2->buffer, sha2->length);
}



wi_string_t * wi_sha2_string(wi_sha2_t *sha2) {
    static unsigned char    hex[] = "0123456789abcdef";
    char                    sha2_hex[sha2->length * 2 + 1];
    wi_uinteger_t           i;

    _WI_SHA2_ASSERT_CLOSED(sha2);
    
    for(i = 0; i < sha2->length; i++) {
        sha2_hex[i + i]         = hex[sha2->buffer[i] >> 4];
        sha2_hex[i + i + 1]     = hex[sha2->buffer[i] & 0x0F];
    }

    sha2_hex[i+i] = '\0';

    return wi_string_with_utf8_string(sha2_hex);
}

#endif
