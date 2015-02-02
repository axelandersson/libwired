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

WI_TEST_EXPORT void                     wi_test_filesystem_successes(void);
WI_TEST_EXPORT void                     wi_test_filesystem_failures(void);

static void                             _wi_test_filesystem_successes_copy_callback(wi_string_t *, wi_string_t *);
static void                             _wi_test_filesystem_successes_delete_callback(wi_string_t *);


void wi_test_filesystem_successes(void) {
    wi_array_t              *contents;
    wi_string_t             *path, *otherpath;
    wi_file_stats_t         file_stats;
    wi_filesystem_stats_t   filesystem_stats;
    wi_boolean_t            result, is_directory;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-filesystem.XXXXXXX"));
    
    WI_TEST_ASSERT_NOT_NULL(path, "");
    
    result = wi_filesystem_create_directory_at_path(path);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    result = wi_filesystem_file_exists_at_path(path, &is_directory);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_TRUE(is_directory, "");
    
    result = wi_filesystem_change_current_directory_to_path(path);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    otherpath = wi_filesystem_current_directory_path();
    
    WI_TEST_ASSERT_NOT_NULL(otherpath, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(otherpath, wi_string_by_resolving_symbolic_links_in_path(path), "");
    
    contents = wi_filesystem_directory_contents_at_path(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(contents, wi_array(), "");
    
    result = wi_filesystem_create_directory_at_path(WI_STR("foobar"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    contents = wi_filesystem_directory_contents_at_path(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(contents, wi_array_with_data(WI_STR("foobar"), NULL), "");
    
    result = wi_string_write_utf8_string_to_path(WI_STR("hello world"), WI_STR("foobar/foobar"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    result = wi_filesystem_get_file_stats_for_path(WI_STR("foobar/foobar"), &file_stats);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_TRUE(file_stats.filesystem_id > 0, "");
    WI_TEST_ASSERT_TRUE(file_stats.file_id > 0, "");
    WI_TEST_ASSERT_EQUALS(file_stats.file_type, WI_FILE_REGULAR, "");
    WI_TEST_ASSERT_TRUE(file_stats.posix_permissions > 0, "");
    WI_TEST_ASSERT_EQUALS(file_stats.reference_count, 1, "");
    WI_TEST_ASSERT_EQUALS(file_stats.size, 11, "");
    WI_TEST_ASSERT_NOT_NULL(file_stats.user, "");
    WI_TEST_ASSERT_TRUE(wi_string_length(file_stats.user) > 0, "");
    WI_TEST_ASSERT_NOT_NULL(file_stats.group, "");
    WI_TEST_ASSERT_TRUE(wi_string_length(file_stats.group) > 0, "");
    WI_TEST_ASSERT_NOT_NULL(file_stats.creation_date, "");
    WI_TEST_ASSERT_TRUE(wi_date_time_interval(file_stats.creation_date) > 0, "");
    WI_TEST_ASSERT_NOT_NULL(file_stats.modification_date, "");
    WI_TEST_ASSERT_TRUE(wi_date_time_interval(file_stats.modification_date) > 0, "");
    
    result = wi_filesystem_get_filesystem_stats_for_path(WI_STR("foobar/foobar"), &filesystem_stats);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    WI_TEST_ASSERT_TRUE(filesystem_stats.size > 0, "");
    WI_TEST_ASSERT_TRUE(filesystem_stats.free_size > 0, "");
    WI_TEST_ASSERT_TRUE(filesystem_stats.free_size < filesystem_stats.size, "");
    WI_TEST_ASSERT_TRUE(filesystem_stats.nodes > 0, "");
    WI_TEST_ASSERT_TRUE(filesystem_stats.free_nodes > 0, "");
    WI_TEST_ASSERT_TRUE(filesystem_stats.free_nodes < filesystem_stats.nodes, "");
    
    result = wi_filesystem_create_symbolic_link_from_path(WI_STR("foobar"), WI_STR("foobar/foobaz"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    result = wi_filesystem_copy_path_with_callback(WI_STR("foobar"), WI_STR("foobaz"), _wi_test_filesystem_successes_copy_callback);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    contents = wi_filesystem_directory_contents_at_path(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(contents, wi_array_with_data(WI_STR("foobar"), WI_STR("foobaz"), NULL), "");
    
    result = wi_filesystem_delete_path_with_callback(path, _wi_test_filesystem_successes_delete_callback);
    
    WI_TEST_ASSERT_TRUE(result, "");
}



static void _wi_test_filesystem_successes_copy_callback(wi_string_t *frompath, wi_string_t *topath) {
    WI_TEST_ASSERT_NOT_NULL(frompath, "");
    WI_TEST_ASSERT_NOT_NULL(topath, "");
}



static void _wi_test_filesystem_successes_delete_callback(wi_string_t *path) {
    WI_TEST_ASSERT_NOT_NULL(path, "");
}



void wi_test_filesystem_failures(void) {
    wi_array_t              *contents;
    wi_string_t             *path, *otherpath;
    wi_file_stats_t         file_stats;
    wi_filesystem_stats_t   filesystem_stats;
    wi_boolean_t            result, is_directory;
    
    result = wi_filesystem_create_directory_at_path(WI_STR("/non/existing/directory"));
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_file_exists_at_path(WI_STR("/non/existing/directory"), NULL);
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_change_current_directory_to_path(WI_STR("/non/existing/directory"));
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    contents = wi_filesystem_directory_contents_at_path(WI_STR("/non/existing/directory"));
    
    WI_TEST_ASSERT_NULL(contents, "");
    
    result = wi_filesystem_create_directory_at_path(WI_STR("/non/existing/directory"));
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_get_file_stats_for_path(WI_STR("/non/existing/file"), &file_stats);
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_get_filesystem_stats_for_path(WI_STR("/non/existing/file"), &filesystem_stats);
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_create_symbolic_link_from_path(WI_STR("/non/existing/file1"), WI_STR("/non/existing/file2"));
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_copy_path(WI_STR("/non/existing/file1"), WI_STR("/non/existing/file2"));
    
    WI_TEST_ASSERT_FALSE(result, "");
    
    result = wi_filesystem_delete_path(WI_STR("/non/existing/directory"));
    
    WI_TEST_ASSERT_FALSE(result, "");
}
