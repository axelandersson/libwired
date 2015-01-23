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

WI_TEST_EXPORT void                     wi_test_uuid_creation(void);
WI_TEST_EXPORT void                     wi_test_uuid_runtime_functions(void);


void wi_test_uuid_creation(void) {
    wi_uuid_t       *uuid;
    wi_data_t       *data;
    wi_string_t     *string, *time_string, *node_string;
    char            buffer[WI_UUID_BUFFER_SIZE];
    
    uuid = wi_uuid();
    
    WI_TEST_ASSERT_NOT_NULL(uuid, "");
    
    uuid = wi_uuid_with_string(WI_STR("foobar"));
    
    WI_TEST_ASSERT_NULL(uuid, "");
    
    uuid = wi_uuid_with_string(WI_STR("3545D40D-6F34-4DCE-8732-61D183A41DF4"));

    WI_TEST_ASSERT_NOT_NULL(uuid, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_uuid_string(uuid), WI_STR("3545D40D-6F34-4DCE-8732-61D183A41DF4"), "");
    
    data = wi_data_with_base64_string(WI_STR("NUXUDW80Tc6HMmHRg6Qd9A=="));
    uuid = wi_uuid_with_bytes(wi_data_bytes(data));
    
    wi_uuid_get_bytes(uuid, buffer);
    
    WI_TEST_ASSERT_NOT_NULL(uuid, "");
    WI_TEST_ASSERT_EQUALS(memcmp(buffer, wi_data_bytes(data), WI_UUID_BUFFER_SIZE), 0, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_uuid_string(uuid), WI_STR("3545D40D-6F34-4DCE-8732-61D183A41DF4"), "");
    
    uuid = wi_autorelease(wi_uuid_init_from_random_data(wi_uuid_alloc()));
    
    WI_TEST_ASSERT_NOT_NULL(uuid, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(wi_uuid_string(uuid)), 36U, "");
    
    uuid = wi_autorelease(wi_uuid_init_from_time(wi_uuid_alloc()));
    string = wi_uuid_string(uuid);
    time_string = wi_string_substring_to_index(string, 23);
    node_string = wi_string_substring_from_index(string, 24);
    
    WI_TEST_ASSERT_NOT_NULL(uuid, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 36U, "");
    
    uuid = wi_autorelease(wi_uuid_init_from_time(wi_uuid_alloc()));
    string = wi_uuid_string(uuid);
    
    WI_TEST_ASSERT_NOT_NULL(uuid, "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(wi_string_substring_to_index(string, 23), time_string, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_substring_from_index(string, 24), node_string, "");
}



void wi_test_uuid_runtime_functions(void) {
    wi_uuid_t   *uuid1, *uuid2;
    
    uuid1 = wi_uuid();
    uuid2 = wi_uuid_with_string(wi_uuid_string(uuid1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(uuid1, uuid2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(uuid1), wi_hash(uuid2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(uuid1), wi_uuid_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(uuid2), wi_uuid_runtime_id(), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(uuid1), wi_uuid_string(uuid1), 0), WI_NOT_FOUND, "");
}
