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

#ifndef WI_BASE_H
#define WI_BASE_H 1

#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <inttypes.h>

#ifdef __cplusplus
#define WI_EXPORT                           extern "C"
#else
#define WI_EXPORT                           extern
#endif

#define WI_INLINE                           inline
#define WI_STATIC_INLINE                    static inline

#ifdef __GNUC__
#define WI_DEPRECATED                       __attribute__ ((deprecated))
#define WI_PRINTF_FORMAT(x, y)              __attribute__ ((format (printf, x, y)))
#define WI_STRFTIME_FORMAT(x)               __attribute__ ((format (strftime, x, 0)))
#else
#define WI_DEPRECATED
#define WI_PRINTF_FORMAT(x, y)
#define WI_STRFTIME_FORMAT(x)
#endif

#if __GNUC__ == 4 || __GNUC__ > 4
#define WI_SENTINEL                         __attribute__ ((sentinel))
#else
#define WI_SENTINEL
#endif

#if __LP64__
#define WI_64                               1

typedef int64_t                             wi_integer_t;
typedef uint64_t                            wi_uinteger_t;

#define WI_INTEGER_MAX                      9223372036854775807LL
#define WI_UINTEGER_MAX                     18446744073709551615ULL
#define WI_INTEGER_MIN                      (-WI_INTEGER_MAX-1)
#else
#define WI_32                               1

typedef int32_t                             wi_integer_t;
typedef uint32_t                            wi_uinteger_t;

#define WI_INTEGER_MAX                      2147483647
#define WI_UINTEGER_MAX                     4294967295U
#define WI_INTEGER_MIN                      (-WI_INTEGER_MAX-1)
#endif


enum {
    WI_NOT_FOUND                            = WI_INTEGER_MAX
};


struct _wi_range {
    wi_uinteger_t                           location;
    wi_uinteger_t                           length;
};
typedef struct _wi_range                    wi_range_t;


WI_STATIC_INLINE wi_range_t wi_make_range(wi_uinteger_t location, wi_uinteger_t length) {
    wi_range_t        range = { location, length };
    
    return range;
}



struct _wi_size {
    wi_uinteger_t                           width;
    wi_uinteger_t                           height;
};
typedef struct _wi_size                     wi_size_t;


WI_STATIC_INLINE wi_size_t wi_make_size(wi_uinteger_t width, wi_uinteger_t height) {
    wi_size_t        size = { width, height };
    
    return size;
}



struct _wi_point {
    wi_uinteger_t                           x;
    wi_uinteger_t                           y;
};
typedef struct _wi_point                    wi_point_t;


WI_STATIC_INLINE wi_point_t wi_make_point(wi_uinteger_t x, wi_uinteger_t y) {
    wi_point_t        point = { x, y };
    
    return point;
}



WI_STATIC_INLINE struct timeval wi_dtotv(double d) {
    struct timeval    tv;

    tv.tv_sec = (time_t) floor(d);
    tv.tv_usec = (suseconds_t) ((d - tv.tv_sec) * 1000000.0);

    return tv;
}



WI_STATIC_INLINE double wi_tvtod(struct timeval tv) {
    return tv.tv_sec + ((double) tv.tv_usec / 1000000.0);
}



WI_STATIC_INLINE struct timespec wi_dtots(double d) {
    struct timespec    ts;

    ts.tv_sec = (time_t) floor(d);
    ts.tv_nsec = (long) ((d - ts.tv_sec) * 1000000000.0);

    return ts;
}



WI_STATIC_INLINE double wi_tstod(struct timespec ts) {
    return ts.tv_sec + ((double) ts.tv_nsec / 1000000000.0);
}



WI_STATIC_INLINE wi_uinteger_t wi_log2(wi_uinteger_t n) {
    return n < 2 ? 0 : wi_log2(n >> 1) + 1;
}



WI_STATIC_INLINE wi_uinteger_t wi_exp2m1(wi_uinteger_t n) {
    return (1 << n) - 1;
}

    
typedef int32_t                             wi_boolean_t;

#ifndef true
#define true                                1
#endif

#ifndef false
#define false                               0
#endif


typedef wi_uinteger_t                       wi_hash_code_t;

typedef double                              wi_time_interval_t;

typedef struct _wi_address                  wi_address_t;
typedef struct _wi_address                  wi_mutable_address_t;
typedef struct _wi_array                    wi_array_t;
typedef struct _wi_array                    wi_mutable_array_t;
typedef struct _wi_cipher                   wi_cipher_t;
typedef struct _wi_condition_lock           wi_condition_lock_t;
typedef struct _wi_data                     wi_data_t;
typedef struct _wi_data                     wi_mutable_data_t;
typedef struct _wi_date                     wi_date_t;
typedef struct _wi_date                     wi_mutable_date_t;
typedef struct _wi_dh                       wi_dh_t;
typedef struct _wi_dictionary               wi_dictionary_t;
typedef struct _wi_dictionary               wi_mutable_dictionary_t;
typedef struct _wi_directory_enumerator     wi_directory_enumerator_t;
typedef struct _wi_enumerator               wi_enumerator_t;
typedef struct _wi_error                    wi_error_t;
typedef struct _wi_file                     wi_file_t;
typedef struct _wi_filesystem_events        wi_filesystem_events_t;
typedef struct _wi_host                     wi_host_t;
typedef struct _wi_indexset                 wi_indexset_t;
typedef struct _wi_indexset                 wi_mutable_indexset_t;
typedef struct _wi_lock                     wi_lock_t;
typedef struct _wi_md5                      wi_md5_t;
typedef struct _wi_null                     wi_null_t;
typedef struct _wi_number                   wi_number_t;
typedef struct _wi_pipe                     wi_pipe_t;
typedef struct _wi_pool                     wi_pool_t;
typedef struct _wi_process                  wi_process_t;
typedef struct _wi_readwrite_lock           wi_readwrite_lock_t;
typedef struct _wi_recursive_lock           wi_recursive_lock_t;
typedef struct _wi_regexp                   wi_regexp_t;
typedef struct _wi_rsa                      wi_rsa_t;
typedef struct _wi_set                      wi_set_t;
typedef struct _wi_set                      wi_mutable_set_t;
typedef struct _wi_sha1                     wi_sha1_t;
typedef struct _wi_socket                   wi_socket_t;
typedef struct _wi_socket_tls               wi_socket_tls_t;
typedef struct _wi_string                   wi_string_t;
typedef struct _wi_string                   wi_mutable_string_t;
typedef struct _wi_string_encoding          wi_string_encoding_t;
typedef struct _wi_task                     wi_task_t;
typedef struct _wi_thread                   wi_thread_t;
typedef struct _wi_timer                    wi_timer_t;
typedef struct _wi_url                      wi_url_t;
typedef struct _wi_url                      wi_mutable_url_t;
typedef struct _wi_uuid                     wi_uuid_t;
typedef struct _wi_x509                     wi_x509_t;
typedef struct _wi_xml_node                 wi_xml_node_t;


WI_EXPORT void                              wi_initialize(void);
WI_EXPORT void                              wi_load(int, const char **);

WI_EXPORT void                              wi_abort(void);
WI_EXPORT void                              wi_crash(void);

#endif /* WI_BASE_H */
