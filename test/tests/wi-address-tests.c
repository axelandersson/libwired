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
#include <netinet/in.h>
#include <string.h>
#include "config.h"

WI_TEST_EXPORT void                     wi_test_address_creation(void);
WI_TEST_EXPORT void                     wi_test_address_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_address_comparison(void);
WI_TEST_EXPORT void                     wi_test_address_accessors(void);
WI_TEST_EXPORT void                     wi_test_address_matching(void);
WI_TEST_EXPORT void                     wi_test_address_mutation(void);


void wi_test_address_creation(void) {
    wi_address_t            *address;
    struct sockaddr_in6     sa;
    
    memset(&sa, 0, sizeof(sa));
    
    address = wi_address_with_sa((struct sockaddr *) &sa);
    
    WI_TEST_ASSERT_NULL(address, "");
    
    sa.sin6_family  = AF_INET6;
    sa.sin6_addr    = in6addr_any;
#ifdef HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN
    sa.sin6_len     = sizeof(sa);
#endif
    
    address = wi_address_with_sa((struct sockaddr *) &sa);
    
    WI_TEST_ASSERT_NOT_NULL(address, "");
    WI_TEST_ASSERT_EQUALS(wi_address_family(address), WI_ADDRESS_IPV6, "");
    
    address = wi_address_with_wildcard_for_family(WI_ADDRESS_IPV4);
    
    WI_TEST_ASSERT_NOT_NULL(address, "");
    WI_TEST_ASSERT_EQUALS(wi_address_family(address), WI_ADDRESS_IPV4, "");
    
    address = wi_address_with_wildcard_for_family(WI_ADDRESS_IPV6);
    
    WI_TEST_ASSERT_NOT_NULL(address, "");
    WI_TEST_ASSERT_EQUALS(wi_address_family(address), WI_ADDRESS_IPV6, "");
    
    address = wi_address_with_string(WI_STR("127.0.0.1"));
    
    WI_TEST_ASSERT_NOT_NULL(address, "");
    WI_TEST_ASSERT_EQUALS(wi_address_family(address), WI_ADDRESS_IPV4, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_address_string(address), WI_STR("127.0.0.1"), "");
    
    address = wi_address_with_string(WI_STR("::1"));
    
    WI_TEST_ASSERT_NOT_NULL(address, "");
    WI_TEST_ASSERT_EQUALS(wi_address_family(address), WI_ADDRESS_IPV6, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_address_string(address), WI_STR("::1"), "");
    
    address = wi_address_with_string(WI_STR("foobar"));
    
    WI_TEST_ASSERT_NULL(address, "");
}



void wi_test_address_runtime_functions(void) {
    wi_address_t           *address1;
    wi_mutable_address_t   *address2, *address3;
    
    address1 = wi_address_with_wildcard_for_family(WI_ADDRESS_IPV4);
    address2 = wi_autorelease(wi_mutable_copy(address1));
    address3 = wi_autorelease(wi_address_init_with_wildcard_for_family(wi_mutable_address_alloc(), WI_ADDRESS_IPV6));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(address1, address2, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(address1, address3, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(address2, address3, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(address1), wi_hash(address2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(address1), wi_address_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(address2), wi_address_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(address3), wi_address_runtime_id(), "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(address1) & WI_RUNTIME_OPTION_IMMUTABLE, "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(address2) & WI_RUNTIME_OPTION_MUTABLE, "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(address3) & WI_RUNTIME_OPTION_MUTABLE, "");
    
    wi_mutable_address_set_port(address2, 4871);
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(address1, address2, "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(address1), WI_STR("ipv4"), 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(address3), WI_STR("ipv6"), 0), WI_NOT_FOUND, "");
}



void wi_test_address_comparison(void) {
    wi_address_t    *address1, *address2;
    
    address1 = wi_address_with_wildcard_for_family(WI_ADDRESS_IPV4);
    address2 = wi_address_with_wildcard_for_family(WI_ADDRESS_IPV6);
    
    WI_TEST_ASSERT_EQUALS(wi_address_compare_family(address1, address1), 0, "");
    WI_TEST_ASSERT_EQUALS(wi_address_compare_family(address1, address2), 1, "");
    WI_TEST_ASSERT_EQUALS(wi_address_compare_family(address2, address1), -1, "");
}



void wi_test_address_accessors(void) {
    wi_address_t    *address;
    
    address = wi_address_with_wildcard_for_family(WI_ADDRESS_IPV4);

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_address_string(address), WI_STR("0.0.0.0"), "");
    WI_TEST_ASSERT_NULL(wi_address_hostname(address), "");
}



void wi_test_address_matching(void) {
    WI_TEST_ASSERT_TRUE(wi_address_matches_pattern(wi_address_with_string(WI_STR("127.0.0.1")), WI_STR("127.0.0.1")), "");
    WI_TEST_ASSERT_TRUE(wi_address_matches_pattern(wi_address_with_string(WI_STR("127.0.0.1")), WI_STR("127.0.0.*")), "");
    WI_TEST_ASSERT_FALSE(wi_address_matches_pattern(wi_address_with_string(WI_STR("255.0.0.1")), WI_STR("127.0.0.*")), "");
    WI_TEST_ASSERT_TRUE(wi_address_matches_pattern(wi_address_with_string(WI_STR("127.0.0.1")), WI_STR("127.0.0.0/24")), "");
    WI_TEST_ASSERT_FALSE(wi_address_matches_pattern(wi_address_with_string(WI_STR("255.0.0.1")), WI_STR("127.0.0.0/24")), "");
    WI_TEST_ASSERT_TRUE(wi_address_matches_pattern(wi_address_with_string(WI_STR("::1")), WI_STR("0000:0000:0000:0000:0000:0000:0000:0001")), "");
}



void wi_test_address_mutation(void) {
    wi_mutable_address_t    *address1, *address2;
    wi_boolean_t            result1, result2;
    struct sockaddr_in      sa1;
    struct sockaddr_in6     sa2;
    
    address1 = wi_autorelease(wi_address_init_with_wildcard_for_family(wi_mutable_address_alloc(), WI_ADDRESS_IPV4));
    address2 = wi_autorelease(wi_address_init_with_wildcard_for_family(wi_mutable_address_alloc(), WI_ADDRESS_IPV6));
    
    WI_TEST_ASSERT_EQUALS(wi_address_port(address1), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_address_port(address2), 0U, "");
    
    wi_mutable_address_set_port(address1, 4871);
    wi_mutable_address_set_port(address2, 4871);

    WI_TEST_ASSERT_EQUALS(wi_address_port(address1), 4871U, "");
    WI_TEST_ASSERT_EQUALS(wi_address_port(address2), 4871U, "");
    
    memset(&sa1, 0, sizeof(sa1));
    memset(&sa2, 0, sizeof(sa2));
    
    result1 = wi_mutable_address_set_sa(address1, (struct sockaddr *) &sa1);
    result2 = wi_mutable_address_set_sa(address2, (struct sockaddr *) &sa2);

    WI_TEST_ASSERT_FALSE(result1, "");
    WI_TEST_ASSERT_FALSE(result2, "");

    sa1.sin_family          = AF_INET;
    sa1.sin_addr.s_addr     = INADDR_ANY;
#ifdef HAVE_STRUCT_SOCKADDR_IN_SIN_LEN
    sa1.sin_len             = sizeof(sa1);
#endif

    sa2.sin6_family         = AF_INET6;
    sa2.sin6_addr           = in6addr_any;
#ifdef HAVE_STRUCT_SOCKADDR_IN6_SIN6_LEN
    sa2.sin6_len            = sizeof(sa2);
#endif

    result1 = wi_mutable_address_set_sa(address1, (struct sockaddr *) &sa1);
    result2 = wi_mutable_address_set_sa(address2, (struct sockaddr *) &sa2);
    
    WI_TEST_ASSERT_TRUE(result1, "");
    WI_TEST_ASSERT_TRUE(result2, "");
    WI_TEST_ASSERT_EQUALS(wi_address_port(address1), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_address_port(address2), 0U, "");
}
