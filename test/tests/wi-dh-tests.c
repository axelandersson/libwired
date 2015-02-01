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

WI_TEST_EXPORT void                     wi_test_dh_creation(void);
WI_TEST_EXPORT void                     wi_test_dh_runtime_functions(void);


void wi_test_dh_creation(void) {
#ifdef WI_DH
    wi_dh_t     *dh;
    wi_data_t   *p1, *g1, *p2, *g2;
    
    dh = wi_autorelease(wi_dh_init_with_bits(wi_dh_alloc(), 64));
    
    WI_TEST_ASSERT_NOT_NULL(dh, "");
    
    wi_dh_get_data(dh, &p1, &g1);
    
    WI_TEST_ASSERT_EQUALS(wi_data_length(p1), 8, "");
    WI_TEST_ASSERT_EQUALS(wi_data_length(g1), 1, "");
    
    dh = wi_autorelease(wi_dh_init_with_data(wi_dh_alloc(), p1, g1));
    
    WI_TEST_ASSERT_NOT_NULL(dh, "");
    
    wi_dh_get_data(dh, &p2, &g2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(p1, p2, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(g1, g2, "");
#endif
}



void wi_test_dh_runtime_functions(void) {
#ifdef WI_DH
    wi_dh_t     *dh1, *dh2;
    wi_data_t   *p, *g;
    
    dh1 = wi_autorelease(wi_dh_init_with_bits(wi_dh_alloc(), 64));
    dh2 = wi_autorelease(wi_copy(dh1));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dh1), wi_dh_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dh2), wi_dh_runtime_id(), "");
    
    wi_dh_get_data(dh1, &p, &g);
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(dh1), wi_description(p), 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(dh2), wi_description(p), 0), WI_NOT_FOUND, "");
#endif
}
