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
    
    regexp = wi_regexp_with_pattern(WI_STR("*"), 0);
    
    WI_TEST_ASSERT_NULL(regexp, "");
    
    regexp = wi_regexp_with_pattern(WI_STR("foobar"), WI_REGEXP_CASE_INSENSITIVE | WI_REGEXP_NEWLINE_SENSITIVE);
    
    WI_TEST_ASSERT_NOT_NULL(regexp, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_regexp_pattern(regexp), WI_STR("foobar"), "");
    WI_TEST_ASSERT_EQUALS(wi_regexp_options(regexp), WI_REGEXP_CASE_INSENSITIVE | WI_REGEXP_NEWLINE_SENSITIVE, "");
}



void wi_test_regexp_runtime_functions(void) {
    wi_regexp_t   *regexp1, *regexp2;
    
    regexp1 = wi_regexp_with_pattern(WI_STR("foobar"), 0);
    regexp2 = wi_autorelease(wi_copy(regexp1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(regexp1, regexp2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(regexp1), wi_hash(regexp2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(regexp1), wi_regexp_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(regexp2), wi_regexp_runtime_id(), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(regexp1), wi_regexp_pattern(regexp1), 0), WI_NOT_FOUND, "");
}



void wi_test_regexp_matching(void) {
    wi_regexp_t         *regexp;
    wi_string_t         *string;
    wi_regexp_match_t   matches[8];
    wi_range_t          range;
    wi_uinteger_t       i, count;
    
    regexp = wi_regexp_with_pattern(WI_STR("l"), 0);
    
    WI_TEST_ASSERT_EQUALS(wi_regexp_number_of_capture_groups(regexp), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_regexp_number_of_matches_in_string(regexp, WI_STR("foobar")), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_regexp_number_of_matches_in_string(regexp, WI_STR("hello world")), 3U, "");
    
    count = wi_regexp_get_matches_in_string(regexp, WI_STR("foobar"), matches, 8);
    
    WI_TEST_ASSERT_EQUALS(count, 0U, "");
    
    for(i = 0; i < 8; i++) {
        WI_TEST_ASSERT_EQUALS(matches[i].range.location, WI_NOT_FOUND, "");
        WI_TEST_ASSERT_EQUALS(matches[i].range.length, 0U, "");
    }
    
    count = wi_regexp_get_matches_in_string(regexp, WI_STR("hello world"), matches, 8);
    
    WI_TEST_ASSERT_EQUALS(count, 3U, "");
    WI_TEST_ASSERT_EQUALS(matches[0].range.location, 2U, "");
    WI_TEST_ASSERT_EQUALS(matches[0].range.length, 1U, "");
    WI_TEST_ASSERT_EQUALS(matches[1].range.location, 3U, "");
    WI_TEST_ASSERT_EQUALS(matches[1].range.length, 1U, "");
    WI_TEST_ASSERT_EQUALS(matches[2].range.location, 9U, "");
    WI_TEST_ASSERT_EQUALS(matches[2].range.length, 1U, "");
    
    for(i = 3; i < 8; i++) {
        WI_TEST_ASSERT_EQUALS(matches[i].range.location, WI_NOT_FOUND, "");
        WI_TEST_ASSERT_EQUALS(matches[i].range.length, 0U, "");
    }
    
    range = wi_regexp_range_of_first_match_in_string(regexp, WI_STR("foobar"));

    WI_TEST_ASSERT_EQUALS(range.location, WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUALS(range.length, 0U, "");
    
    range = wi_regexp_range_of_first_match_in_string(regexp, WI_STR("hello world"));
    
    WI_TEST_ASSERT_EQUALS(range.location, 2U, "");
    WI_TEST_ASSERT_EQUALS(range.length, 1U, "");
    
    string = wi_regexp_string_of_first_match_in_string(regexp, WI_STR("foobar"));
    
    WI_TEST_ASSERT_NULL(string, "");
    
    string = wi_regexp_string_of_first_match_in_string(regexp, WI_STR("hello world"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("l"), "");
}
