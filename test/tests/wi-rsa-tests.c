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

WI_TEST_EXPORT void                     wi_test_rsa_creation(void);
WI_TEST_EXPORT void                     wi_test_rsa_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_rsa_accessors(void);
WI_TEST_EXPORT void                     wi_test_rsa_encrypting_and_decrypting(void);


void wi_test_rsa_creation(void) {
#ifdef WI_RSA
    wi_rsa_t    *rsa;
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 0));
    
    WI_TEST_ASSERT_NULL(rsa, "");
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    
    WI_TEST_ASSERT_NOT_NULL(rsa, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(rsa), wi_rsa_runtime_id(), "");
    
    rsa = wi_autorelease(wi_rsa_init_with_pem_file(wi_rsa_alloc(), WI_STR("/non/existing/file.pem")));
    
    WI_TEST_ASSERT_NULL(rsa, "");
    
    rsa = wi_autorelease(wi_rsa_init_with_pem_file(wi_rsa_alloc(), wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-rsa-tests-1.pem"))));
    
    WI_TEST_ASSERT_NULL(rsa, "");
    
    rsa = wi_autorelease(wi_rsa_init_with_pem_file(wi_rsa_alloc(), wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-rsa-tests-2.pem"))));
    
    WI_TEST_ASSERT_NOT_NULL(rsa, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(rsa), wi_rsa_runtime_id(), "");
    
    rsa = wi_autorelease(wi_rsa_init_with_private_key(wi_rsa_alloc(), wi_data()));
    
    WI_TEST_ASSERT_NULL(rsa, "");
    
    rsa = wi_autorelease(wi_rsa_init_with_private_key(wi_rsa_alloc(), wi_data_with_base64(WI_STR("MIIBOwIBAAJBANlpi/JRzsGFCHyHARWkjg6qLnNjvgo84Shha4aOKQlQVON6LjVUTKuTGodkp7yZK0W4gfoNF/5CNbXb1Qo4xcUCAwEAAQJAafHFAJBc8HCjcgtXu/Q0RXEosZIpSVPhZIwUmb0swhw9LULNarL244HT2WJ/pSSUu3uIx+sT6mpNL+OtunQJAQIhAPSgtPWiWbHE7Bf3F4GS87PuVD2uYj9nbHuGAqfkrTaLAiEA44Tzb52/2dKz56sOW/ga/4ydsQeIQAxVBmr3uHK9zu8CIQDzQviQp5CQUeYBcurCJHMKA79r0wTKTju3niz37lQ9PwIhANdjtv5UzhpNgalxY++nSw/gtCyy38capaekvo2seoqbAiBYCzlmjq02JpohH29ijG52ecfb88uS9eUufUVoOfTC/A=="))));
    
    WI_TEST_ASSERT_NOT_NULL(rsa, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(rsa), wi_rsa_runtime_id(), "");
    
    rsa = wi_autorelease(wi_rsa_init_with_public_key(wi_rsa_alloc(), wi_data()));
    
    WI_TEST_ASSERT_NULL(rsa, "");
    
    rsa = wi_autorelease(wi_rsa_init_with_public_key(wi_rsa_alloc(), wi_data_with_base64(WI_STR("MEgCQQDZaYvyUc7BhQh8hwEVpI4Oqi5zY74KPOEoYWuGjikJUFTjei41VEyrkxqHZKe8mStFuIH6DRf+QjW129UKOMXFAgMBAAE="))));
    
    WI_TEST_ASSERT_NOT_NULL(rsa, "%m");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(rsa), wi_rsa_runtime_id(), "");
#endif
}



void wi_test_rsa_runtime_functions(void) {
#ifdef WI_RSA
    wi_rsa_t    *rsa1, *rsa2;
    
    rsa1 = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    rsa2 = wi_autorelease(wi_copy(rsa1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_rsa_private_key(rsa1), wi_rsa_private_key(rsa2), "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(rsa1), WI_STR("512"), 0), WI_NOT_FOUND, "");
#endif
}



void wi_test_rsa_accessors(void) {
#ifdef WI_RSA
    wi_rsa_t    *rsa;
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    
    WI_TEST_ASSERT_NOT_NULL(wi_rsa_rsa(rsa), "");
    WI_TEST_ASSERT_TRUE(wi_data_length(wi_rsa_public_key(rsa)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_data_length(wi_rsa_private_key(rsa)) > 0, "");
    WI_TEST_ASSERT_EQUALS(wi_rsa_bits(rsa), 512U, "");
#endif
}



void wi_test_rsa_encrypting_and_decrypting(void) {
#ifdef WI_RSA
    wi_rsa_t    *rsa1, *rsa2;
    
    rsa1 = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    rsa2 = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_rsa_decrypt(rsa1, wi_rsa_encrypt(rsa1, wi_string_data(WI_STR("hello world")))),
                                   wi_string_data(WI_STR("hello world")), "");
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(wi_rsa_decrypt(rsa1, wi_rsa_encrypt(rsa2, wi_string_data(WI_STR("hello world")))),
                                       wi_string_data(WI_STR("hello world")), "");
#endif
}
