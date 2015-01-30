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

#ifndef WI_RECURSIVE_LOCK_H
#define WI_RECURSIVE_LOCK_H 1

#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

WI_EXPORT wi_runtime_id_t               wi_recursive_lock_runtime_id(void);

WI_EXPORT wi_recursive_lock_t *         wi_recursive_lock_alloc(void);
WI_EXPORT wi_recursive_lock_t *         wi_recursive_lock_init(wi_recursive_lock_t *);

WI_EXPORT void                          wi_recursive_lock_lock(wi_recursive_lock_t *);
WI_EXPORT wi_boolean_t                  wi_recursive_lock_try_lock(wi_recursive_lock_t *);
WI_EXPORT void                          wi_recursive_lock_unlock(wi_recursive_lock_t *);

#endif /* WI_RECURSIVE_LOCK_H */
