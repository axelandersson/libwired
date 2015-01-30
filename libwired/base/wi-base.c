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

#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#include <wired/wi-base.h>
#include <wired/wi-macros.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>

#define _WI_ELF_STEP(byte, hash)                \
    WI_STMT_START                               \
        (hash) = ((hash) << 4) + (byte);        \
        (hash) ^= ((hash) >> 24) & 0xF0;        \
    WI_STMT_END


wi_string_t                    *wi_root_path = NULL;



void wi_initialize(void) {
    wi_runtime_register();

    wi_address_register();
    wi_array_register();
    
#ifdef WI_CIPHERS
    wi_cipher_register();
#endif
    
#ifdef WI_PTHREADS
    wi_condition_lock_register();
#endif
    
    wi_data_register();
    wi_date_register();
    
#ifdef WI_DH
    wi_dh_register();
#endif
    
    wi_dictionary_register();
    wi_digest_register();
    wi_directory_enumerator_register();
    wi_enumerator_register();
    wi_error_register();
    wi_file_register();
    
#ifdef WI_FILESYSTEM_EVENTS
    wi_filesystem_events_register();
#endif
    
    wi_host_register();
    wi_indexset_register();
    
#ifdef WI_PTHREADS
    wi_lock_register();
#endif
    
    wi_log_register();
    wi_null_register();
    wi_number_register();
    wi_pipe_register();
    wi_pool_register();
    wi_process_register();
    wi_random_register();

#ifdef WI_PTHREADS
    wi_readwrite_lock_register();
    wi_recursive_lock_register();
#endif
    
    wi_regexp_register();
    
#ifdef WI_RSA
    wi_rsa_register();
#endif
    
    wi_set_register();
    wi_socket_register();
    
#ifdef WI_SSL
    wi_socket_tls_register();
#endif
    
    wi_string_register();
    
#ifdef WI_STRING_ENCODING
    wi_string_encoding_register();
#endif
    
    wi_task_register();
    wi_test_register();
    wi_thread_register();

#if WI_PTHREADS
    wi_timer_register();
#endif

    wi_url_register();
    wi_uuid_register();
    wi_version_register();
    
#ifdef WI_X509
    wi_x509_register();
#endif
    
#ifdef WI_XML
    wi_xml_node_register();
#endif

#ifdef WI_PTHREADS
    wi_condition_lock_initialize();
    wi_lock_initialize();
    wi_readwrite_lock_initialize();
    wi_recursive_lock_initialize();
#endif
    
    wi_runtime_initialize();
    wi_array_initialize();
    wi_dictionary_initialize();
    wi_set_initialize();
    wi_string_initialize();

    wi_address_initialize();

#ifdef WI_CIPHERS
    wi_cipher_initialize();
#endif
    
    wi_data_initialize();
    wi_date_initialize();

#ifdef WI_DH
    wi_dh_initialize();
#endif
    
    wi_digest_initialize();
    wi_directory_enumerator_initialize();
    wi_enumerator_initialize();
    wi_error_initialize();
    wi_file_initialize();
    
#ifdef WI_FILESYSTEM_EVENTS
    wi_filesystem_events_initialize();
#endif
    
    wi_host_initialize();
    wi_indexset_initialize();
    wi_log_initialize();
    wi_null_initialize();
    wi_number_initialize();
    wi_pipe_initialize();
    wi_pool_initialize();
    wi_process_initialize();
    wi_random_initialize();
    wi_regexp_initialize();
    
#ifdef WI_RSA
    wi_rsa_initialize();
#endif
    
#ifdef WI_STRING_ENCODING
    wi_string_encoding_initialize();
#endif
    
    wi_socket_initialize();
    
#ifdef WI_SSL
    wi_socket_tls_initialize();
#endif
    
    wi_task_initialize();
    wi_test_initialize();
    wi_thread_initialize();

#if WI_PTHREADS
    wi_timer_initialize();
#endif

    wi_url_initialize();
    wi_uuid_initialize();
    wi_version_initialize();

#ifdef WI_X509
    wi_x509_initialize();
#endif
    
#ifdef WI_XML
    wi_xml_node_initialize();
#endif
}



void wi_load(int argc, const char **argv) {
    wi_pool_t        *pool;
    
    pool = wi_pool_init(wi_pool_alloc());
    
    wi_process_load(argc, argv);
    
    wi_release(pool);
}



#pragma mark -

void wi_abort(void) {
    abort();
}



void wi_crash(void) {
    *((volatile char *) NULL) = 0;
}



#pragma mark -

wi_hash_code_t wi_hash_utf8_string(const char *s) {
    wi_uinteger_t   length;
    wi_hash_code_t  hash = length;
    const char      *end, *end4;
    
    length = strlen(s);
    hash = length;

    if(length < 16) {
        end = s + length;
        end4 = s + (length & ~3);
        
        while(s < end4) {
            hash = (hash * 67503105) + (s[0] * 16974593) + (s[1] * 66049) + (s[2] * 257) + s[3];
            s += 4;
        }
        
        while(s < end)
            hash = (hash * 257) + *s++;
    } else {
        hash = (hash * 67503105) + (s[0] * 16974593) + (s[1] * 66049) + (s[2] * 257) + s[3];
        hash = (hash * 67503105) + (s[4] * 16974593) + (s[5] * 66049) + (s[6] * 257) + s[7];
        s += length - 8;
        hash = (hash * 67503105) + (s[0] * 16974593) + (s[1] * 66049) + (s[2] * 257) + s[3];
        hash = (hash * 67503105) + (s[4] * 16974593) + (s[5] * 66049) + (s[6] * 257) + s[7];
    }

    return hash + (hash << (length & 31));
}



wi_hash_code_t wi_hash_pointer(const void *p) {
#ifdef WI_32
    return (wi_hash_code_t) ((((uint32_t) p) >> 16) ^ ((uint32_t) p));
#else
    return (wi_hash_code_t) ((((uint64_t) p) >> 32) ^ ((uint64_t) p));
#endif
}



wi_hash_code_t wi_hash_int(int32_t i) {
    return (wi_hash_code_t) WI_ABS(i);
}



wi_hash_code_t wi_hash_double(double d) {
    double        i;

    i = rint(WI_ABS(d));

    return (wi_hash_code_t) fmod(i, (double) 0xFFFFFFFF) + ((d - i) * 0xFFFFFFFF);
}



wi_hash_code_t wi_hash_data(const unsigned char *bytes, wi_uinteger_t length) {
    wi_hash_code_t    hash = 0;
    wi_uinteger_t    i;
    
    i = length;
    
    while(i > 3) {
        _WI_ELF_STEP(bytes[length - i    ], hash);
        _WI_ELF_STEP(bytes[length - i + 1], hash);
        _WI_ELF_STEP(bytes[length - i + 2], hash);
        _WI_ELF_STEP(bytes[length - i + 3], hash);
        i -= 4;
    }

    switch(i) {
        case 3: _WI_ELF_STEP(bytes[length - 3], hash);
        case 2: _WI_ELF_STEP(bytes[length - 2], hash);
        case 1: _WI_ELF_STEP(bytes[length - 1], hash);
    }
    
    return hash;
}
