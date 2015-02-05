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

WI_TEST_EXPORT void                     wi_test_md5_creation(void);
WI_TEST_EXPORT void                     wi_test_md5_digest(void);



void wi_test_md5_creation(void) {
#ifdef WI_DIGESTS
    wi_md5_t    *md5;
    
    md5 = wi_md5();
    
    WI_TEST_ASSERT_NOT_NULL(md5, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(md5), wi_md5_runtime_id(), "");
    
    md5 = wi_autorelease(wi_md5_init(wi_md5_alloc()));
    
    WI_TEST_ASSERT_NOT_NULL(md5, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(md5), wi_md5_runtime_id(), "");
#endif
}



void wi_test_md5_digest(void) {
    wi_md5_t        *md5;
    wi_data_t       *data;
    unsigned char   buffer[WI_MD5_LENGTH];
    
    data = wi_data();
    
    wi_md5_digest(wi_data_bytes(data), wi_data_length(data), buffer);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_MD5_LENGTH), wi_data_with_base64_string(WI_STR("1B2M2Y8AsgTpgAmY7PhCfg==")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_md5_digest_string(data), WI_STR("d41d8cd98f00b204e9800998ecf8427e"), "");
    
    md5 = wi_md5();
    
    wi_md5_close(md5);
    wi_md5_get_data(md5, buffer);

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_MD5_LENGTH), wi_data_with_base64_string(WI_STR("1B2M2Y8AsgTpgAmY7PhCfg==")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_md5_data(md5), wi_data_with_base64_string(WI_STR("1B2M2Y8AsgTpgAmY7PhCfg==")), "");
}
