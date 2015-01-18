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
#include <string.h>
#include "test.h"

WI_TEST_EXPORT void                     wi_test_data_creation(void);
WI_TEST_EXPORT void                     wi_test_data_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_data_accessors(void);
WI_TEST_EXPORT void                     wi_test_data_appending(void);
WI_TEST_EXPORT void                     wi_test_data_digests(void);
WI_TEST_EXPORT void                     wi_test_data_serialization(void);
WI_TEST_EXPORT void                     wi_test_data_mutation(void);


void wi_test_data_creation(void) {
    wi_data_t   *data;
    char        *buffer;
    
    buffer = wi_malloc(1024);
    
    data = wi_data();
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");

    data = wi_data_with_bytes(buffer, 1024);
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
    
    data = wi_data_with_bytes_no_copy(buffer, 1024, false);
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
    
    data = wi_data_with_bytes_no_copy(buffer, 1024, true);
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
    
    data = wi_data_with_random_bytes(1024);
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
    
    data = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
    
    data = wi_data_with_contents_of_file(WI_STR("/non/existing/file.data"));
    
    WI_TEST_ASSERT_NULL(data, "");
    
    data = wi_data_with_contents_of_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-data-tests-1.data")));
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
    
    data = wi_mutable_data();
    
    WI_TEST_ASSERT_NOT_NULL(data, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(data), wi_data_runtime_id(), "");
}



void wi_test_data_runtime_functions(void) {
    wi_data_t           *data1;
    wi_mutable_data_t   *data2;
    
    data1 = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));
    data2 = wi_autorelease(wi_mutable_copy(data1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data1, data2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(data1), wi_hash(data2), "");
    
    wi_mutable_data_append_data(data2, data1);

    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(data1, data2, "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(data1), WI_STR("68656C6C"), 0), WI_NOT_FOUND, "");
}



void wi_test_data_accessors(void) {
    wi_data_t   *data;
    char        *buffer;
    
    data = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));
    
    WI_TEST_ASSERT_TRUE(memcmp(wi_data_bytes(data), wi_data_bytes(wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="))), wi_data_length(data)) == 0, "");
    
    buffer = wi_malloc(wi_data_length(data));
    
    wi_data_get_bytes(data, buffer, wi_data_length(data));

    WI_TEST_ASSERT_TRUE(memcmp(buffer, wi_data_bytes(data), wi_data_length(data)) == 0, "");
    
    wi_free(buffer);
}



void wi_test_data_appending(void) {
    wi_data_t   *data1, *data2, *data3;
    char        *buffer;
    
    data1 = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));
    data2 = wi_data_by_appending_data(data1, wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ=")));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(data1, data2, "");
    WI_TEST_ASSERT_TRUE(wi_data_length(data1) * 2 == wi_data_length(data2), "");
    WI_TEST_ASSERT_TRUE(memcmp(wi_data_bytes(data1), wi_data_bytes(data2), wi_data_length(data1)) == 0, "");
    WI_TEST_ASSERT_TRUE(memcmp(wi_data_bytes(data1), wi_data_bytes(data2) + wi_data_length(data1), wi_data_length(data1)) == 0, "");

    buffer = wi_malloc(wi_data_length(data1));

    wi_data_get_bytes(data1, buffer, wi_data_length(data1));
    
    data2 = wi_data_by_appending_bytes(data2, buffer, wi_data_length(data1));

    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(data1, data2, "");
    WI_TEST_ASSERT_TRUE(wi_data_length(data1) * 3 == wi_data_length(data2), "");
}



void wi_test_data_digests(void) {
    wi_data_t   *data;
    
    data = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));

#ifdef WI_DIGESTS
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_md5(data), WI_STR("5eb63bbbe01eeed093cb22bb8f5acdc3"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_sha1(data), WI_STR("2aae6c35c94fcfb415dbe95f408b9ce91ee846ed"), "");
#endif
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_base64(data), WI_STR("aGVsbG8gd29ybGQ="), "");
}



void wi_test_data_serialization(void) {
    wi_data_t       *data1, *data2;
    wi_string_t     *path;

    data1 = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));
    
    WI_TEST_ASSERT_FALSE(wi_data_write_to_file(data1, WI_STR("/non/existing/file.data")), "");
    
    path = wi_fs_temporary_path_with_template(WI_STR("/tmp/libwired-test.data.XXXXXXX"));
    
    WI_TEST_ASSERT_TRUE(wi_data_write_to_file(data1, path), "");
    
    data2 = wi_data_with_contents_of_file(path);

    WI_TEST_ASSERT_EQUAL_INSTANCES(data1, data2, "");
}



void wi_test_data_mutation(void) {
    wi_mutable_data_t   *data1;
    wi_data_t           *data2;
    wi_uinteger_t       i;
    
    data1 = wi_mutable_data();
    data2 = wi_data_with_base64(WI_STR("aGVsbG8gd29ybGQ="));
    
    for(i = 0; i < 100; i++) {
        wi_mutable_data_append_data(data1, data2);
        wi_mutable_data_append_bytes(data1, wi_data_bytes(data2), wi_data_length(data2));
        
        WI_TEST_ASSERT_TRUE(memcmp(wi_data_bytes(data1) + (i * wi_data_length(data2)), wi_data_bytes(data2), wi_data_length(data2)) == 0, "");
    }
}
