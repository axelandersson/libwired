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

#include <wired/wi-data.h>
#include <wired/wi-digest.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

#if defined(HAVE_OPENSSL_SHA_H) || defined(HAVE_COMMONCRYPTO_COMMONDIGEST_H)

#ifdef HAVE_COMMONCRYPTO_COMMONDIGEST_H
#define WI_DIGEST_COMMONCRYPTO              1
#else
#define WI_DIGEST_OPENSSL                   1
#endif

#include <string.h>

#ifdef HAVE_OPENSSL_SHA_H
#include <openssl/md5.h>
#include <openssl/sha.h>
#endif

#ifdef HAVE_COMMONCRYPTO_COMMONDIGEST_H
#include <CommonCrypto/CommonDigest.h>
#endif


#define _WI_DIGEST_ASSERT_OPEN(digest) \
    WI_ASSERT(!(digest)->closed, "%@ has been closed", (digest))

#define _WI_DIGEST_ASSERT_CLOSED(digest) \
    WI_ASSERT((digest)->closed, "%@ is open", (digest))


struct _wi_md5_ctx {
#ifdef WI_DIGEST_OPENSSL
    MD5_CTX                                 openssl_ctx;
#endif
    
#ifdef WI_DIGEST_COMMONCRYPTO
    CC_MD5_CTX                              commondigest_ctx;
#endif
};
typedef struct _wi_md5_ctx                  _wi_md5_ctx_t;


struct _wi_sha1_ctx {
#ifdef WI_DIGEST_OPENSSL
    SHA_CTX                                 openssl_ctx;
#endif
    
#ifdef WI_DIGEST_COMMONCRYPTO
    CC_SHA1_CTX                             commondigest_ctx;
#endif
};
typedef struct _wi_sha1_ctx                 _wi_sha1_ctx_t;


struct _wi_md5 {
    wi_runtime_base_t                       base;
    
    _wi_md5_ctx_t                           ctx;
    
    unsigned char                           buffer[WI_MD5_DIGEST_LENGTH];
    
    wi_boolean_t                            closed;
};


struct _wi_sha1 {
    wi_runtime_base_t                       base;
    
    _wi_sha1_ctx_t                          ctx;
    
    unsigned char                           buffer[WI_SHA1_DIGEST_LENGTH];
    
    wi_boolean_t                            closed;
};


static void                                 _wi_md5_ctx_init(_wi_md5_ctx_t *);
static void                                 _wi_md5_ctx_update(_wi_md5_ctx_t *, const void *, unsigned long);
static void                                 _wi_md5_ctx_final(unsigned char *, _wi_md5_ctx_t *);


static wi_runtime_id_t                      _wi_md5_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t                   _wi_md5_runtime_class = {
    "wi_md5_t",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};

static wi_runtime_id_t                      _wi_sha1_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t                   _wi_sha1_runtime_class = {
    "wi_sha1_t",
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};



void wi_digest_register(void) {
    _wi_md5_runtime_id      = wi_runtime_register_class(&_wi_md5_runtime_class);
    _wi_sha1_runtime_id     = wi_runtime_register_class(&_wi_sha1_runtime_class);
}



void wi_digest_initialize(void) {
}



#pragma mark -

static void _wi_md5_ctx_init(_wi_md5_ctx_t *ctx) {
#ifdef WI_DIGEST_OPENSSL
    MD5_Init(&ctx->openssl_ctx);
#endif

#ifdef WI_DIGEST_COMMONCRYPTO
    CC_MD5_Init(&ctx->commondigest_ctx);
#endif
}



static void _wi_md5_ctx_update(_wi_md5_ctx_t *ctx, const void *data, unsigned long length) {
#ifdef WI_DIGEST_OPENSSL
    MD5_Update(&ctx->openssl_ctx, data, length);
#endif

#ifdef WI_DIGEST_COMMONCRYPTO
    CC_MD5_Update(&ctx->commondigest_ctx, data, length);
#endif
}



static void _wi_md5_ctx_final(unsigned char *buffer, _wi_md5_ctx_t *ctx) {
#ifdef WI_DIGEST_OPENSSL
    MD5_Final(buffer, &ctx->openssl_ctx);
#endif
    
#ifdef WI_DIGEST_COMMONCRYPTO
    CC_MD5_Final(buffer, &ctx->commondigest_ctx);
#endif
}



#pragma mark -

static void _wi_sha1_ctx_init(_wi_sha1_ctx_t *ctx) {
#ifdef WI_DIGEST_OPENSSL
    SHA1_Init(&ctx->openssl_ctx);
#endif

#ifdef WI_DIGEST_COMMONCRYPTO
    CC_SHA1_Init(&ctx->commondigest_ctx);
#endif
}



static void _wi_sha1_ctx_update(_wi_sha1_ctx_t *ctx, const void *data, unsigned long length) {
#ifdef WI_DIGEST_OPENSSL
    SHA1_Update(&ctx->openssl_ctx, data, length);
#endif

#ifdef WI_DIGEST_COMMONCRYPTO
    CC_SHA1_Update(&ctx->commondigest_ctx, data, length);
#endif
}



static void _wi_sha1_ctx_final(unsigned char *buffer, _wi_sha1_ctx_t *ctx) {
#ifdef WI_DIGEST_OPENSSL
    SHA1_Final(buffer, &ctx->openssl_ctx);
#endif

#ifdef WI_DIGEST_COMMONCRYPTO
    CC_SHA1_Final(buffer, &ctx->commondigest_ctx);
#endif
}



#pragma mark -

void wi_md5_digest(const void *data, wi_uinteger_t length, unsigned char *buffer) {
    _wi_md5_ctx_t   c;

    _wi_md5_ctx_init(&c);
    _wi_md5_ctx_update(&c, data, length);
    _wi_md5_ctx_final(buffer, &c);
}



wi_string_t * wi_md5_digest_string(wi_data_t *data) {
    wi_md5_t    *md5;
    
    md5 = wi_md5();
    
    wi_md5_update(md5, wi_data_bytes(data), wi_data_length(data));
    wi_md5_close(md5);
    
    return wi_md5_string(md5);
}



#pragma mark -

wi_runtime_id_t wi_md5_runtime_id(void) {
    return _wi_md5_runtime_id;
}



#pragma mark -

wi_md5_t * wi_md5(void) {
    return wi_autorelease(wi_md5_init(wi_md5_alloc()));
}



#pragma mark -

wi_md5_t * wi_md5_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_md5_runtime_id, sizeof(wi_md5_t), 0);
}



wi_md5_t * wi_md5_init(wi_md5_t *md5) {
    _wi_md5_ctx_init(&md5->ctx);
    
    return md5;
}



#pragma mark -

void wi_md5_update(wi_md5_t *md5, const void *data, wi_uinteger_t length) {
    _WI_DIGEST_ASSERT_OPEN(md5);
    
    _wi_md5_ctx_update(&md5->ctx, data, length);
}



void wi_md5_close(wi_md5_t *md5) {
    _WI_DIGEST_ASSERT_OPEN(md5);
    
    _wi_md5_ctx_final(md5->buffer, &md5->ctx);
    
    md5->closed = true;
}



#pragma mark -

void wi_md5_get_data(wi_md5_t *md5, unsigned char *buffer) {
    _WI_DIGEST_ASSERT_CLOSED(md5);
    
    memcpy(buffer, md5->buffer, sizeof(md5->buffer));
}



wi_data_t * wi_md5_data(wi_md5_t *md5) {
    _WI_DIGEST_ASSERT_CLOSED(md5);
    
    return wi_data_with_bytes(md5->buffer, sizeof(md5->buffer));
}



wi_string_t * wi_md5_string(wi_md5_t *md5) {
    static unsigned char    hex[] = "0123456789abcdef";
    char                    md5_hex[sizeof(md5->buffer) * 2 + 1];
    wi_uinteger_t           i;

    _WI_DIGEST_ASSERT_CLOSED(md5);
    
    for(i = 0; i < sizeof(md5->buffer); i++) {
        md5_hex[i + i]      = hex[md5->buffer[i] >> 4];
        md5_hex[i + i + 1]  = hex[md5->buffer[i] & 0x0F];
    }

    md5_hex[i+i] = '\0';

    return wi_string_with_utf8_string(md5_hex);
}



#pragma mark -

void wi_sha1_digest(const void *data, wi_uinteger_t length, unsigned char *buffer) {
    _wi_sha1_ctx_t  c;

    _wi_sha1_ctx_init(&c);
    _wi_sha1_ctx_update(&c, data, length);
    _wi_sha1_ctx_final(buffer, &c);
}



wi_string_t * wi_sha1_digest_string(wi_data_t *data) {
    wi_sha1_t   *sha1;
    
    sha1 = wi_sha1();
    
    wi_sha1_update(sha1, wi_data_bytes(data), wi_data_length(data));
    wi_sha1_close(sha1);
    
    return wi_sha1_string(sha1);
}



#pragma mark -

wi_runtime_id_t wi_sha1_runtime_id(void) {
    return _wi_sha1_runtime_id;
}



#pragma mark -

wi_sha1_t * wi_sha1(void) {
    return wi_autorelease(wi_sha1_init(wi_sha1_alloc()));
}



#pragma mark -

wi_sha1_t * wi_sha1_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_sha1_runtime_id, sizeof(wi_sha1_t), 0);
}



wi_sha1_t * wi_sha1_init(wi_sha1_t *sha1) {
    _wi_sha1_ctx_init(&sha1->ctx);
    
    return sha1;
}



#pragma mark -

void wi_sha1_update(wi_sha1_t *sha1, const void *data, wi_uinteger_t length) {
    _WI_DIGEST_ASSERT_OPEN(sha1);
    
    _wi_sha1_ctx_update(&sha1->ctx, data, length);
}



void wi_sha1_close(wi_sha1_t *sha1) {
    _WI_DIGEST_ASSERT_OPEN(sha1);
    
    _wi_sha1_ctx_final(sha1->buffer, &sha1->ctx);
    
    sha1->closed = true;
}



#pragma mark -

void wi_sha1_get_data(wi_sha1_t *sha1, unsigned char *buffer) {
    _WI_DIGEST_ASSERT_CLOSED(sha1);
    
    memcpy(buffer, sha1->buffer, sizeof(sha1->buffer));
}



wi_data_t * wi_sha1_data(wi_sha1_t *sha1) {
    _WI_DIGEST_ASSERT_CLOSED(sha1);
    
    return wi_data_with_bytes(sha1->buffer, sizeof(sha1->buffer));
}



wi_string_t * wi_sha1_string(wi_sha1_t *sha1) {
    static unsigned char    hex[] = "0123456789abcdef";
    char                    sha1_hex[sizeof(sha1->buffer) * 2 + 1];
    wi_uinteger_t           i;

    _WI_DIGEST_ASSERT_CLOSED(sha1);
    
    for(i = 0; i < sizeof(sha1->buffer); i++) {
        sha1_hex[i + i]         = hex[sha1->buffer[i] >> 4];
        sha1_hex[i + i + 1]     = hex[sha1->buffer[i] & 0x0F];
    }

    sha1_hex[i+i] = '\0';

    return wi_string_with_utf8_string(sha1_hex);
}

#endif
