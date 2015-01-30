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

#include <wired/wired.h>

WI_TEST_EXPORT void                     wi_test_lock_creation(void);
WI_TEST_EXPORT void                     wi_test_lock_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_lock_locking(void);


void wi_test_lock_creation(void) {
#ifdef WI_PTHREADS
    wi_lock_t   *lock;
    
    lock = wi_autorelease(wi_lock_init(wi_lock_alloc()));
    
    WI_TEST_ASSERT_NOT_NULL(lock, "");
#endif
}



void wi_test_lock_runtime_functions(void) {
#ifdef WI_PTHREADS
    wi_lock_t   *lock;
    
    lock = wi_autorelease(wi_lock_init(wi_lock_alloc()));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(lock), wi_lock_runtime_id(), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(lock), WI_STR("wi_lock_t"), 0), WI_NOT_FOUND, "");
#endif
}



void wi_test_lock_locking(void) {
#ifdef WI_PTHREADS
    wi_lock_t       *lock;
    wi_boolean_t    result;
    
    lock = wi_autorelease(wi_lock_init(wi_lock_alloc()));
    
    wi_lock_lock(lock);
    wi_lock_unlock(lock);
    
    result = wi_lock_try_lock(lock);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    wi_lock_unlock(lock);
#endif
}
