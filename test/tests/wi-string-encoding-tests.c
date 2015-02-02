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

WI_TEST_EXPORT void                     wi_test_string_encoding_creation(void);
WI_TEST_EXPORT void                     wi_test_string_encoding_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_string_encoding_conversion(void);


void wi_test_string_encoding_creation(void) {
#ifdef WI_STRING_ENCODING
    wi_string_encoding_t    *encoding;
    
    encoding = wi_string_encoding_with_charset(WI_STR("ASCII"), 0);
    
    WI_TEST_ASSERT_NOT_NULL(encoding, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_encoding_charset(encoding), WI_STR("ASCII"), "");
    WI_TEST_ASSERT_EQUALS(wi_string_encoding_options(encoding), 0, "");
    
    encoding = wi_string_encoding_with_charset(WI_STR("ASCII"), WI_STRING_ENCODING_IGNORE);
    
    WI_TEST_ASSERT_NOT_NULL(encoding, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_encoding_charset(encoding), WI_STR("ASCII"), "");
    WI_TEST_ASSERT_EQUALS(wi_string_encoding_options(encoding), WI_STRING_ENCODING_IGNORE, "");
#endif
}



void wi_test_string_encoding_runtime_functions(void) {
#ifdef WI_STRING_ENCODING
    wi_string_encoding_t    *encoding;

    encoding = wi_string_encoding_with_charset(WI_STR("ASCII"), 0);

    WI_TEST_ASSERT_EQUALS(wi_runtime_id(encoding), wi_string_encoding_runtime_id(), "");

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(encoding), WI_STR("ASCII"), 0), WI_NOT_FOUND, "");
#endif
}



void wi_test_string_encoding_conversion(void) {
#ifdef WI_STRING_ENCODING
    wi_string_t             *string, *path;
    wi_data_t               *data;
    wi_string_encoding_t    *encoding;
    
    encoding = wi_string_encoding_with_charset(WI_STR("ISO-8859-1"), 0);
    string = wi_string_with_c_string("hello world", encoding);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world"), "");
    
    data = wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ="));
    string = wi_string_with_data(data, encoding);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world"), "");
    
    string = wi_string_with_bytes(wi_data_bytes(data), wi_data_length(data), encoding);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world"), "");
    
    path = wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-string-encoding-tests-1.txt"));
    string = wi_string_with_contents_of_file(path, encoding);
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_character_at_index(string, 0), 'h', "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 14, "");
    
    data = wi_string_data(WI_STR("hello world"), encoding);

    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")), "");

    encoding = wi_string_encoding_with_charset(WI_STR("ASCII"), 0);
    string = wi_string_with_contents_of_file(path, encoding);
    
    WI_TEST_ASSERT_NULL(string, "");
    
    encoding = wi_string_encoding_with_charset(WI_STR("hello world"), 0);
    string = wi_string_with_contents_of_file(path, encoding);
    
    WI_TEST_ASSERT_NULL(string, "");
#endif
}
