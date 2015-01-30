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

WI_TEST_EXPORT void                     wi_test_pipe_creation(void);
WI_TEST_EXPORT void                     wi_test_pipe_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_pipe_reading_and_writing(void);


void wi_test_pipe_creation(void) {
    wi_pipe_t   *pipe;
    
    pipe = wi_pipe();
    
    WI_TEST_ASSERT_NOT_NULL(pipe, "");
    WI_TEST_ASSERT_TRUE(wi_pipe_descriptor_for_reading(pipe) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_pipe_descriptor_for_writing(pipe) > 0, "");
}



void wi_test_pipe_runtime_functions(void) {
    wi_pipe_t   *pipe;
    
    pipe = wi_pipe();
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(pipe), wi_pipe_runtime_id(), "");

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(pipe), WI_STR("wi_pipe_t"), 0), WI_NOT_FOUND, "");
}



void wi_test_pipe_reading_and_writing(void) {
    wi_pipe_t   *pipe;
    wi_data_t   *data;
    
    pipe = wi_pipe();
    
    wi_pipe_write(pipe, wi_string_utf8_data(WI_STR("hello world\n")));
    
    data = wi_pipe_read(pipe, 12);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_utf8_data(data), WI_STR("hello world\n"), "");
}
