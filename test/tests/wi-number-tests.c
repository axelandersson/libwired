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

WI_TEST_EXPORT void                     wi_test_number_creation(void);
WI_TEST_EXPORT void                     wi_test_number_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_number_accessors(void);
WI_TEST_EXPORT void                     wi_test_number_conversion(void);
WI_TEST_EXPORT void                     wi_test_number_values(void);


void wi_test_number_creation(void) {
    wi_number_t     *number;
    int16_t         i16 = 1;
    int8_t          i8 = 1;
    
    number = wi_number_with_value(WI_NUMBER_INT8, &i8);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_value(WI_NUMBER_INT16, &i16);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_bool(true);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_char('a');
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_short(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_int(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_int32(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_int64(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_integer(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_long(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_long_long(1);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_float(1.0f);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
    
    number = wi_number_with_double(1.0);
    
    WI_TEST_ASSERT_NOT_NULL(number, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(number), wi_number_runtime_id(), "");
}



void wi_test_number_runtime_functions(void) {
    wi_number_t   *number1, *number2;
    
    number1 = wi_number_with_bool(true);
    number2 = wi_autorelease(wi_copy(number1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(number1, number2, "");

    number1 = wi_number_with_int(1);
    number2 = wi_number_with_long(1);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(number1, number2, "");
    
    number1 = wi_number_with_int(1);
    number2 = wi_number_with_double(1.0);

    WI_TEST_ASSERT_EQUAL_INSTANCES(number1, number2, "");
    
    WI_TEST_ASSERT_EQUALS(wi_hash(wi_number_with_char(1)), wi_hash(wi_number_with_short(1)), "");
    WI_TEST_ASSERT_EQUALS(wi_hash(wi_number_with_int32(1)), wi_hash(wi_number_with_int64(1)), "");
    WI_TEST_ASSERT_EQUALS(wi_hash(wi_number_with_float(1.0f)), wi_hash(wi_number_with_double(1.0)), "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(wi_number_with_int(1337)), WI_STR("1337"), 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(wi_number_with_double(42.42)), WI_STR("42.42"), 0), WI_NOT_FOUND, "");
}



void wi_test_number_accessors(void) {
    WI_TEST_ASSERT_EQUALS(wi_number_type(wi_number_with_bool(true)), WI_NUMBER_BOOL, "");

    WI_TEST_ASSERT_EQUALS(wi_number_storage_type(wi_number_with_bool(true)), WI_NUMBER_STORAGE_INT32, "");

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_number_string(wi_number_with_int(1337)), WI_STR("1337"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_number_string(wi_number_with_double(42.42)), WI_STR("42.420000"), "");
}



void wi_test_number_conversion(void) {
    wi_array_t          *array;
    wi_enumerator_t     *enumerator;
    wi_number_t         *number;
    int64_t             i64;
    int32_t             i32;
    int16_t             i16;
    int8_t              i8;
    double              d;
    float               f;
    long long           ll;
    long                l;
    int                 i;
    short               s;
    char                c;
    wi_boolean_t        b;
    
    array = wi_array_with_data(wi_number_with_char(1),
                               wi_number_with_short(1),
                               wi_number_with_int32(1),
                               wi_number_with_int64(1),
                               wi_number_with_float(1.0f),
                               wi_number_with_double(1.0),
                               NULL);
    enumerator = wi_array_data_enumerator(array);
    
    while((number = wi_enumerator_next_data(enumerator))) {
        wi_number_get_value(number, WI_NUMBER_BOOL, &b);

        WI_TEST_ASSERT_EQUALS(b, true, "");
        
        wi_number_get_value(number, WI_NUMBER_CHAR, &c);
        
        WI_TEST_ASSERT_EQUALS(c, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_SHORT, &s);
        
        WI_TEST_ASSERT_EQUALS(s, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_INT, &i);
        
        WI_TEST_ASSERT_EQUALS(i, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_INT8, &i8);
        
        WI_TEST_ASSERT_EQUALS(i8, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_INT16, &i16);
        
        WI_TEST_ASSERT_EQUALS(i16, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_INT32, &i32);
        
        WI_TEST_ASSERT_EQUALS(i32, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_INT64, &i64);
        
        WI_TEST_ASSERT_EQUALS(i64, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_LONG, &l);
        
        WI_TEST_ASSERT_EQUALS(l, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_LONG_LONG, &ll);
        
        WI_TEST_ASSERT_EQUALS(ll, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_FLOAT, &f);
        
        WI_TEST_ASSERT_EQUALS(f, 1, "");
        
        wi_number_get_value(number, WI_NUMBER_DOUBLE, &d);
        
        WI_TEST_ASSERT_EQUALS(d, 1, "");
    }
}



void wi_test_number_values(void) {
    WI_TEST_ASSERT_EQUALS(wi_number_bool(wi_number_with_bool(true)), true, "");
    WI_TEST_ASSERT_EQUALS(wi_number_bool(wi_number_with_bool(false)), false, "");
    WI_TEST_ASSERT_EQUALS(wi_number_char(wi_number_with_char(127)), 127, "");
    WI_TEST_ASSERT_EQUALS(wi_number_short(wi_number_with_short(32767)), 32767, "");
    WI_TEST_ASSERT_EQUALS(wi_number_int32(wi_number_with_int32(2147483647)), 2147483647, "");
    WI_TEST_ASSERT_EQUALS((uint32_t) wi_number_int32(wi_number_with_int32(4294967295U)), 4294967295U, "");
    WI_TEST_ASSERT_EQUALS(wi_number_int64(wi_number_with_int64(9223372036854775807LL)), 9223372036854775807LL, "");
    WI_TEST_ASSERT_EQUALS((uint64_t) wi_number_int64(wi_number_with_int64(18446744073709551615ULL)), 18446744073709551615ULL, "");
    
#ifdef WI_32
    WI_TEST_ASSERT_EQUALS(wi_number_integer(wi_number_with_integer(2147483647)), 2147483647, "");
    WI_TEST_ASSERT_EQUALS((wi_uinteger_t) wi_number_integer(wi_number_with_integer(4294967295U)), 4294967295U, "");
#else
    WI_TEST_ASSERT_EQUALS(wi_number_integer(wi_number_with_integer(9223372036854775807LL)), 9223372036854775807LL, "");
    WI_TEST_ASSERT_EQUALS((wi_uinteger_t) wi_number_integer(wi_number_with_integer(18446744073709551615ULL)), 18446744073709551615ULL, "");
#endif
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_number_float(wi_number_with_float(3.40282346e38)), 3.40282346e38F, 0.0001, "");
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_number_double(wi_number_with_double(1.7976931348623155e308)), 1.7976931348623155e308, 0.0001, "");
}
