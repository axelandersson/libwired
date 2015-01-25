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

WI_TEST_EXPORT void                     wi_test_array_creation(void);
WI_TEST_EXPORT void                     wi_test_array_serialization(void);
WI_TEST_EXPORT void                     wi_test_array_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_array_instances(void);
WI_TEST_EXPORT void                     wi_test_array_scalars(void);
WI_TEST_EXPORT void                     wi_test_array_enumeration(void);
WI_TEST_EXPORT void                     wi_test_array_mutation(void);


void wi_test_array_creation(void) {
    wi_array_t      *array;
    wi_string_t     *strings[2];
    
    array = wi_array();

    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 0U, "");
    
    array = wi_mutable_array();
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 0U, "");
    
    array = wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL);

    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("bar"), "");
    
    array = wi_autorelease(wi_array_init_with_data(wi_array_alloc(), WI_STR("foo"), WI_STR("bar"), NULL));
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("bar"), "");
    
    strings[0] = WI_STR("foo");
    strings[1] = WI_STR("bar");
    array = wi_autorelease(wi_array_init_with_data_and_count(wi_array_alloc(), (void **) strings, 2));
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("bar"), "");
}



void wi_test_array_serialization(void) {
#ifdef WI_PLIST
    wi_array_t      *array1, *array2;
    wi_string_t     *path;
    
    array1 = wi_array_with_plist_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-array-tests-1.plist")));
    
    WI_TEST_ASSERT_NOT_NULL(array1, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(array1), wi_array_runtime_id(), "");
    
    array1 = wi_array_with_plist_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-array-tests-2.plist")));
    
    WI_TEST_ASSERT_NULL(array1, "");
    
    array1 = wi_array_with_plist_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-array-tests-3.plist")));
    
    WI_TEST_ASSERT_NULL(array1, "");

    array1 = wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    path = wi_fs_temporary_path_with_template(WI_STR("/tmp/libwired-test-plist.XXXXXXX"));
    
    wi_array_write_to_path(array1, path);

    array2 = wi_array_with_plist_file(path);
    
    wi_fs_delete_path(path);

    WI_TEST_ASSERT_EQUAL_INSTANCES(array1, array2, "");
#endif
}



void wi_test_array_runtime_functions(void) {
    wi_array_t          *array1;
    wi_mutable_array_t  *array2;

    array1 = wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    array2 = wi_autorelease(wi_mutable_copy(array1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(array1, array2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(array1), wi_hash(array2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(array1), wi_array_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(array2), wi_array_runtime_id(), "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(array1) & WI_RUNTIME_OPTION_IMMUTABLE, "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(array2) & WI_RUNTIME_OPTION_MUTABLE, "");
    
    wi_mutable_array_remove_data(array2, WI_STR("bar"));
    wi_mutable_array_add_data(array2, WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(array1, array2, "");
    
    wi_mutable_array_remove_data(array2, WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(array1, array2, "");
    
    array2 = wi_autorelease(wi_array_init_with_capacity_and_callbacks(wi_mutable_array_alloc(), 0, wi_array_null_callbacks));
    
    wi_mutable_array_add_data(array2, (void *) 1);
    wi_mutable_array_add_data(array2, (void *) 2);
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(array1, array2, "");

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(array1), WI_STR("foo"), 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(array2), WI_STR("0x1"), 0), WI_NOT_FOUND, "");
}



void wi_test_array_instances(void) {
    wi_array_t      *array, *subarray;
    wi_string_t     *string, *strings[5];
    
    array = wi_array();
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 0U, "");
    WI_TEST_ASSERT_FALSE(wi_array_contains_data(array, WI_STR("foo")), "");
    WI_TEST_ASSERT_FALSE(wi_array_contains_data(array, WI_STR("bar")), "");
    WI_TEST_ASSERT_NULL(wi_array_first_data(array), "");
    WI_TEST_ASSERT_NULL(wi_array_last_data(array), "");

    array = wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("foo")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, WI_STR("foo")), 0U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("bar")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, WI_STR("bar")), 1U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_first_data(array), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(array), WI_STR("bar"), "");
    
    subarray = wi_array_subarray_with_range(array, wi_make_range(0, 1));
    
    WI_TEST_ASSERT_NOT_NULL(subarray, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(subarray), 1U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(subarray, WI_STR("foo")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(subarray, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(subarray, WI_STR("foo")), 0U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_first_data(subarray), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(subarray), WI_STR("foo"), "");
    
    subarray = wi_array_data_at_indexes(array, wi_indexset_with_index(0));
    
    WI_TEST_ASSERT_NOT_NULL(subarray, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(subarray), 1U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(subarray, WI_STR("foo")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(subarray, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(subarray, WI_STR("foo")), 0U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_first_data(subarray), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(subarray), WI_STR("foo"), "");
    
    string = wi_array_components_joined_by_string(array, WI_STR(","));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("foo,bar"), "");
    
    array = wi_array_by_sorting(array, wi_string_compare);

    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("bar")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, WI_STR("bar")), 0U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("foo")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, WI_STR("foo")), 1U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_first_data(array), WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(array), WI_STR("foo"), "");
    
    array = wi_array_by_adding_data(array, WI_STR("baz"));
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 3U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("baz")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 2), WI_STR("baz"), "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, WI_STR("baz")), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(array), WI_STR("baz"), "");
    
    array = wi_array_by_adding_data_from_array(array, wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL));
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 5U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 3), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 4), WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(array), WI_STR("bar"), "");
    
    strings[0] = strings[1] = strings[2] = strings[3] = strings[4] = NULL;
    
    wi_array_get_data(array, (void *) strings);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[0], WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[1], WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[2], WI_STR("baz"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[3], WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[4], WI_STR("bar"), "");
    
    strings[0] = strings[1] = strings[2] = strings[3] = strings[4] = NULL;
    
    wi_array_get_data_in_range(array, (void *) strings, wi_make_range(0, 1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[0], WI_STR("bar"), "");
    
    strings[0] = strings[1] = strings[2] = strings[3] = strings[4] = NULL;
    
    wi_array_get_data_in_range(array, (void *) strings, wi_make_range(1, 1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[0], WI_STR("foo"), "");
    
    strings[0] = strings[1] = strings[2] = strings[3] = strings[4] = NULL;
    
    wi_array_get_data_in_range(array, (void *) strings, wi_make_range(0, 2));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[0], WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(strings[1], WI_STR("foo"), "");
}



void wi_test_array_scalars(void) {
    wi_mutable_array_t  *array;
    wi_array_t          *subarray;
    wi_uinteger_t       integers[3];
    wi_string_t         *string;
    
    array = wi_autorelease(wi_array_init_with_capacity_and_callbacks(wi_mutable_array_alloc(), 0, wi_array_null_callbacks));
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 0U, "");
    WI_TEST_ASSERT_FALSE(wi_array_contains_data(array, (void *) 1), "");
    WI_TEST_ASSERT_FALSE(wi_array_contains_data(array, (void *) 2), "");
    WI_TEST_ASSERT_NULL(wi_array_first_data(array), "");
    WI_TEST_ASSERT_NULL(wi_array_last_data(array), "");
    
    array = wi_autorelease(wi_array_init_with_capacity_and_callbacks(wi_mutable_array_alloc(), 0, wi_array_null_callbacks));
    
    wi_mutable_array_add_data(array, (void *) 1);
    wi_mutable_array_add_data(array, (void *) 2);
    
    WI_TEST_ASSERT_NOT_NULL(array, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, (void *) 1), "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(array, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, (void *) 1), 0U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, (void *) 2), "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(array, 1), (void *) 2, "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, (void *) 2), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_array_first_data(array), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_last_data(array), (void *) 2, "");
    
    subarray = wi_array_subarray_with_range(array, wi_make_range(0, 1));
    
    WI_TEST_ASSERT_NOT_NULL(subarray, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(subarray), 1U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(subarray, (void *) 1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(subarray, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(subarray, (void *) 1), 0U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_first_data(subarray), (void *) 1, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(subarray), (void *) 1, "");
    
    subarray = wi_array_data_at_indexes(array, wi_indexset_with_index(0));
    
    WI_TEST_ASSERT_NOT_NULL(subarray, "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(subarray), 1U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(subarray, (void *) 1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(subarray, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(subarray, (void *) 1), 0U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_first_data(subarray), (void *) 1, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(subarray), (void *) 1, "");
    
    string = wi_array_components_joined_by_string(array, WI_STR(","));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("0x1,0x2"), "");
    
    array = wi_array_by_adding_data(array, (void *) 3);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 3U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, (void *) 3), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 2), (void *) 3, "");
    WI_TEST_ASSERT_EQUALS(wi_array_index_of_data(array, (void *) 3), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_last_data(array), (void *) 3, "");
    
    integers[0] = integers[1] = integers[2] = 0;
    
    wi_array_get_data(array, (void *) integers);
    
    WI_TEST_ASSERT_EQUALS(integers[0], 1U, "");
    WI_TEST_ASSERT_EQUALS(integers[1], 2U, "");
    WI_TEST_ASSERT_EQUALS(integers[2], 3U, "");
    
    integers[0] = integers[1] = integers[2] = 0;
    
    wi_array_get_data_in_range(array, (void *) integers, wi_make_range(0, 1));
    
    WI_TEST_ASSERT_EQUALS(integers[0], 1U, "");
    
    integers[0] = integers[1] = integers[2] = 0;
    
    wi_array_get_data_in_range(array, (void *) integers, wi_make_range(1, 1));
    
    WI_TEST_ASSERT_EQUALS(integers[0], 2U, "");
    
    integers[0] = integers[1] = integers[2] = 0;
    
    wi_array_get_data_in_range(array, (void *) integers, wi_make_range(0, 2));
    
    WI_TEST_ASSERT_EQUALS(integers[0], 1U, "");
    WI_TEST_ASSERT_EQUALS(integers[1], 2U, "");
}



void wi_test_array_enumeration(void) {
    wi_mutable_array_t  *array;
    wi_enumerator_t     *enumerator;
    wi_number_t         *number;
    wi_uinteger_t       i;
    
    array = wi_mutable_array();
    
    for(i = 5000; i <= 10000; i++)
        wi_mutable_array_add_data(array, WI_INT32(i));
    
    for(i = 1; i < 5000; i++)
        wi_mutable_array_insert_data_at_index(array, WI_INT32(i), i - 1);
    
    enumerator = wi_array_data_enumerator(array);
    i = 1;
    
    while((number = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_EQUAL_INSTANCES(number, WI_INT32(i), "");
        
        i++;
    }

    WI_TEST_ASSERT_EQUALS(i, 10001, "");

    enumerator = wi_array_reverse_data_enumerator(array);
    i = 10000;
    
    while((number = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_EQUAL_INSTANCES(number, WI_INT32(i), "");
        
        i--;
    }

    WI_TEST_ASSERT_EQUALS(i, 0, "");
    
    for(i = 0; i < 10000; i++)
        wi_mutable_array_remove_data_at_index(array, 0);
}



void wi_test_array_mutation(void) {
    wi_mutable_array_t  *array1, *array2;
    
    array1 = wi_mutable_array();
    
    wi_mutable_array_add_data(array1, WI_STR("foo"));
    wi_mutable_array_add_data(array1, WI_STR("bar"));
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("bar"), "");
    
    wi_mutable_array_replace_data_at_index(array1, WI_STR("foo2"), 0);
    wi_mutable_array_replace_data_at_index(array1, WI_STR("bar2"), 1);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("bar2"), "");
    
    array2 = wi_mutable_copy(array1);
    
    wi_mutable_array_add_data(array2, WI_STR("baz"));
    
    wi_mutable_array_set_array(array1, array2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(array1, array2, "");
    
    wi_mutable_array_add_data_sorted(array1, WI_STR("abc"), wi_string_compare);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 4U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("abc"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 2), WI_STR("bar2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 3), WI_STR("baz"), "");
    
    wi_mutable_array_insert_data_at_index(array1, WI_STR("zzz"), 0);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 5U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("zzz"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("abc"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 2), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 3), WI_STR("bar2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 4), WI_STR("baz"), "");
    
    wi_mutable_array_sort(array1, wi_string_compare);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 5U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("abc"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("bar2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 2), WI_STR("baz"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 3), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 4), WI_STR("zzz"), "");
    
    wi_mutable_array_reverse(array1);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 5U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("zzz"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 2), WI_STR("baz"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 3), WI_STR("bar2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 4), WI_STR("abc"), "");
    
    wi_mutable_array_remove_data_at_index(array1, 0);

    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 4U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("baz"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 2), WI_STR("bar2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 3), WI_STR("abc"), "");
    
    wi_mutable_array_remove_data_in_range(array1, wi_make_range(1, 2));
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("foo2"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("abc"), "");
    
    wi_mutable_array_remove_data_in_array(array1, wi_array_with_data(WI_STR("abc"), NULL));
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 1U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("foo2"), "");

    wi_mutable_array_remove_all_data(array1);

    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 0U, "");
    
    wi_mutable_array_sort(array1, wi_string_compare);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 0U, "");
    
    wi_mutable_array_add_data_sorted(array1, WI_STR("abc"), wi_string_compare);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 1U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("abc"), "");

    wi_mutable_array_add_data_sorted(array1, WI_STR("zzz"), wi_string_compare);

    WI_TEST_ASSERT_EQUALS(wi_array_count(array1), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 0), WI_STR("abc"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array1, 1), WI_STR("zzz"), "");
}
