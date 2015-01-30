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

WI_TEST_EXPORT void                     wi_test_log_file_logging(void);
WI_TEST_EXPORT void                     wi_test_log_callback_logging(void);

static void                             _wi_test_log_callback_logging_callback(wi_log_level_t, wi_string_t *);


static wi_mutable_array_t               *_wi_test_log_callback_logging_logs;


void wi_test_log_file_logging(void) {
    wi_string_t     *path, *contents;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-log.XXXXXXX"));
    
    wi_log_add_file_logger(path, 1);
    
    wi_log_info(WI_STR("hello world"));
    
    contents = wi_string_with_utf8_contents_of_file(path);
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(contents, WI_STR("Info: hello world"), 0), WI_NOT_FOUND, "");
    
    wi_log_info(WI_STR("foobar"));
    
    contents = wi_string_with_utf8_contents_of_file(path);
    
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string(contents, WI_STR("Info: hello world"), 0), WI_NOT_FOUND, "");
    
    wi_log_remove_file_logger();
    
    wi_filesystem_delete_path(path);
}



void wi_test_log_callback_logging(void) {
    _wi_test_log_callback_logging_logs = wi_array_init(wi_mutable_array_alloc());
    
    wi_log_add_callback_logger(_wi_test_log_callback_logging_callback);
    
    wi_log_debug(WI_STR("hello world 1"));
    wi_log_info(WI_STR("hello world 2"));
    wi_log_warn(WI_STR("hello world 3"));
    wi_log_error(WI_STR("hello world 4"));
    
    wi_log_remove_callback_logger();
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(_wi_test_log_callback_logging_logs,
                                   wi_array_with_data(WI_STR("hello world 1"),
                                                      WI_STR("hello world 2"),
                                                      WI_STR("hello world 3"),
                                                      WI_STR("hello world 4"), NULL),
                                   "");
    
    wi_release(_wi_test_log_callback_logging_logs);
    _wi_test_log_callback_logging_logs = NULL;
}



static void _wi_test_log_callback_logging_callback(wi_log_level_t level, wi_string_t *line) {
    wi_mutable_array_add_data(_wi_test_log_callback_logging_logs, line);
}
