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
#include "test.h"

WI_TEST_EXPORT void                     wi_test_date_time_intervals(void);
WI_TEST_EXPORT void                     wi_test_date_creation(void);
WI_TEST_EXPORT void                     wi_test_date_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_date_comparison(void);
WI_TEST_EXPORT void                     wi_test_date_accessors(void);
WI_TEST_EXPORT void                     wi_test_date_mutation(void);


void wi_test_date_time_intervals(void) {
    wi_time_interval_t  interval;
    
    interval = wi_time_interval();
    
    WI_TEST_ASSERT_TRUE(interval > 0.0, "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_string(1), WI_STR("00:01 seconds"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_string(60 + 1), WI_STR("01:01 minutes"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_string(3600 + 60 + 1), WI_STR("01:01:01 hours"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_string(86400 + 3600 + 60 + 1), WI_STR("1:01:01:01 days"), "");

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_string_with_format(86400 + 3600 + 60 + 1, WI_STR(("%Y-%m-%d %H:%M:%S"))), WI_STR("1970-01-02 02:01:01"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_string_with_format(86400 + 3600 + 60 + 1, WI_STR(("%Y-%m-%d %H:%M:%S %Z"))), WI_STR("1970-01-02 01:01:01 UTC"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_time_interval_rfc3339_string(86400 + 3600 + 60 + 1), WI_STR("1970-01-02T02:01:01+01:00"), "");
}



void wi_test_date_creation(void) {
    wi_date_t   *date;
    
    date = wi_date();
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_date_with_time_interval(86400 + 3600 + 60 + 1);
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_date_with_time(86400 + 3600 + 60 + 1);
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_date_with_rfc3339_string(WI_STR("1970-01-02T02:01:01+01:00"));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_autorelease(wi_date_init_with_tv(wi_date_alloc(), wi_dtotv(86400 + 3600 + 60 + 1)));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_autorelease(wi_date_init_with_ts(wi_date_alloc(), wi_dtots(86400 + 3600 + 60 + 1)));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_autorelease(wi_date_init_with_string(wi_date_alloc(), WI_STR("foo"), WI_STR("bar")));
    
    WI_TEST_ASSERT_NULL(date, "");

    date = wi_autorelease(wi_date_init_with_string(wi_date_alloc(), WI_STR("1970-01-01 00:00:00 +0100"), WI_STR("%Y-%m-%d %H:%M:%S %z")));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_autorelease(wi_date_init_with_string(wi_date_alloc(), WI_STR("1970-01-01 00:00:00 -0100"), WI_STR("%Y-%m-%d %H:%M:%S %z")));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");

    date = wi_autorelease(wi_date_init_with_rfc3339_string(wi_date_alloc(), WI_STR("foo")));
    
    WI_TEST_ASSERT_NULL(date, "");
    
    date = wi_autorelease(wi_date_init_with_rfc3339_string(wi_date_alloc(), WI_STR("1970-01-01T00:00:00+01:00")));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
    
    date = wi_autorelease(wi_date_init_with_rfc3339_string(wi_date_alloc(), WI_STR("1970-01-01T00:00:00Z")));
    
    WI_TEST_ASSERT_NOT_NULL(date, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(date), wi_date_runtime_id(), "");
}



void wi_test_date_runtime_functions(void) {
    wi_date_t           *date1;
    wi_mutable_date_t   *date2;
    
    date1 = wi_date_with_time_interval(0.0);
    date2 = wi_autorelease(wi_mutable_copy(date1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(date1, date2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(date1), wi_hash(date2), "");
    
    wi_mutable_date_add_time_interval(date2, 60);

    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(date1, date2, "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(date1), WI_STR("1970"), 0), WI_NOT_FOUND, "");
}



void wi_test_date_comparison(void) {
    wi_date_t           *date1;
    wi_mutable_date_t   *date2;
    
    date1 = wi_date_with_time_interval(0.0);
    date2 = wi_autorelease(wi_mutable_copy(date1));
    
    WI_TEST_ASSERT_EQUALS(wi_date_compare(date1, date2), 0, "");

    wi_mutable_date_add_time_interval(date2, 60);
    
    WI_TEST_ASSERT_EQUALS(wi_date_compare(date1, date2), -1, "");
    
    wi_mutable_date_add_time_interval(date2, -60);
    
    WI_TEST_ASSERT_EQUALS(wi_date_compare(date1, date2), 0, "");
    
    wi_mutable_date_add_time_interval(date2, -60);
    
    WI_TEST_ASSERT_EQUALS(wi_date_compare(date1, date2), 1, "");
}



void wi_test_date_accessors(void) {
    wi_date_t   *date;
    
    date = wi_date_with_time_interval(0.0);
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_date_time_interval(date), 0.0, 0.001, "");
    
    WI_TEST_ASSERT_TRUE(wi_date_time_interval_since_now(date) > 0.0, "");
    WI_TEST_ASSERT_TRUE(wi_date_time_interval_since_date(date, wi_date()) > 0.0, "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_date_string_with_format(date, WI_STR("%Y-%m-%d %H:%M:%S")), WI_STR("1970-01-01 01:00:00"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_date_rfc3339_string(date), WI_STR("1970-01-01T01:00:00+01:00"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_date_time_interval_string(date), WI_STR("00:00 seconds"), "");
}



void wi_test_date_mutation(void) {
    wi_mutable_date_t   *date;
    
    date = wi_autorelease(wi_date_init_with_time_interval(wi_mutable_date_alloc(), 0.0));
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_date_time_interval(date), 0.0, 0.001, "");
    
    wi_mutable_date_add_time_interval(date, 60);
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_date_time_interval(date), 60.0, 0.001, "");
    
    wi_mutable_date_add_time_interval(date, -60);
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_date_time_interval(date), 0.0, 0.001, "");
    
    wi_mutable_date_set_time_interval(date, 60);
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_date_time_interval(date), 60.0, 0.001, "");
    
    wi_mutable_date_set_time_interval(date, -60);
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_date_time_interval(date), -60.0, 0.001, "");
}
