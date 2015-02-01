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
#include "test.h"

WI_TEST_EXPORT void                     wi_test_x509_creation(void);
WI_TEST_EXPORT void                     wi_test_x509_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_x509_accessors(void);


void wi_test_x509_creation(void) {
#ifdef WI_X509
    wi_x509_t   *x509;
    wi_rsa_t    *rsa;
    
    x509 = wi_autorelease(wi_x509_init_with_pem_file(wi_x509_alloc(), WI_STR("/non/existing/file.pem")));
    
    WI_TEST_ASSERT_NULL(x509, "");

    x509 = wi_autorelease(wi_x509_init_with_pem_file(wi_x509_alloc(), wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-x509-tests-1.pem"))));
    
    WI_TEST_ASSERT_NULL(x509, "");
    
    x509 = wi_autorelease(wi_x509_init_with_pem_file(wi_x509_alloc(), wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-x509-tests-2.pem"))));
    
    WI_TEST_ASSERT_NOT_NULL(x509, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(x509), wi_x509_runtime_id(), "");
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    
    x509 = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), rsa, WI_STR("")));
    
    WI_TEST_ASSERT_NULL(x509, "");
    
    x509 = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), rsa, WI_STR("foobar")));
    
    WI_TEST_ASSERT_NOT_NULL(x509, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(x509), wi_x509_runtime_id(), "");
#endif
}



void wi_test_x509_runtime_functions(void) {
#ifdef WI_X509
    wi_x509_t   *x5091, *x5092;
    wi_rsa_t    *rsa;
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    
    x5091 = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), rsa, WI_STR("foobar")));
    x5092 = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), rsa, WI_STR("foobar")));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_x509_common_name(x5091), wi_x509_common_name(x5092), "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(x5091), WI_STR("foobar"), 0), WI_NOT_FOUND, "");
#endif
}



void wi_test_x509_accessors(void) {
#ifdef WI_X509
    wi_x509_t   *x509;
    wi_rsa_t    *rsa;
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    
    x509 = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), rsa, WI_STR("foobar")));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_x509_common_name(x509), WI_STR("foobar"), "");
#endif
}
