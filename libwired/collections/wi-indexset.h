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

#ifndef WI_INDEXSET_H
#define WI_INDEXSET_H 1

#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

typedef struct _wi_indexset                 wi_indexset_t;
typedef struct _wi_indexset                 wi_mutable_indexset_t;

WI_EXPORT wi_runtime_id_t                   wi_indexset_runtime_id(void);

WI_EXPORT wi_indexset_t *                   wi_indexset(void);
WI_EXPORT wi_indexset_t *                   wi_indexset_with_index(wi_uinteger_t);
WI_EXPORT wi_indexset_t *                   wi_indexset_with_indexes_in_range(wi_range_t);
WI_EXPORT wi_mutable_indexset_t *           wi_mutable_indexset(void);

WI_EXPORT wi_indexset_t *                   wi_indexset_alloc(void);
WI_EXPORT wi_mutable_indexset_t *           wi_mutable_indexset_alloc(void);
WI_EXPORT wi_indexset_t *                   wi_indexset_init(wi_indexset_t *);
WI_EXPORT wi_indexset_t *                   wi_indexset_init_with_index(wi_indexset_t *, wi_uinteger_t);
WI_EXPORT wi_indexset_t *                   wi_indexset_init_with_indexes_in_range(wi_indexset_t *, wi_range_t);

WI_EXPORT wi_uinteger_t                     wi_indexset_count(wi_indexset_t *);

WI_EXPORT wi_boolean_t                      wi_indexset_contains_index(wi_indexset_t *, wi_uinteger_t);
WI_EXPORT wi_boolean_t                      wi_indexset_contains_indexes(wi_indexset_t *, wi_indexset_t *);
WI_EXPORT wi_boolean_t                      wi_indexset_contains_indexes_in_range(wi_indexset_t *, wi_range_t);

WI_EXPORT wi_uinteger_t                     wi_indexset_first_index(wi_indexset_t *);
WI_EXPORT wi_uinteger_t                     wi_indexset_last_index(wi_indexset_t *);

WI_EXPORT wi_enumerator_t *                 wi_indexset_index_enumerator(wi_indexset_t *);

WI_EXPORT void                              wi_mutable_indexset_add_index(wi_mutable_indexset_t *, wi_uinteger_t);
WI_EXPORT void                              wi_mutable_indexset_add_indexes(wi_mutable_indexset_t *, wi_indexset_t *);
WI_EXPORT void                              wi_mutable_indexset_add_indexes_in_range(wi_mutable_indexset_t *, wi_range_t);
WI_EXPORT void                              wi_mutable_indexset_set_indexes(wi_mutable_indexset_t *, wi_indexset_t *);

WI_EXPORT void                              wi_mutable_indexset_remove_index(wi_mutable_indexset_t *, wi_uinteger_t);
WI_EXPORT void                              wi_mutable_indexset_remove_indexes(wi_mutable_indexset_t *, wi_indexset_t *);
WI_EXPORT void                              wi_mutable_indexset_remove_indexes_in_range(wi_mutable_indexset_t *, wi_range_t);
WI_EXPORT void                              wi_mutable_indexset_remove_all_indexes(wi_mutable_indexset_t *);

#endif /* WI_INDEXSET_H */
