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

#if defined(WI_FILESYSTEM_EVENTS) && defined(WI_PTHREADS)
static void                             _wi_test_filesystem_events_thread(wi_runtime_instance_t *);
static void                             _wi_test_filesystem_events_callback(wi_string_t *);
#endif


#if defined(WI_FILESYSTEM_EVENTS) && defined(WI_PTHREADS)
static wi_condition_lock_t              *wi_test_filesystem_events_lock;
static wi_mutable_array_t               *wi_test_filesystem_events_paths;
#endif


#if defined(WI_PTHREADS) && defined(WI_FILESYSTEM_EVENTS)
//static void                             wi_test_fsevents_thread(wi_runtime_instance_t *);
//static void                             wi_test_fsevents_callback(wi_string_t *);
//
//
//static wi_fsevents_t                    *wi_test_fsevents_fsevents;
//static wi_condition_lock_t              *wi_test_fsevents_lock;
//static wi_fsevents_t                    *wi_test_fsevents_path;
#endif


void wi_test_filesystem_events(void) {
#if defined(WI_FILESYSTEM_EVENTS) && defined(WI_PTHREADS)
    wi_filesystem_events_t  *filesystem_events;
    wi_string_t             *path;
    wi_boolean_t            result;
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-filesystem.XXXXXXX"));
    
    WI_TEST_ASSERT_NOT_NULL(path, "");
    
    result = wi_filesystem_create_directory_at_path(path);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    filesystem_events = wi_filesystem_events();
    
    WI_TEST_ASSERT_NOT_NULL(filesystem_events, "");
    
    wi_filesystem_events_set_callback(filesystem_events, _wi_test_filesystem_events_callback);
    
    result = wi_filesystem_events_add_path(filesystem_events, path);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    wi_test_filesystem_events_lock = wi_autorelease(wi_condition_lock_init_with_condition(wi_condition_lock_alloc(), 0));
    wi_test_filesystem_events_paths = wi_mutable_array();
    
    result = wi_thread_create_thread(_wi_test_filesystem_events_thread, filesystem_events);
    
    WI_TEST_ASSERT_TRUE(result, "");

    if(wi_condition_lock_lock_when_condition(wi_test_filesystem_events_lock, 1, 1.0))
        wi_condition_lock_unlock(wi_test_filesystem_events_lock);
    else
        WI_TEST_FAIL("timed out waiting for filesystem events thread");
    
    result = wi_filesystem_create_directory_at_path(wi_string_by_appending_path_component(path, WI_STR("foobar")));
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    if(wi_condition_lock_lock_when_condition(wi_test_filesystem_events_lock, 2, 1.0)) {
        WI_TEST_ASSERT_EQUAL_INSTANCES(wi_test_filesystem_events_paths, wi_array_with_data(path, NULL), "");
        
        wi_condition_lock_unlock(wi_test_filesystem_events_lock);
    } else {
        WI_TEST_FAIL("timed out waiting for filesystem events result");
    }
    
    wi_filesystem_delete_path(path);
#endif
}



#if defined(WI_FILESYSTEM_EVENTS) && defined(WI_PTHREADS)

static void _wi_test_filesystem_events_thread(wi_runtime_instance_t *instance) {
    wi_filesystem_events_t  *filesystem_events = instance;
    wi_pool_t               *pool;
    wi_boolean_t            result;
    
    pool = wi_pool_init(wi_pool_alloc());
    
    wi_condition_lock_lock(wi_test_filesystem_events_lock);
    wi_condition_lock_unlock_with_condition(wi_test_filesystem_events_lock, 1);
    
    result = wi_filesystem_events_run_with_timeout(filesystem_events, 1.0);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    wi_condition_lock_lock(wi_test_filesystem_events_lock);
    wi_condition_lock_unlock_with_condition(wi_test_filesystem_events_lock, 2);
    
    wi_release(pool);
}



static void _wi_test_filesystem_events_callback(wi_string_t *path) {
    wi_mutable_array_add_data(wi_test_filesystem_events_paths, path);
}

#endif
