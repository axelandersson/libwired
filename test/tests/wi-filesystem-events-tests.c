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

WI_TEST_EXPORT void                     wi_test_filesystem_events(void);

#if defined(WI_FILESYSTEM_EVENTS)
static void                             _wi_test_filesystem_events_callback(wi_filesystem_events_t *, wi_string_t *);
#endif


#if defined(WI_FILESYSTEM_EVENTS)
static wi_condition_lock_t              *_wi_test_filesystem_events_lock;
static wi_mutable_set_t                 *_wi_test_filesystem_events_paths;
#endif



void wi_test_filesystem_events(void) {
#if defined(WI_FILESYSTEM_EVENTS)
    wi_filesystem_events_t  *filesystem_events;
    wi_string_t             *path;
    wi_boolean_t            result;
    
    _wi_test_filesystem_events_lock = wi_autorelease(wi_condition_lock_init_with_condition(wi_condition_lock_alloc(), 0));
    _wi_test_filesystem_events_paths = wi_mutable_set();
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-filesystem.XXXXXXX"));
    
    WI_TEST_ASSERT_NOT_NULL(path, "");
    
    result = wi_filesystem_create_directory_at_path(path);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    filesystem_events = wi_filesystem_events();
    
    WI_TEST_ASSERT_NOT_NULL(filesystem_events, "");
    
    result = wi_filesystem_events_add_path_with_callback(filesystem_events, path, _wi_test_filesystem_events_callback);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    result = wi_string_write_utf8_string_to_path(WI_STR("hello world"), wi_string_by_appending_path_component(path, WI_STR("foobar")));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    if(!wi_condition_lock_lock_when_condition(_wi_test_filesystem_events_lock, 1, 1.0))
        WI_TEST_FAIL("timed out waiting for filesystem events thread");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(_wi_test_filesystem_events_paths, wi_set_with_data(path, NULL), "");
    
    wi_condition_lock_unlock(_wi_test_filesystem_events_lock);
#endif
}



static void _wi_test_filesystem_events_callback(wi_filesystem_events_t *filesystem_events, wi_string_t *path) {
    wi_condition_lock_lock(_wi_test_filesystem_events_lock);
    
    wi_mutable_set_add_data(_wi_test_filesystem_events_paths, path);
    
    wi_condition_lock_unlock_with_condition(_wi_test_filesystem_events_lock, 1);
    
    wi_filesystem_events_remove_path(filesystem_events, path);
}
