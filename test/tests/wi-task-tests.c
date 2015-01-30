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

WI_TEST_EXPORT void                     wi_test_task_creation(void);
WI_TEST_EXPORT void                     wi_test_task_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_task_launching(void);


void wi_test_task_creation(void) {
    wi_task_t   *task;
    
    task = wi_autorelease(wi_task_init(wi_task_alloc()));
    
    WI_TEST_ASSERT_NOT_NULL(task, "");
    
    wi_task_set_launch_path(task, WI_STR("echo"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_task_launch_path(task), WI_STR("echo"), "");
    
    wi_task_set_arguments(task, wi_array_with_data(WI_STR("hello world"), NULL));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_task_arguments(task), wi_array_with_data(WI_STR("hello world"), NULL), "");
}



void wi_test_task_runtime_functions(void) {
    wi_task_t   *task;
    
    task = wi_autorelease(wi_task_init(wi_task_alloc()));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(task), wi_task_runtime_id(), "");

    wi_task_set_launch_path(task, WI_STR("echo"));

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(task), WI_STR("echo"), 0), WI_NOT_FOUND, "");
}



void wi_test_task_launching(void) {
    wi_task_t       *task;
    wi_integer_t    status;
    
    task = wi_task_launched_task_with_path(WI_STR("true"), wi_array());
    status = wi_task_wait_until_exit(task);
    
    WI_TEST_ASSERT_EQUALS(status, 0, "");
    
    task = wi_task_launched_task_with_path(WI_STR("false"), wi_array());
    status = wi_task_wait_until_exit(task);
    
    WI_TEST_ASSERT_EQUALS(status, 1, "");
}
