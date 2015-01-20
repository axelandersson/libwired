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

WI_TEST_EXPORT void                     wi_test_url_creation(void);
WI_TEST_EXPORT void                     wi_test_url_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_url_mutation(void);


void wi_test_url_creation(void) {
    wi_url_t    *url;
    
    url = wi_url_with_string(WI_STR("wired://"));
    
    WI_TEST_ASSERT_NOT_NULL(url, "");
    WI_TEST_ASSERT_FALSE(wi_url_is_valid(url), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url), WI_STR("wired:///"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_scheme(url), WI_STR("wired"), "");
    WI_TEST_ASSERT_NULL(wi_url_host(url), "");
    WI_TEST_ASSERT_EQUALS(wi_url_port(url), 0U, "");
    WI_TEST_ASSERT_NULL(wi_url_path(url), "");
    WI_TEST_ASSERT_NULL(wi_url_user(url), "");
    WI_TEST_ASSERT_NULL(wi_url_password(url), "");
    
    url = wi_url_with_string(WI_STR("wired://user@localhost/"));
    
    WI_TEST_ASSERT_NOT_NULL(url, "");
    WI_TEST_ASSERT_TRUE(wi_url_is_valid(url), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url), WI_STR("wired://user@localhost/"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_scheme(url), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_host(url), WI_STR("localhost"), "");
    WI_TEST_ASSERT_EQUALS(wi_url_port(url), 0U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_path(url), WI_STR("/"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_user(url), WI_STR("user"), "");
    WI_TEST_ASSERT_NULL(wi_url_password(url), "");
    
    url = wi_url_with_string(WI_STR("wired://[2001:db8:1f70::999:de8:7648:6e8]:2000/"));
    
    WI_TEST_ASSERT_NOT_NULL(url, "");
    WI_TEST_ASSERT_TRUE(wi_url_is_valid(url), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url), WI_STR("wired://[2001:db8:1f70::999:de8:7648:6e8]:2000/"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_scheme(url), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_host(url), WI_STR("2001:db8:1f70::999:de8:7648:6e8"), "");
    WI_TEST_ASSERT_EQUALS(wi_url_port(url), 2000U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_path(url), WI_STR("/"), "");
    WI_TEST_ASSERT_NULL(wi_url_user(url), "");
    WI_TEST_ASSERT_NULL(wi_url_password(url), "");
    
    url = wi_url_with_string(WI_STR("wired://user:pass@localhost:2000/file.txt"));
    
    WI_TEST_ASSERT_NOT_NULL(url, "");
    WI_TEST_ASSERT_TRUE(wi_url_is_valid(url), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url), WI_STR("wired://user:pass@localhost:2000/file.txt"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_scheme(url), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_host(url), WI_STR("localhost"), "");
    WI_TEST_ASSERT_EQUALS(wi_url_port(url), 2000U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_path(url), WI_STR("/file.txt"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_user(url), WI_STR("user"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_password(url), WI_STR("pass"), "");
    
    url = wi_url_init(wi_mutable_url_alloc());
    
    WI_TEST_ASSERT_NOT_NULL(url, "");
    WI_TEST_ASSERT_FALSE(wi_url_is_valid(url), "");
    
//    wi_url_t            *url1;
//    wi_mutable_url_t    *url2;
//    
//    url1 = wi_url_init_with_string(wi_url_alloc(), WI_STR("wired://user:pass@localhost:2000/file.txt"));
//    
//    WI_TEST_ASSERT_TRUE(wi_url_is_valid(url1), "");
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url1), WI_STR("wired://user:pass@localhost:2000/file.txt"), "");
//    
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_scheme(url1), WI_STR("wired"), "");
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_host(url1), WI_STR("localhost"), "");
//    WI_TEST_ASSERT_EQUALS(wi_url_port(url1), 2000U, "");
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_path(url1), WI_STR("/file.txt"), "");
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_user(url1), WI_STR("user"), "");
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_password(url1), WI_STR("pass"), "");
//    
//    url2 = wi_mutable_copy(url1);
//    
//    wi_mutable_url_set_scheme(url2, WI_STR("wired2"));
//    wi_mutable_url_set_host(url2, WI_STR("localhost2"));
//    wi_mutable_url_set_port(url2, 2001);
//    wi_mutable_url_set_path(url2, WI_STR("/anotherfile.txt"));
//    wi_mutable_url_set_user(url2, WI_STR("user2"));
//    wi_mutable_url_set_password(url2, WI_STR("pass2"));
//
//    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url2), WI_STR("wired2://user2:pass2@localhost2:2001/anotherfile.txt"), "");
//    
//    wi_release(url1);
//    wi_release(url2);
}



void wi_test_url_runtime_functions(void) {
    wi_url_t            *url1;
    wi_mutable_url_t    *url2;

    url1 = wi_url_with_string(WI_STR("wired://user:pass@localhost:2000/file.txt"));
    url2 = wi_autorelease(wi_mutable_copy(url1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(url1, url2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(url1), wi_hash(url2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(url1), wi_url_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(url2), wi_url_runtime_id(), "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(url1) & WI_RUNTIME_OPTION_IMMUTABLE, "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(url2) & WI_RUNTIME_OPTION_MUTABLE, "");
    
    wi_mutable_url_set_scheme(url2, WI_STR("http"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(url1, url2, "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(url1), WI_STR("localhost"), 0), WI_NOT_FOUND, "");
}



void wi_test_url_mutation(void) {
    wi_mutable_url_t    *url;
    
    url = wi_url_init(wi_mutable_url_alloc());

    wi_mutable_url_set_scheme(url, WI_STR("wired"));
    wi_mutable_url_set_host(url, WI_STR("localhost"));
    wi_mutable_url_set_port(url, 2000);
    wi_mutable_url_set_path(url, WI_STR("/file.txt"));
    wi_mutable_url_set_user(url, WI_STR("user"));
    wi_mutable_url_set_password(url, WI_STR("pass"));

    WI_TEST_ASSERT_TRUE(wi_url_is_valid(url), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_url_string(url), WI_STR("wired://user:pass@localhost:2000/file.txt"), "");
}
