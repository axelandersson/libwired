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

#ifndef WI_REGEXP_H
#define WI_REGEXP_H 1

#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

struct _wi_regexp_match {
    wi_range_t                          range;
};
typedef struct _wi_regexp_match         wi_regexp_match_t;

enum _wi_regexp_options {
    WI_REGEXP_CASE_INSENSITIVE          = (1 << 0),
    WI_REGEXP_NEWLINE_SENSITIVE         = (1 << 1),
};
typedef enum _wi_regexp_options         wi_regexp_options_t;


WI_EXPORT wi_runtime_id_t               wi_regexp_runtime_id(void);

WI_EXPORT wi_regexp_t *                 wi_regexp_with_pattern(wi_string_t *, wi_regexp_options_t);

WI_EXPORT wi_regexp_t *                 wi_regexp_alloc(void);
WI_EXPORT wi_regexp_t *                 wi_regexp_init_with_pattern(wi_regexp_t *, wi_string_t *, wi_regexp_options_t);

WI_EXPORT wi_string_t *                 wi_regexp_pattern(wi_regexp_t *);
WI_EXPORT wi_regexp_options_t           wi_regexp_options(wi_regexp_t *);
WI_EXPORT wi_uinteger_t                 wi_regexp_number_of_capture_groups(wi_regexp_t *);

WI_EXPORT wi_uinteger_t                 wi_regexp_number_of_matches_in_string(wi_regexp_t *, wi_string_t *);
WI_EXPORT wi_range_t                    wi_regexp_range_of_first_match_in_string(wi_regexp_t *, wi_string_t *);
WI_EXPORT wi_string_t *                 wi_regexp_string_of_first_match_in_string(wi_regexp_t *, wi_string_t *);
WI_EXPORT wi_uinteger_t                 wi_regexp_get_matches_in_string(wi_regexp_t *, wi_string_t *, wi_regexp_match_t *, wi_uinteger_t);

WI_EXPORT wi_uinteger_t                 wi_regexp_replace_matches_in_string(wi_regexp_t *, wi_mutable_string_t *, wi_string_t *);
WI_EXPORT wi_string_t *                 wi_regexp_string_by_replacing_matches_in_string(wi_regexp_t *, wi_string_t *, wi_string_t *);

#endif /* WI_REGEXP_H */
