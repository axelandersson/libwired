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

#ifndef WI_SHA2_H
#define WI_SHA2_H 1

#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

#define WI_SHA2_224_LENGTH              28
#define WI_SHA2_256_LENGTH              32
#define WI_SHA2_384_LENGTH              48
#define WI_SHA2_512_LENGTH              64
#define WI_SHA2_MAX_LENGTH              WI_SHA2_512_LENGTH


enum _wi_sha2_bits {
    WI_SHA2_224,
    WI_SHA2_256,
    WI_SHA2_384,
    WI_SHA2_512
};
typedef enum _wi_sha2_bits              wi_sha2_bits_t;


WI_EXPORT void                          wi_sha2_digest(wi_sha2_bits_t, const void *, wi_uinteger_t, unsigned char *);
WI_EXPORT wi_string_t *                 wi_sha2_digest_string(wi_sha2_bits_t, wi_data_t *);

WI_EXPORT wi_runtime_id_t               wi_sha2_runtime_id(void);

WI_EXPORT wi_sha2_t *                   wi_sha2_with_bits(wi_sha2_bits_t);

WI_EXPORT wi_sha2_t *                   wi_sha2_alloc(void);
WI_EXPORT wi_sha2_t *                   wi_sha2_init_with_bits(wi_sha2_t *, wi_sha2_bits_t);

WI_EXPORT void                          wi_sha2_update(wi_sha2_t *, const void *, wi_uinteger_t);
WI_EXPORT void                          wi_sha2_close(wi_sha2_t *);

WI_EXPORT void                          wi_sha2_get_data(wi_sha2_t *, unsigned char *);
WI_EXPORT wi_data_t *                   wi_sha2_data(wi_sha2_t *);
WI_EXPORT wi_string_t *                 wi_sha2_string(wi_sha2_t *);
WI_EXPORT wi_sha2_bits_t                wi_sha2_bits(wi_sha2_t);

#endif /* WI_SHA2_H */
