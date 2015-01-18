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

WI_TEST_EXPORT void                     wi_test_digest_creation(void);
WI_TEST_EXPORT void                     wi_test_digest_md5(void);
WI_TEST_EXPORT void                     wi_test_digest_sha1(void);
WI_TEST_EXPORT void                     wi_test_digest_base64(void);



void wi_test_digest_creation(void) {
#ifdef WI_DIGESTS
    wi_md5_t    *md5;
    wi_sha1_t   *sha1;
    
    md5 = wi_md5();
    
    WI_TEST_ASSERT_NOT_NULL(md5, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(md5), wi_md5_runtime_id(), "");
    
    md5 = wi_autorelease(wi_md5_init(wi_md5_alloc()));
    
    WI_TEST_ASSERT_NOT_NULL(md5, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(md5), wi_md5_runtime_id(), "");
    
    sha1 = wi_sha1();
    
    WI_TEST_ASSERT_NOT_NULL(sha1, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(sha1), wi_sha1_runtime_id(), "");
    
    sha1 = wi_autorelease(wi_sha1_init(wi_sha1_alloc()));
    
    WI_TEST_ASSERT_NOT_NULL(sha1, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(sha1), wi_sha1_runtime_id(), "");
#endif
}



void wi_test_digest_md5(void) {
    wi_md5_t        *md5;
    wi_data_t       *data;
    unsigned char   buffer[WI_MD5_DIGEST_LENGTH];
    
    data = wi_data();
    
    wi_md5_digest(wi_data_bytes(data), wi_data_length(data), buffer);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_MD5_DIGEST_LENGTH), wi_data_with_base64(WI_STR("1B2M2Y8AsgTpgAmY7PhCfg==")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_md5_digest_string(data), WI_STR("d41d8cd98f00b204e9800998ecf8427e"), "");
    
    md5 = wi_md5();
    
    wi_md5_close(md5);
    wi_md5_get_data(md5, buffer);

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_MD5_DIGEST_LENGTH), wi_data_with_base64(WI_STR("1B2M2Y8AsgTpgAmY7PhCfg==")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_md5_data(md5), wi_data_with_base64(WI_STR("1B2M2Y8AsgTpgAmY7PhCfg==")), "");
}



void wi_test_digest_sha1(void) {
    wi_sha1_t       *sha1;
    wi_data_t       *data;
    unsigned char   buffer[WI_SHA1_DIGEST_LENGTH];
    
    data = wi_data();
    
    wi_sha1_digest(wi_data_bytes(data), wi_data_length(data), buffer);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_SHA1_DIGEST_LENGTH), wi_data_with_base64(WI_STR("2jmj7l5rSw0yVb/vlWAYkK/YBwk=")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_sha1_digest_string(data), WI_STR("da39a3ee5e6b4b0d3255bfef95601890afd80709"), "");
    
    sha1 = wi_sha1();
    
    wi_sha1_close(sha1);
    wi_sha1_get_data(sha1, buffer);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_SHA1_DIGEST_LENGTH), wi_data_with_base64(WI_STR("2jmj7l5rSw0yVb/vlWAYkK/YBwk=")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_sha1_data(sha1), wi_data_with_base64(WI_STR("2jmj7l5rSw0yVb/vlWAYkK/YBwk=")), "");
}



void wi_test_digest_base64(void) {
    wi_string_t     *string;
    wi_data_t       *data;
    
    data = wi_string_data(WI_STR("hello world"));
    string = wi_base64_string_from_data(data);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("aGVsbG8gd29ybGQ="), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_from_base64_string(string), wi_string_data(WI_STR("hello world")), "");
}
