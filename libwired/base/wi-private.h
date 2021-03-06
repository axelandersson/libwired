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

#ifndef WI_PRIVATE_H
#define WI_PRIVATE_H 1

#include <sys/types.h>
#include <regex.h>

#include <wired/wi-array.h>
#include <wired/wi-assert.h>
#include <wired/wi-base.h>
#include <wired/wi-data.h>
#include <wired/wi-dictionary.h>
#include <wired/wi-directory-enumerator.h>
#include <wired/wi-enumerator.h>
#include <wired/wi-error.h>
#include <wired/wi-set.h>
#include <wired/wi-thread.h>

#define WI_RUNTIME_MAGIC                    0xAC1DFEED

#define WI_RUNTIME_BASE(instance)                                           \
    ((wi_runtime_base_t *) instance)

#define WI_RUNTIME_ASSERT_MUTABLE(instance)                                 \
    WI_ASSERT(wi_runtime_options((instance)) & WI_RUNTIME_OPTION_MUTABLE,   \
        "%@ is not mutable", (instance))


struct _wi_enumerator_context {
    wi_uinteger_t                           index;
    void                                    *bucket;
};
typedef struct _wi_enumerator_context       wi_enumerator_context_t;


typedef wi_boolean_t                        wi_enumerator_func_t(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);


WI_EXPORT void                              wi_address_register(void);
WI_EXPORT void                              wi_array_register(void);
WI_EXPORT void                              wi_cipher_register(void);
WI_EXPORT void                              wi_condition_lock_register(void);
WI_EXPORT void                              wi_data_register(void);
WI_EXPORT void                              wi_date_register(void);
WI_EXPORT void                              wi_dh_register(void);
WI_EXPORT void                              wi_dictionary_register(void);
WI_EXPORT void                              wi_directory_enumerator_register(void);
WI_EXPORT void                              wi_dsa_register(void);
WI_EXPORT void                              wi_enumerator_register(void);
WI_EXPORT void                              wi_error_register(void);
WI_EXPORT void                              wi_file_register(void);
WI_EXPORT void                              wi_filesystem_events_register(void);
WI_EXPORT void                              wi_host_register(void);
WI_EXPORT void                              wi_indexset_register(void);
WI_EXPORT void                              wi_lock_register(void);
WI_EXPORT void                              wi_log_register(void);
WI_EXPORT void                              wi_md5_register(void);
WI_EXPORT void                              wi_null_register(void);
WI_EXPORT void                              wi_number_register(void);
WI_EXPORT void                              wi_pipe_register(void);
WI_EXPORT void                              wi_pool_register(void);
WI_EXPORT void                              wi_process_register(void);
WI_EXPORT void                              wi_random_register(void);
WI_EXPORT void                              wi_readwrite_lock_register(void);
WI_EXPORT void                              wi_recursive_lock_register(void);
WI_EXPORT void                              wi_regexp_register(void);
WI_EXPORT void                              wi_rsa_register(void);
WI_EXPORT void                              wi_runtime_register(void);
WI_EXPORT void                              wi_set_register(void);
WI_EXPORT void                              wi_sha1_register(void);
WI_EXPORT void                              wi_sha2_register(void);
WI_EXPORT void                              wi_socket_register(void);
WI_EXPORT void                              wi_string_register(void);
WI_EXPORT void                              wi_string_encoding_register(void);
WI_EXPORT void                              wi_task_register(void);
WI_EXPORT void                              wi_test_register(void);
WI_EXPORT void                              wi_timer_register(void);
WI_EXPORT void                              wi_thread_register(void);
WI_EXPORT void                              wi_url_register(void);
WI_EXPORT void                              wi_uuid_register(void);
WI_EXPORT void                              wi_version_register(void);
WI_EXPORT void                              wi_x509_register(void);
WI_EXPORT void                              wi_xml_node_register(void);

WI_EXPORT void                              wi_address_initialize(void);
WI_EXPORT void                              wi_array_initialize(void);
WI_EXPORT void                              wi_cipher_initialize(void);
WI_EXPORT void                              wi_condition_lock_initialize(void);
WI_EXPORT void                              wi_data_initialize(void);
WI_EXPORT void                              wi_date_initialize(void);
WI_EXPORT void                              wi_dh_initialize(void);
WI_EXPORT void                              wi_dictionary_initialize(void);
WI_EXPORT void                              wi_directory_enumerator_initialize(void);
WI_EXPORT void                              wi_dsa_initialize(void);
WI_EXPORT void                              wi_enumerator_initialize(void);
WI_EXPORT void                              wi_error_initialize(void);
WI_EXPORT void                              wi_file_initialize(void);
WI_EXPORT void                              wi_filesystem_events_initialize(void);
WI_EXPORT void                              wi_host_initialize(void);
WI_EXPORT void                              wi_indexset_initialize(void);
WI_EXPORT void                              wi_lock_initialize(void);
WI_EXPORT void                              wi_log_initialize(void);
WI_EXPORT void                              wi_md5_initialize(void);
WI_EXPORT void                              wi_null_initialize(void);
WI_EXPORT void                              wi_number_initialize(void);
WI_EXPORT void                              wi_pipe_initialize(void);
WI_EXPORT void                              wi_pool_initialize(void);
WI_EXPORT void                              wi_process_initialize(void);
WI_EXPORT void                              wi_random_initialize(void);
WI_EXPORT void                              wi_readwrite_lock_initialize(void);
WI_EXPORT void                              wi_recursive_lock_initialize(void);
WI_EXPORT void                              wi_regexp_initialize(void);
WI_EXPORT void                              wi_rsa_initialize(void);
WI_EXPORT void                              wi_runtime_initialize(void);
WI_EXPORT void                              wi_set_initialize(void);
WI_EXPORT void                              wi_sha1_initialize(void);
WI_EXPORT void                              wi_sha2_initialize(void);
WI_EXPORT void                              wi_socket_initialize(void);
WI_EXPORT void                              wi_string_initialize(void);
WI_EXPORT void                              wi_string_encoding_initialize(void);
WI_EXPORT void                              wi_task_initialize(void);
WI_EXPORT void                              wi_test_initialize(void);
WI_EXPORT void                              wi_timer_initialize(void);
WI_EXPORT void                              wi_thread_initialize(void);
WI_EXPORT void                              wi_url_initialize(void);
WI_EXPORT void                              wi_uuid_initialize(void);
WI_EXPORT void                              wi_version_initialize(void);
WI_EXPORT void                              wi_x509_initialize(void);
WI_EXPORT void                              wi_xml_node_initialize(void);


WI_EXPORT void                              wi_process_load(int, const char **);

WI_EXPORT wi_hash_code_t                    wi_hash_utf8_string(const char *);
WI_EXPORT wi_hash_code_t                    wi_hash_pointer(const void *);
WI_EXPORT wi_hash_code_t                    wi_hash_int(int);
WI_EXPORT wi_hash_code_t                    wi_hash_double(double);
WI_EXPORT wi_hash_code_t                    wi_hash_data(const unsigned char *, wi_uinteger_t);

WI_EXPORT wi_array_callbacks_t              wi_array_callbacks(wi_array_t *);
WI_EXPORT wi_dictionary_key_callbacks_t     wi_dictionary_key_callbacks(wi_dictionary_t *);
WI_EXPORT wi_dictionary_value_callbacks_t   wi_dictionary_value_callbacks(wi_dictionary_t *);
WI_EXPORT wi_set_callbacks_t                wi_set_callbacks(wi_set_t *);

#ifdef HAVE_OPENSSL_SSL_H
WI_EXPORT void *                            wi_dh_openssl_dh(wi_dh_t *);
#endif

WI_EXPORT wi_directory_enumerator_t *       wi_directory_enumerator_alloc(void);
WI_EXPORT wi_directory_enumerator_t *       wi_directory_enumerator_init_with_path(wi_directory_enumerator_t *, wi_string_t *);

#ifdef HAVE_OPENSSL_SSL_H
WI_EXPORT void *                            wi_dsa_openssl_dsa(wi_dsa_t *);
#endif

WI_EXPORT wi_enumerator_t *                 wi_enumerator_alloc(void);
WI_EXPORT wi_enumerator_t *                 wi_enumerator_init_with_collection(wi_enumerator_t *, wi_runtime_instance_t *, wi_enumerator_func_t *);

WI_EXPORT wi_boolean_t                      wi_enumerator_array_data_enumerator(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);
WI_EXPORT wi_boolean_t                      wi_enumerator_array_reverse_data_enumerator(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);
WI_EXPORT wi_boolean_t                      wi_enumerator_dictionary_key_enumerator(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);
WI_EXPORT wi_boolean_t                      wi_enumerator_dictionary_data_enumerator(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);
WI_EXPORT wi_boolean_t                      wi_enumerator_indexset_index_enumerator(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);
WI_EXPORT wi_boolean_t                      wi_enumerator_set_data_enumerator(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);

WI_EXPORT void                              wi_error_enter_thread(void);
WI_EXPORT void                              wi_error_set_error(wi_error_domain_t, int);
WI_EXPORT void                              wi_error_set_error_with_string(wi_error_domain_t, int, wi_string_t *);
WI_EXPORT void                              wi_error_set_errno(int);

#ifdef HAVE_OPENSSL_SHA_H
WI_EXPORT void                              wi_error_set_openssl_error(void);
#endif

#ifdef HAVE_OPENSSL_SSL_H
WI_EXPORT void                              wi_error_set_openssl_ssl_error_with_result(void *, int);
#endif

#ifdef HAVE_COMMONCRYPTO_COMMONCRYPTOR_H
WI_EXPORT void                              wi_error_set_commoncrypto_error(int);
#endif

#ifdef HAVE_LIBXML_PARSER_H
WI_EXPORT void                              wi_error_set_libxml2_error(void);
#endif

WI_EXPORT void                              wi_error_set_regex_error(regex_t *, int);

WI_EXPORT void                              wi_error_set_libwired_error(int);
WI_EXPORT void                              wi_error_set_libwired_error_with_string(int, wi_string_t *);
WI_EXPORT void                              wi_error_set_libwired_error_with_format(int, wi_string_t *, ...);

#ifdef HAVE_OPENSSL_SSL_H
WI_EXPORT void *                            wi_rsa_openssl_rsa(wi_rsa_t *);
#endif

WI_EXPORT void                              wi_runtime_make_immutable(wi_runtime_instance_t *);

WI_EXPORT wi_string_t *                     wi_string_encoding_utf8_string_from_data(wi_string_encoding_t *, wi_data_t *);
WI_EXPORT wi_string_t *                     wi_string_encoding_utf8_string_from_bytes(wi_string_encoding_t *, const char *, wi_uinteger_t);
WI_EXPORT wi_data_t *                       wi_string_encoding_data_from_utf8_bytes(wi_string_encoding_t *, const char *, wi_uinteger_t);

WI_EXPORT void                              wi_socket_exit_thread(void);

WI_EXPORT void                              wi_thread_set_poolstack(wi_thread_t *, void *);
WI_EXPORT void *                            wi_thread_poolstack(wi_thread_t *);

#ifdef HAVE_OPENSSL_SSL_H
WI_EXPORT wi_x509_t *                       wi_x509_init_with_openssl_x509(wi_x509_t *, void *);
WI_EXPORT void *                            wi_x509_openssl_x509(wi_x509_t *);
#endif

#endif /* WI_PRIVATE_H */
