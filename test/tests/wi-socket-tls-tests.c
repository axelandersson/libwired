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

WI_TEST_EXPORT void                     wi_test_socket_tls_creation(void);
WI_TEST_EXPORT void                     wi_test_socket_tls_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_socket_tls_mutation(void);


void wi_test_socket_tls_creation(void) {
#ifdef WI_SSL
    wi_socket_tls_t     *tls;
    
    tls = wi_autorelease(wi_socket_tls_init_with_type(wi_socket_tls_alloc(), WI_SOCKET_TLS_CLIENT));
    
    WI_TEST_ASSERT_NOT_NULL(tls, "");
    WI_TEST_ASSERT_NOT_NULL(wi_socket_tls_ssl_context(tls), "");
    WI_TEST_ASSERT_NULL(wi_socket_tls_certificate(tls), "");
    WI_TEST_ASSERT_NULL(wi_socket_tls_private_key(tls), "");
    WI_TEST_ASSERT_NULL(wi_socket_tls_dh(tls), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_ciphers(tls), WI_STR("ALL"), "");

    tls = wi_autorelease(wi_socket_tls_init_with_type(wi_socket_tls_alloc(), WI_SOCKET_TLS_SERVER));
    
    WI_TEST_ASSERT_NOT_NULL(tls, "");
    WI_TEST_ASSERT_NOT_NULL(wi_socket_tls_ssl_context(tls), "");
    WI_TEST_ASSERT_NULL(wi_socket_tls_certificate(tls), "");
    WI_TEST_ASSERT_NULL(wi_socket_tls_private_key(tls), "");
    WI_TEST_ASSERT_NULL(wi_socket_tls_dh(tls), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_ciphers(tls), WI_STR("ALL"), "");
#endif
}



void wi_test_socket_tls_runtime_functions(void) {
#ifdef WI_SSL
    wi_socket_tls_t     *tls;
    
    tls = wi_autorelease(wi_socket_tls_init_with_type(wi_socket_tls_alloc(), WI_SOCKET_TLS_CLIENT));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(tls), wi_socket_tls_runtime_id(), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(tls), WI_STR("wi_socket_tls_t"), 0), WI_NOT_FOUND, "");
#endif
}



void wi_test_socket_tls_mutation(void) {
#ifdef WI_SSL
    wi_socket_tls_t     *tls;
    wi_rsa_t            *private_key;
    wi_x509_t           *certificate;
    wi_boolean_t        result;
    
    tls = wi_autorelease(wi_socket_tls_init_with_type(wi_socket_tls_alloc(), WI_SOCKET_TLS_CLIENT));
    
    private_key = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    certificate = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), private_key, WI_STR("hello world")));
    result = wi_socket_tls_set_certificate(tls, certificate);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_certificate(tls), certificate, "");

    result = wi_socket_tls_set_private_key(tls, private_key);

    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_private_key(tls), private_key, "");
    
    result = wi_socket_tls_set_ciphers(tls, WI_STR("ALL"));
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_ciphers(tls), WI_STR("ALL"), "");
#endif
}
