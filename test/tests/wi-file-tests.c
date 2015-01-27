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

WI_TEST_EXPORT void                     wi_test_file_creation(void);
WI_TEST_EXPORT void                     wi_test_file_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_file_reading(void);
WI_TEST_EXPORT void                     wi_test_file_writing(void);
WI_TEST_EXPORT void                     wi_test_file_updating(void);
WI_TEST_EXPORT void                     wi_test_file_truncating(void);


void wi_test_file_creation(void) {
    wi_file_t       *file;
    wi_string_t     *path;
    
    file = wi_file_for_reading(WI_STR("/non/existing/file.txt"));
    
    WI_TEST_ASSERT_NULL(file, "");
    
    path = wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-file-tests-1.txt"));
    file = wi_file_for_reading(path);
    
    WI_TEST_ASSERT_NOT_NULL(file, "");
    WI_TEST_ASSERT_TRUE(wi_file_descriptor(file) > 0, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_file_path(file), path, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_file_read_to_end_of_file(file), wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK")), "");
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-file.XXXXXXX"));
    file = wi_file_for_writing(path);
    
    WI_TEST_ASSERT_NOT_NULL(file, "");
    WI_TEST_ASSERT_TRUE(wi_file_descriptor(file) > 0, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_file_path(file), path, "");
    WI_TEST_ASSERT_EQUALS(wi_file_write(file, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK"))), 12, "");
    
    wi_filesystem_delete_path(path);
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-file.XXXXXXX"));
    file = wi_file_for_updating(path);
    
    WI_TEST_ASSERT_NOT_NULL(file, "");
    WI_TEST_ASSERT_TRUE(wi_file_descriptor(file) > 0, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_file_path(file), path, "");
    WI_TEST_ASSERT_EQUALS(wi_file_write(file, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK"))), 12, "");
    
    wi_filesystem_delete_path(path);
    
    file = wi_file_temporary_file();
    
    WI_TEST_ASSERT_NOT_NULL(file, "");
    WI_TEST_ASSERT_TRUE(wi_file_descriptor(file) > 0, "");
    WI_TEST_ASSERT_NULL(wi_file_path(file), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_file_read_to_end_of_file(file), wi_data(), "");
}



void wi_test_file_runtime_functions(void) {
    wi_file_t       *file;
    wi_string_t     *path;
    
    path = wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-file-tests-1.txt"));
    file = wi_file_for_reading(path);
    
    WI_TEST_ASSERT_TRUE(wi_hash(file) >= 0, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(file), wi_file_runtime_id(), "");

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(file), path, 0), WI_NOT_FOUND, "");
}



void wi_test_file_reading(void) {
    wi_file_t       *file;
    wi_string_t     *path;
    wi_data_t       *data;
    wi_integer_t    length;
    
    path = wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-file-tests-1.txt"));
    file = wi_file_for_reading(path);
    data = wi_file_read(file, 12);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_string_utf8_data(WI_STR("hello world\n")), "");

    path = wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-file-tests-2.txt"));
    file = wi_file_for_reading(path);
    data = wi_file_read(file, 14);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_string_utf8_data(WI_STR("hello world 1\n")), "");
    
    wi_file_seek(file, wi_file_offset(file) + 14);

    data = wi_file_read(file, 14);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_string_utf8_data(WI_STR("hello world 3\n")), "");
    
    wi_file_seek_to_end_of_file(file);
    
    data = wi_file_read(file, 14);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_data(), "");
    
    wi_file_seek(file, 0);
    
    length = wi_file_write(file, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK")));
    
    WI_TEST_ASSERT_EQUALS(length, -1, "");
}




void wi_test_file_writing(void) {
    wi_file_t       *file;
    wi_string_t     *path;
    wi_data_t       *data;
    wi_integer_t    length;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-file.XXXXXXX"));
    file = wi_file_for_writing(path);
    length = wi_file_write(file, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK")));
    
    WI_TEST_ASSERT_EQUALS(length, 12, "");
    
    wi_file_seek(file, 0);
    
    data = wi_file_read_to_end_of_file(file);
    
    WI_TEST_ASSERT_NULL(data, "");
    
    wi_filesystem_delete_path(path);
}



void wi_test_file_updating(void) {
    wi_file_t       *file;
    wi_string_t     *path;
    wi_data_t       *data;
    wi_integer_t    length;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-file.XXXXXXX"));
    file = wi_file_for_updating(path);
    length = wi_file_write(file, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK")));
    
    WI_TEST_ASSERT_EQUALS(length, 12, "");
    
    wi_file_seek(file, 0);
    
    data = wi_file_read_to_end_of_file(file);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK")), "");
    
    wi_filesystem_delete_path(path);
}



void wi_test_file_truncating(void) {
    wi_file_t       *file;
    wi_string_t     *path;
    wi_data_t       *data;
    wi_integer_t    length;
    wi_boolean_t    result;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-file.XXXXXXX"));
    file = wi_file_for_updating(path);
    length = wi_file_write(file, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQK")));
    
    WI_TEST_ASSERT_EQUALS(length, 12, "");
    
    result = wi_file_truncate(file, 0);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    wi_file_seek(file, 0);
    
    data = wi_file_read_to_end_of_file(file);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_data(), "");
    
    file = wi_file_for_reading(path);
    result = wi_file_truncate(file, 0);
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    wi_filesystem_delete_path(path);
}
