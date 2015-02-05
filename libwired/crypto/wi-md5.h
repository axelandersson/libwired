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

#ifndef WI_MD5_H
#define WI_MD5_H 1

#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

#define WI_MD5_LENGTH                   16


WI_EXPORT void                          wi_md5_digest(const void *, wi_uinteger_t, unsigned char *);
WI_EXPORT wi_string_t *                 wi_md5_digest_string(wi_data_t *);

WI_EXPORT wi_runtime_id_t               wi_md5_runtime_id(void);

WI_EXPORT wi_md5_t *                    wi_md5(void);

WI_EXPORT wi_md5_t *                    wi_md5_alloc(void);
WI_EXPORT wi_md5_t *                    wi_md5_init(wi_md5_t *);

WI_EXPORT void                          wi_md5_update(wi_md5_t *, const void *, wi_uinteger_t);
WI_EXPORT void                          wi_md5_close(wi_md5_t *);

WI_EXPORT void                          wi_md5_get_data(wi_md5_t *, unsigned char *);
WI_EXPORT wi_data_t *                   wi_md5_data(wi_md5_t *);
WI_EXPORT wi_string_t *                 wi_md5_string(wi_md5_t *);

#endif /* WI_MD5_H */
