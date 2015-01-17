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

WI_TEST_EXPORT void                     wi_test_process(void);


void wi_test_process(void) {
    wi_process_t    *process;
    
    process = wi_process();
    
    WI_TEST_ASSERT_NOT_NULL(process, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(process), wi_process_runtime_id(), "");
    WI_TEST_ASSERT_TRUE(wi_string_index_of_string(wi_description(process), wi_process_name(process), 0) != WI_NOT_FOUND, "");

    WI_TEST_ASSERT_TRUE(wi_string_length(wi_process_name(process)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_string_length(wi_process_path(process)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_array_count(wi_process_arguments(process)) >= 0, "");
    
    WI_TEST_ASSERT_TRUE(wi_string_length(wi_process_hostname(process)) > 0, "");

    WI_TEST_ASSERT_TRUE(wi_string_length(wi_process_os_name(process)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_string_length(wi_process_os_release(process)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_string_length(wi_process_os_arch(process)) > 0, "");
}
