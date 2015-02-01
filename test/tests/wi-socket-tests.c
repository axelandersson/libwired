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

WI_TEST_EXPORT void                     wi_test_socket_creation(void);
WI_TEST_EXPORT void                     wi_test_socket_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_socket_settings(void);
WI_TEST_EXPORT void                     wi_test_socket_plaintext_client_server(void);
WI_TEST_EXPORT void                     wi_test_socket_secure_client_server(void);


#ifdef WI_PTHREADS
static void                             _wi_test_socket_plaintext_client_server_thread(wi_runtime_instance_t *);
static void                             _wi_test_socket_secure_client_server_thread(wi_runtime_instance_t *);


static wi_condition_lock_t              *_wi_test_socket_condition_lock;
#endif


void wi_test_socket_creation(void) {
    wi_socket_t     *socket;
    wi_address_t    *address;
    
    address = wi_address_with_string(WI_STR("127.0.0.1"));
    socket = wi_socket_with_address(address, WI_SOCKET_TCP);

    WI_TEST_ASSERT_NOT_NULL(socket, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_address(socket), address, "");
    WI_TEST_ASSERT_TRUE(wi_socket_descriptor(socket) > 0, "");
    
    socket = wi_autorelease(wi_socket_init_with_descriptor(wi_socket_alloc(), 1));

    WI_TEST_ASSERT_NOT_NULL(socket, "");
    WI_TEST_ASSERT_NULL(wi_socket_address(socket), "");
}



void wi_test_socket_runtime_functions(void) {
    wi_socket_t     *socket;
    
    socket = wi_autorelease(wi_socket_init_with_descriptor(wi_socket_alloc(), 1));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(socket), wi_socket_runtime_id(), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(socket), WI_STR("1"), 0), WI_NOT_FOUND, "");
}



void wi_test_socket_settings(void) {
    wi_socket_t     *socket;
    wi_address_t    *address;
    wi_boolean_t    result;
    
    address = wi_address_with_string(WI_STR("127.0.0.1"));
    socket = wi_socket_with_address(address, WI_SOCKET_TCP);
    
    wi_socket_set_port(socket, 80);
    
    WI_TEST_ASSERT_EQUALS(wi_socket_port(socket), 80U, "");
    WI_TEST_ASSERT_EQUALS(wi_address_port(wi_socket_address(socket)), 80U, "");
    
    wi_socket_set_direction(socket, WI_SOCKET_WRITE);

    WI_TEST_ASSERT_EQUALS(wi_socket_direction(socket), WI_SOCKET_WRITE, "");
    
    wi_socket_set_data(socket, WI_STR("test"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_data(socket), WI_STR("test"), "");
    
    result = wi_socket_set_blocking(socket, true);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_TRUE(wi_socket_blocking(socket), "");
    
    wi_socket_set_timeout(socket, 10.0);
    
    WI_TEST_ASSERT_TRUE(result, "");
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_socket_timeout(socket), 10.0, 0.001, "");
    
    wi_socket_set_interactive(socket, true);
    
    WI_TEST_ASSERT_TRUE(wi_socket_interactive(socket), "");
}




void wi_test_socket_plaintext_client_server(void) {
#ifdef WI_PTHREADS
    wi_socket_t         *client_socket, *waiting_socket;
    wi_address_t        *server_address;
    wi_data_t           *data;
    wi_socket_state_t   state;
    wi_boolean_t        result;
    
    _wi_test_socket_condition_lock = wi_autorelease(wi_condition_lock_init_with_condition(wi_condition_lock_alloc(), 0));
    
    result = wi_thread_create_thread(_wi_test_socket_plaintext_client_server_thread, NULL);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    if(!wi_condition_lock_lock_when_condition(_wi_test_socket_condition_lock, 1, 2.0))
        WI_TEST_FAIL("timed out waiting for socket");
    
    server_address = wi_host_address(wi_host_with_string(WI_STR("localhost")));
    
    WI_TEST_ASSERT_NOT_NULL(server_address, "");
    
    client_socket = wi_socket_with_address(server_address, WI_SOCKET_TCP);
    
    wi_socket_set_direction(client_socket, WI_SOCKET_READ);
    wi_socket_set_port(client_socket, 4871);
    
    result = wi_socket_connect(client_socket, 2.0);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    state = wi_socket_wait(client_socket, 2.0);
    
    WI_TEST_ASSERT_EQUALS(state, WI_SOCKET_READY, "");
    
    waiting_socket = wi_socket_wait_multiple(wi_array_with_data(client_socket, NULL), 2.0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(waiting_socket, client_socket, "");
    
    data = wi_socket_read_data(client_socket, 2.0, 11);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_string_utf8_data(WI_STR("hello world")), "");
    
    wi_condition_lock_unlock(_wi_test_socket_condition_lock);
#endif
}



#ifdef WI_PTHREADS

static void _wi_test_socket_plaintext_client_server_thread(wi_runtime_instance_t *instance) {
    wi_pool_t       *pool;
    wi_socket_t     *server_socket, *client_socket;
    wi_address_t    *server_address, *client_address;
    wi_boolean_t    result;
    
    pool = wi_pool_init(wi_pool_alloc());
    
    wi_condition_lock_lock(_wi_test_socket_condition_lock);
    
    server_address = wi_host_address(wi_host_with_string(WI_STR("localhost")));
    
    WI_TEST_ASSERT_NOT_NULL(server_address, "");
    
    server_socket = wi_socket_with_address(server_address, WI_SOCKET_TCP);
    
    wi_socket_set_direction(server_socket, WI_SOCKET_WRITE);
    wi_socket_set_port(server_socket, 4871);
    
    result = wi_socket_listen(server_socket);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    wi_condition_lock_unlock_with_condition(_wi_test_socket_condition_lock, 1);
    
    client_socket = wi_socket_accept_multiple(wi_array_with_data(server_socket, NULL), 2.0, &client_address);
    
    WI_TEST_ASSERT_NOT_NULL(client_socket, "");
    WI_TEST_ASSERT_NOT_NULL(client_address, "");
    
    wi_socket_write_data(client_socket, 2.0, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")));
    
    wi_release(pool);
}

#endif



void wi_test_socket_secure_client_server(void) {
#if defined(WI_PTHREADS) && defined(WI_SSL)
    wi_socket_tls_t     *tls;
    wi_socket_t         *client_socket, *waiting_socket;
    wi_address_t        *server_address;
    wi_data_t           *data;
    wi_x509_t           *x509;
    wi_socket_state_t   state;
    wi_boolean_t        result;
    
    _wi_test_socket_condition_lock = wi_autorelease(wi_condition_lock_init_with_condition(wi_condition_lock_alloc(), 0));
    
    result = wi_thread_create_thread(_wi_test_socket_secure_client_server_thread, NULL);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    if(!wi_condition_lock_lock_when_condition(_wi_test_socket_condition_lock, 1, 2.0))
        WI_TEST_FAIL("timed out waiting for socket");
    
    server_address = wi_host_address(wi_host_with_string(WI_STR("localhost")));
    
    WI_TEST_ASSERT_NOT_NULL(server_address, "");
    
    client_socket = wi_socket_with_address(server_address, WI_SOCKET_TCP);
    
    wi_socket_set_direction(client_socket, WI_SOCKET_WRITE | WI_SOCKET_READ);
    wi_socket_set_port(client_socket, 4871);
    
    result = wi_socket_connect(client_socket, 5.0);
    
    WI_TEST_ASSERT_TRUE(result, "");

    wi_socket_set_tls_ciphers(client_socket, WI_STR("ALL"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_ciphers(client_socket), WI_STR("ALL"), "");
    
    result = wi_socket_connect_tls(client_socket, 5.0);
    
    WI_TEST_ASSERT_TRUE(result, "%m");
    WI_TEST_ASSERT_TRUE(wi_string_length(wi_socket_tls_remote_cipher_version(client_socket)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_string_length(wi_socket_tls_remote_cipher_name(client_socket)) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_socket_tls_remote_cipher_bits(client_socket) > 0, "");
    
    x509 = wi_socket_tls_remote_certificate(client_socket);
    
    WI_TEST_ASSERT_NOT_NULL(x509, "");
    
    data = wi_socket_read_data(client_socket, 5.0, 11);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(data, wi_string_utf8_data(WI_STR("hello world")), "");
    
    wi_condition_lock_unlock(_wi_test_socket_condition_lock);
#endif
}



#if defined(WI_PTHREADS) && defined(WI_SSL)

static void _wi_test_socket_secure_client_server_thread(wi_runtime_instance_t *instance) {
    wi_pool_t           *pool;
    wi_socket_tls_t     *tls;
    wi_socket_t         *server_socket, *client_socket;
    wi_address_t        *server_address, *client_address;
    wi_rsa_t            *rsa;
    wi_x509_t           *x509;
    wi_dh_t             *dh;
    wi_boolean_t        result;
    
    pool = wi_pool_init(wi_pool_alloc());
    
    wi_condition_lock_lock(_wi_test_socket_condition_lock);
    
    server_address = wi_host_address(wi_host_with_string(WI_STR("localhost")));
    
    WI_TEST_ASSERT_NOT_NULL(server_address, "");
    
    server_socket = wi_socket_with_address(server_address, WI_SOCKET_TCP);
    
    wi_socket_set_direction(server_socket, WI_SOCKET_WRITE | WI_SOCKET_READ);
    wi_socket_set_port(server_socket, 4871);
    
    result = wi_socket_listen(server_socket);
    
    WI_TEST_ASSERT_TRUE(result, "");
    
    wi_condition_lock_unlock_with_condition(_wi_test_socket_condition_lock, 1);
    
    client_socket = wi_socket_accept(server_socket, 5.0, &client_address);
    
    WI_TEST_ASSERT_NOT_NULL(client_socket, "");
    WI_TEST_ASSERT_NOT_NULL(client_address, "");
    
    rsa = wi_autorelease(wi_rsa_init_with_bits(wi_rsa_alloc(), 512));
    x509 = wi_autorelease(wi_x509_init_with_common_name(wi_x509_alloc(), rsa, WI_STR("helloworldserver")));
    dh = wi_autorelease(wi_dh_init_with_bits(wi_dh_alloc(), 64));
    
    wi_socket_set_tls_certificate(client_socket, x509);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_certificate(client_socket), x509, "");
    
    wi_socket_set_tls_private_key(client_socket, rsa);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_private_key(client_socket), rsa, "");
    
    wi_socket_set_tls_dh(client_socket, dh);

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_dh(client_socket), dh, "");
    
    wi_socket_set_tls_ciphers(client_socket, WI_STR("ALL"));

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_socket_tls_ciphers(client_socket), WI_STR("ALL"), "");

    result = wi_socket_accept_tls(client_socket, 5.0);
    
    WI_TEST_ASSERT_TRUE(result, "%m");
    
    wi_socket_write_data(client_socket, 5.0, wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")));
    
    wi_release(pool);
}

#endif
