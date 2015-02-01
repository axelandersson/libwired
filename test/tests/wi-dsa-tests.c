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

WI_TEST_EXPORT void                     wi_test_dsa_creation(void);
WI_TEST_EXPORT void                     wi_test_dsa_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_dsa_accessors(void);


void wi_test_dsa_creation(void) {
#ifdef WI_DSA
    wi_dsa_t    *dsa;
    
    dsa = wi_autorelease(wi_dsa_init_with_bits(wi_dsa_alloc(), 512));
    
    WI_TEST_ASSERT_NOT_NULL(dsa, "");
    WI_TEST_ASSERT_EQUALS(wi_dsa_bits(dsa), 384U, "");
    
    dsa = wi_autorelease(wi_dsa_init_with_pem_file(wi_dsa_alloc(), WI_STR("/non/existing/file.pem")));
    
    WI_TEST_ASSERT_NULL(dsa, "");
    
    dsa = wi_autorelease(wi_dsa_init_with_pem_file(wi_dsa_alloc(), wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-dsa-tests-1.pem"))));
    
    WI_TEST_ASSERT_NULL(dsa, "");
    
    dsa = wi_autorelease(wi_dsa_init_with_pem_file(wi_dsa_alloc(), wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-dsa-tests-2.pem"))));
    
    WI_TEST_ASSERT_NOT_NULL(dsa, "");
    WI_TEST_ASSERT_EQUALS(wi_dsa_bits(dsa), 384U, "");
    
    dsa = wi_autorelease(wi_dsa_init_with_private_key(wi_dsa_alloc(), wi_data()));
    
    WI_TEST_ASSERT_NULL(dsa, "");
    
    dsa = wi_autorelease(wi_dsa_init_with_private_key(wi_dsa_alloc(), wi_data_with_base64_string(WI_STR("MIH3AgEAAkEA0c36Uaj1S/cKjYAokEUtWQHmFHLqd8mbnY9J9wyNS0mlXvklnko2NlflL1qg9p58uRjkGBtCinI0mxeEIxwckwIVAPpF85s7Iv1o7z4UUAs9KRamjULXAkBFchKBXr+D+lGgnBFtb//EBThgSD9t3WBxyKGvNLBDWGJu2+av/fR9uBxBm1cUUldzGvqBjr/lcBmTIUe4rmAcAkBBN2fEFvs6uuIX+/8EqM1T2yBmwt3KHMVuCD+opZga29WJPNZgNXIj8yNFGFIxJeTndvhcndNrlcztYx3e/GSCAhRDUGb+CUPkNuBbsSHbdYtEQ+/2Gg=="))));
    
    WI_TEST_ASSERT_NOT_NULL(dsa, "");
    WI_TEST_ASSERT_EQUALS(wi_dsa_bits(dsa), 384U, "");
    
    dsa = wi_autorelease(wi_dsa_init_with_public_key(wi_dsa_alloc(), wi_data()));
    
    WI_TEST_ASSERT_NULL(dsa, "");
#endif
}



void wi_test_dsa_runtime_functions(void) {
#ifdef WI_DSA
    wi_dsa_t    *dsa1, *dsa2;
    
    dsa1 = wi_autorelease(wi_dsa_init_with_bits(wi_dsa_alloc(), 512));
    dsa2 = wi_autorelease(wi_copy(dsa1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dsa_private_key(dsa1), wi_dsa_private_key(dsa2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dsa1), wi_dsa_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dsa2), wi_dsa_runtime_id(), "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(dsa1), WI_STR("384"), 0), WI_NOT_FOUND, "");
#endif
}



void wi_test_dsa_accessors(void) {
#ifdef WI_DSA
    wi_dsa_t    *dsa;
    
    dsa = wi_autorelease(wi_dsa_init_with_bits(wi_dsa_alloc(), 512));
    
    WI_TEST_ASSERT_TRUE(wi_data_length(wi_dsa_public_key(dsa)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_data_length(wi_dsa_private_key(dsa)) > 0, "");
    WI_TEST_ASSERT_EQUALS(wi_dsa_bits(dsa), 384U, "");
#endif
}
