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

#ifndef WI_DIRECTORY_ENUMERATOR_H
#define WI_DIRECTORY_ENUMERATOR_H 1

#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

enum _wi_directory_enumerator_status {
    WI_DIRECTORY_ENUMERATOR_EOF,
    WI_DIRECTORY_ENUMERATOR_ERROR,
    WI_DIRECTORY_ENUMERATOR_PATH
};
typedef enum _wi_directory_enumerator_status    wi_directory_enumerator_status_t;

typedef struct _wi_directory_enumerator         wi_directory_enumerator_t;


WI_EXPORT wi_directory_enumerator_status_t      wi_directory_enumerator_get_next_path(wi_directory_enumerator_t *, wi_string_t **);
WI_EXPORT void                                  wi_directory_enumerator_skip_descendents(wi_directory_enumerator_t *);
WI_EXPORT wi_uinteger_t                         wi_directory_enumerator_level(wi_directory_enumerator_t *);

#endif /* WI_DIRECTORY_ENUMERATOR_H */