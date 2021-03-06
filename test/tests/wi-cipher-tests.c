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

WI_TEST_EXPORT void                     wi_test_cipher_creation(void);
WI_TEST_EXPORT void                     wi_test_cipher_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_cipher_suites(void);

#ifdef WI_CIPHERS
static void                             _wi_test_cipher_suite(wi_cipher_type_t, wi_string_t *, wi_uinteger_t, wi_data_t *, wi_data_t *);
#endif



void wi_test_cipher_creation(void) {
    wi_cipher_t     *cipher;
    
    cipher = wi_autorelease(wi_cipher_init_with_key(wi_cipher_alloc(), WI_CIPHER_AES128, wi_data(), wi_data()));
    
    WI_TEST_ASSERT_NOT_NULL(cipher, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_key(cipher), wi_data(), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_iv(cipher), wi_data(), "");
    
    cipher = wi_autorelease(wi_cipher_init_with_random_key(wi_cipher_alloc(), WI_CIPHER_AES128));
    
    WI_TEST_ASSERT_NOT_NULL(cipher, "");
    WI_TEST_ASSERT_EQUALS(wi_data_length(wi_cipher_key(cipher)), 16U, "");
    WI_TEST_ASSERT_EQUALS(wi_data_length(wi_cipher_iv(cipher)), 16U, "");
    
    cipher = wi_autorelease(wi_cipher_init_with_key(wi_cipher_alloc(), 999, wi_data(), wi_data()));

    WI_TEST_ASSERT_NULL(cipher, "");
    
    cipher = wi_autorelease(wi_cipher_init_with_random_key(wi_cipher_alloc(), 999));
    
    WI_TEST_ASSERT_NULL(cipher, "");
}



void wi_test_cipher_runtime_functions(void) {
    wi_cipher_t     *cipher;
    
    cipher = wi_autorelease(wi_cipher_init_with_key(wi_cipher_alloc(), WI_CIPHER_AES128, wi_data(), wi_data()));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(cipher), wi_cipher_runtime_id(), "");
}



void wi_test_cipher_suites(void) {
#ifdef WI_CIPHERS
    _wi_test_cipher_suite(WI_CIPHER_AES128, WI_STR("AES"), 128,
                          wi_data_with_base64_string(WI_STR("ThMdgpVRxOZ+tgJQSmp84w==")),
                          wi_data_with_base64_string(WI_STR("bHHG4L6aGKGsGIzA82DVvQ==")));
    
    _wi_test_cipher_suite(WI_CIPHER_AES192, WI_STR("AES"), 192,
                          wi_data_with_base64_string(WI_STR("DOBad099mTPH6lagfKPRsJAb46fVaiol")),
                          wi_data_with_base64_string(WI_STR("QoJeh/+7zxVAQAX8h88QgA==")));
    
    _wi_test_cipher_suite(WI_CIPHER_AES256, WI_STR("AES"), 256,
                          wi_data_with_base64_string(WI_STR("BZofvj2yZm+pF0Lu+ebDP65XPv1Qbj3eLEIOx9dOLT4=")),
                          wi_data_with_base64_string(WI_STR("1qq13sv6H+sA8vn72Vs1hQ==")));
    
    _wi_test_cipher_suite(WI_CIPHER_BF128, WI_STR("Blowfish"), 128,
                          wi_data_with_base64_string(WI_STR("k96E++BNrz/nvEqRKfK2DA==")),
                          wi_data_with_base64_string(WI_STR("k4NWyhAd0F0=")));
    
    _wi_test_cipher_suite(WI_CIPHER_3DES192, WI_STR("Triple DES"), 192,
                          wi_data_with_base64_string(WI_STR("bqXg+ZSQxitsx5ynTe04m7tNq6PDNQoF")),
                          wi_data_with_base64_string(WI_STR("mY2Zs19VJeE=")));
#endif
}



#ifdef WI_CIPHERS

static void _wi_test_cipher_suite(wi_cipher_type_t type, wi_string_t *name, wi_uinteger_t bits, wi_data_t *key, wi_data_t *iv) {
    wi_cipher_t     *cipher;
    
    cipher = wi_autorelease(wi_cipher_init_with_key(wi_cipher_alloc(), type, key, iv));
    
    WI_TEST_ASSERT_NOT_NULL(cipher, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(cipher), name, 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_key(cipher), key, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_iv(cipher), iv, "");
    WI_TEST_ASSERT_EQUALS(wi_cipher_type(cipher), type, "");
    WI_TEST_ASSERT_EQUALS(wi_cipher_bits(cipher), bits, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_name(cipher), name, "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_decrypt(cipher, wi_cipher_encrypt(cipher, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")))),
                                   wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")), "");
    
    cipher = wi_autorelease(wi_cipher_init_with_random_key(wi_cipher_alloc(), type));

    WI_TEST_ASSERT_NOT_NULL(cipher, "");
    WI_TEST_ASSERT_EQUALS(wi_cipher_type(cipher), type, "");
    WI_TEST_ASSERT_EQUALS(wi_cipher_bits(cipher), bits, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_name(cipher), name, "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_cipher_decrypt(cipher, wi_cipher_encrypt(cipher, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")))),
                                   wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")), "");
}

#endif
