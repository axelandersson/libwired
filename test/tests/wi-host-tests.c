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

#include "config.h"
#include <wired/wired.h>

WI_TEST_EXPORT void                     wi_test_host_creation(void);
WI_TEST_EXPORT void                     wi_test_host_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_host_addresses(void);


void wi_test_host_creation(void) {
    wi_host_t   *host;
    
    host = wi_host();
    
    WI_TEST_ASSERT_NOT_NULL(host, "");
    WI_TEST_ASSERT_NOT_NULL(wi_host_address(host), "");
    
    host = wi_host_with_string(WI_STR("localhost"));
    
    WI_TEST_ASSERT_NOT_NULL(host, "");
    WI_TEST_ASSERT_NOT_NULL(wi_host_address(host), "");
}



void wi_test_host_runtime_functions(void) {
    wi_host_t   *host1, *host2, *host3, *host4, *host5, *host6;
    
    host1 = wi_host();
    host2 = wi_autorelease(wi_copy(host1));
    host3 = wi_host_with_string(WI_STR("localhost"));
    host4 = wi_autorelease(wi_copy(host3));
    host5 = wi_host_with_string(WI_STR("aab119a592b9e23779b66649677b436d24b35aaa5ad5beadf4c2a945b70577e5.com"));
    host6 = wi_autorelease(wi_copy(host5));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(host1, host2, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(host3, host4, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(host5, host6, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host1, host3, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host2, host3, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host1, host4, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host2, host4, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host1, host5, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host2, host5, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host1, host6, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(host2, host6, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(host1), wi_hash(host2), "");
    WI_TEST_ASSERT_EQUALS(wi_hash(host3), wi_hash(host4), "");
    WI_TEST_ASSERT_EQUALS(wi_hash(host5), wi_hash(host6), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(host1), wi_host_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(host2), wi_host_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(host3), wi_host_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(host4), wi_host_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(host5), wi_host_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(host6), wi_host_runtime_id(), "");
    
#ifdef HAVE_GETIFADDRS
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(host1), WI_STR("127.0.0.1"), 0), WI_NOT_FOUND, "");
#else
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(host1), WI_STR("0.0.0.0"), 0), WI_NOT_FOUND, "");
#endif

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(host3), WI_STR("127.0.0.1"), 0), WI_NOT_FOUND, "");
}



void wi_test_host_addresses(void) {
    wi_host_t   *host;
    wi_array_t  *addresses;
    
    host = wi_host();
    addresses = wi_host_addresses(host);
    
    WI_TEST_ASSERT_TRUE(wi_array_count(addresses) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(addresses, wi_host_address(host)), "");
    
#ifndef HAVE_GETIFADDRS
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(addresses, wi_address_wildcard_for_family(WI_ADDRESS_IPV4)), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(addresses, wi_address_wildcard_for_family(WI_ADDRESS_IPV6)), "");
#endif
    
    host = wi_host_with_string(WI_STR("aab119a592b9e23779b66649677b436d24b35aaa5ad5beadf4c2a945b70577e5.com"));

    WI_TEST_ASSERT_NULL(wi_host_addresses(host), "");
    WI_TEST_ASSERT_NULL(wi_host_address(host), "");
}
