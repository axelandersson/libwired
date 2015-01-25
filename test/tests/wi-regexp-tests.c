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

WI_TEST_EXPORT void                     wi_test_regexp_creation(void);
WI_TEST_EXPORT void                     wi_test_regexp_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_regexp_matching(void);


void wi_test_regexp_creation(void) {
    wi_regexp_t     *regexp;
    
    regexp = wi_regexp_with_string(WI_STR("foobar"));
    
    WI_TEST_ASSERT_NULL(regexp, "");
    
    regexp = wi_regexp_with_string(WI_STR("/foobar"));
    
    WI_TEST_ASSERT_NULL(regexp, "");
    
    regexp = wi_regexp_with_string(WI_STR("/foobar/"));
    
    WI_TEST_ASSERT_NOT_NULL(regexp, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_regexp_string(regexp), WI_STR("/foobar/"), "");
    
    regexp = wi_regexp_with_string(WI_STR("/foobar/im"));
    
    WI_TEST_ASSERT_NOT_NULL(regexp, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_regexp_string(regexp), WI_STR("/foobar/im"), "");
    
    regexp = wi_regexp_with_string(WI_STR("/foobar/x"));
    
    WI_TEST_ASSERT_NULL(regexp, "");
    
    regexp = wi_regexp_with_string(WI_STR("/*/"));
    
    WI_TEST_ASSERT_NULL(regexp, "");
}



void wi_test_regexp_runtime_functions(void) {
    wi_regexp_t   *regexp1, *regexp2;
    
    regexp1 = wi_regexp_with_string(WI_STR("/foobar/"));
    regexp2 = wi_autorelease(wi_copy(regexp1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(regexp1, regexp2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(regexp1), wi_hash(regexp2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(regexp1), wi_regexp_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(regexp2), wi_regexp_runtime_id(), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(regexp1), wi_regexp_string(regexp1), 0), WI_NOT_FOUND, "");
}



void wi_test_regexp_matching(void) {
    wi_regexp_t     *regexp;
    wi_string_t     *string;
    wi_range_t      range;
    wi_boolean_t    result;
    
    regexp = wi_regexp_with_string(WI_STR("/(w).*/"));
    
    WI_TEST_ASSERT_FALSE(wi_regexp_matches_string(regexp, WI_STR("foobar")), "");
    WI_TEST_ASSERT_TRUE(wi_regexp_matches_string(regexp, WI_STR("hello world")), "");
    
    result = wi_regexp_get_range_by_matching_string(regexp, WI_STR("foobar"), 0, &range);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_EQUALS(range.location, WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUALS(range.length, 0U, "");
    
    result = wi_regexp_get_range_by_matching_string(regexp, WI_STR("hello world"), 0, &range);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_EQUALS(range.location, 6U, "");
    WI_TEST_ASSERT_EQUALS(range.length, 5U, "");
    
    string = wi_regexp_string_by_matching_string(regexp, WI_STR("foobar"), 0);
    
    WI_TEST_ASSERT_NULL(string, "");
    
    string = wi_regexp_string_by_matching_string(regexp, WI_STR("hello world"), 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("world"), "");
    
    string = wi_regexp_string_by_matching_string(regexp, WI_STR("hello world"), 1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("w"), "");
    
    string = wi_regexp_string_by_matching_string(regexp, WI_STR("hello world"), 2);
    
    WI_TEST_ASSERT_NULL(string, "");
}
