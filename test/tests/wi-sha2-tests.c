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

WI_TEST_EXPORT void                     wi_test_sha2_creation(void);
WI_TEST_EXPORT void                     wi_test_sha2_digest(void);



void wi_test_sha2_creation(void) {
#ifdef WI_SHA1
    wi_sha2_t   *sha2;
    
    sha2 = wi_sha2_with_bits(WI_SHA2_256);
    
    WI_TEST_ASSERT_NOT_NULL(sha2, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(sha2), wi_sha2_runtime_id(), "");
#endif
}



void wi_test_sha2_digest(void) {
#ifdef WI_SHA1
    wi_sha2_t       *sha2;
    wi_data_t       *data;
    unsigned char   buffer[WI_SHA2_MAX_LENGTH];
    
    data = wi_data();
    
    wi_sha2_digest(WI_SHA2_256, wi_data_bytes(data), wi_data_length(data), buffer);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_SHA2_256_LENGTH), wi_data_with_base64_string(WI_STR("47DEQpj8HBSa+/TImW+5JCeuQeRkm5NMpJWZG3hSuFU=")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_sha2_digest_string(WI_SHA2_256, data), WI_STR("e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"), "");
    
    sha2 = wi_sha2_with_bits(WI_SHA2_512);
    
    wi_sha2_close(sha2);
    wi_sha2_get_data(sha2, buffer);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_data_with_bytes(buffer, WI_SHA2_512_LENGTH), wi_data_with_base64_string(WI_STR("z4PhNX7vuL3xVChQ1m2AB9Yg5AULVxXcg/SpIdNs6c5H0NE8XYXysP+DGNKHfuwvY7kxvUdBeoGlODJ6+SfaPg==")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_sha2_data(sha2), wi_data_with_base64_string(WI_STR("z4PhNX7vuL3xVChQ1m2AB9Yg5AULVxXcg/SpIdNs6c5H0NE8XYXysP+DGNKHfuwvY7kxvUdBeoGlODJ6+SfaPg==")), "");
#endif
}
