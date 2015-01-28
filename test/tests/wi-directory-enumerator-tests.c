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

WI_TEST_EXPORT void                     wi_test_directory_enumerator(void);


void wi_test_directory_enumerator(void) {
    wi_directory_enumerator_t           *enumerator;
    wi_mutable_array_t                  *contents;
    wi_string_t                         *path, *subpath;
    wi_boolean_t                        result;
    wi_directory_enumerator_status_t    status;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-directory-enumerator.XXXXXXX"));
    path = wi_string_by_resolving_symbolic_links_in_path(path);
    result = wi_filesystem_create_directory_at_path(path);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    enumerator = wi_filesystem_directory_enumerator_at_path(path);
    
    WI_TEST_ASSERT_NOT_NULL(enumerator, "");
    
    contents = wi_mutable_array();
    
    while((status = wi_directory_enumerator_get_next_path(enumerator, &subpath)) == WI_DIRECTORY_ENUMERATOR_PATH)
        wi_mutable_array_add_data(contents, subpath);
    
    WI_TEST_ASSERT_EQUALS(status, WI_DIRECTORY_ENUMERATOR_EOF, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(contents), 0U, "");
    
    result = wi_filesystem_change_current_directory_to_path(path);
    
    WI_TEST_ASSERT_TRUE(result, "");

    result = wi_filesystem_create_directory_at_path(WI_STR("foo"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    result = wi_filesystem_create_directory_at_path(WI_STR("foo/bar"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    result = wi_filesystem_create_directory_at_path(WI_STR("foo/bar/baz"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    enumerator = wi_filesystem_directory_enumerator_at_path(path);
    
    WI_TEST_ASSERT_NOT_NULL(enumerator, "");
    
    while((status = wi_directory_enumerator_get_next_path(enumerator, &subpath)) == WI_DIRECTORY_ENUMERATOR_PATH)
        wi_mutable_array_add_data(contents, subpath);
    
    WI_TEST_ASSERT_EQUALS(status, WI_DIRECTORY_ENUMERATOR_EOF, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(contents), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(contents, 0), wi_string_by_appending_path_component(path, WI_STR("foo")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(contents, 1), wi_string_by_appending_path_component(path, WI_STR("foo/bar")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(contents, 2), wi_string_by_appending_path_component(path, WI_STR("foo/bar/baz")), "");
    
    wi_filesystem_delete_path(path);
}
