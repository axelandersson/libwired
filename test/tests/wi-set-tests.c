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

WI_TEST_EXPORT void						wi_test_set_creation(void);
WI_TEST_EXPORT void						wi_test_set_runtime_functions(void);
WI_TEST_EXPORT void						wi_test_set_instances(void);
WI_TEST_EXPORT void						wi_test_set_scalars(void);
WI_TEST_EXPORT void						wi_test_set_enumeration(void);
WI_TEST_EXPORT void						wi_test_set_mutation(void);


void wi_test_set_creation(void) {
    wi_set_t    *set;
    
    set = wi_set();

    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(set), wi_set_runtime_id(), "");
    
    set = wi_set_with_data(WI_STR("foo"), WI_STR("bar"), NULL);

    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(set), wi_set_runtime_id(), "");
    
    set = wi_autorelease(wi_set_init_with_data(wi_set_alloc(), WI_STR("foo"), WI_STR("bar"), NULL));
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(set), wi_set_runtime_id(), "");
    
    set = wi_autorelease(wi_set_init_with_array(wi_set_alloc(), wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL)));
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(set), wi_set_runtime_id(), "");
}



void wi_test_set_runtime_functions(void) {
    wi_set_t            *set1;
    wi_mutable_set_t    *set2;
    
    set1 = wi_set_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    set2 = wi_autorelease(wi_mutable_copy(set1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(set1, set2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(set1), wi_hash(set2), "");
    
    wi_mutable_set_remove_data(set2, WI_STR("bar"));
    wi_mutable_set_add_data(set2, WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(set1, set2, "");
    
    wi_mutable_set_remove_data(set2, WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(set1, set2, "");

    set2 = wi_autorelease(wi_set_init_with_capacity_and_callbacks(wi_mutable_set_alloc(), 0, false, wi_set_null_callbacks));
    
    wi_mutable_set_add_data(set2, (void *) 1);
    wi_mutable_set_add_data(set2, (void *) 2);
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(set1), WI_STR("foo"), 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(set2), WI_STR("0x1"), 0), WI_NOT_FOUND, "");
}



void wi_test_set_instances(void) {
    wi_set_t    *set;
    wi_array_t  *array;
    
    set = wi_set_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("bar")), "");
    
    array = wi_set_all_data(set);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("bar")), "");
}



void wi_test_set_scalars(void) {
    wi_mutable_set_t    *set;
    wi_array_t          *array;
    
    set = wi_set_init_with_capacity_and_callbacks(wi_mutable_set_alloc(), 0, false, wi_set_null_callbacks);
    
    wi_mutable_set_add_data(set, "foo");
    wi_mutable_set_add_data(set, "bar");
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, "foo"), "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, "bar"), "");
    
    array = wi_set_all_data(set);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, "foo"), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, "bar"), "");
}



void wi_test_set_enumeration(void) {
    wi_mutable_set_t    *set;
    wi_enumerator_t     *enumerator;
    wi_number_t         *number;
    wi_uinteger_t       i;
    
    set = wi_mutable_set();
    
    for(i = 1; i <= 10000; i++)
        wi_mutable_set_add_data(set, WI_INT32(i));
    
    enumerator = wi_set_data_enumerator(set);
    i = 0;
    
    while((number = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_TRUE(wi_number_int32(number) >= 1, "");
        WI_TEST_ASSERT_TRUE(wi_number_int32(number) <= 10000, "");
        
        i++;
    }
    
    WI_TEST_ASSERT_EQUALS(i, 10000, "");
    
    for(i = 1; i <= 10000; i++)
        wi_mutable_set_remove_data(set, WI_INT32(i));
}



void wi_test_set_mutation(void) {
    wi_mutable_set_t     *set1, *set2;
    
    set1 = wi_set_init_with_capacity_and_callbacks(wi_mutable_set_alloc(), 0, true, wi_set_null_callbacks);
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("foo")), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("bar")), 0U, "");
    
    wi_mutable_set_add_data(set1, WI_STR("foo"));
    wi_mutable_set_add_data(set1, WI_STR("bar"));
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 2U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("foo")), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("bar")), 1U, "");
    
    wi_mutable_set_add_data(set1, WI_STR("foo"));
    wi_mutable_set_add_data(set1, WI_STR("bar"));

    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 2U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("foo")), 2U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("bar")), 2U, "");
    
    wi_mutable_set_remove_data(set1, WI_STR("foo"));
    wi_mutable_set_remove_data(set1, WI_STR("bar"));
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 2U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("foo")), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("bar")), 1U, "");
    
    wi_mutable_set_remove_data(set1, WI_STR("foo"));
    wi_mutable_set_remove_data(set1, WI_STR("bar"));
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 0U, "");

    wi_mutable_set_add_data(set1, WI_STR("foo"));
    wi_mutable_set_add_data(set1, WI_STR("bar"));
    
    set2 = wi_mutable_copy(set1);

    wi_mutable_set_add_data(set2, WI_STR("baz"));

    wi_mutable_set_set_set(set1, set2);

    WI_TEST_ASSERT_EQUAL_INSTANCES(set1, set2, "");
    
    wi_mutable_set_remove_all_data(set1);
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 0U, "");
    
    wi_mutable_set_add_data_from_array(set1, wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL));
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set1), 2U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("foo")), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set1, WI_STR("bar")), 1U, "");
}
