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

WI_TEST_EXPORT void						wi_test_dictionary_creation(void);
WI_TEST_EXPORT void						wi_test_dictionary_serialization(void);
WI_TEST_EXPORT void						wi_test_dictionary_runtime_functions(void);
WI_TEST_EXPORT void						wi_test_dictionary_instances(void);
WI_TEST_EXPORT void						wi_test_dictionary_scalars(void);
WI_TEST_EXPORT void						wi_test_dictionary_enumeration(void);
WI_TEST_EXPORT void						wi_test_dictionary_mutation(void);


void wi_test_dictionary_creation(void) {
    wi_dictionary_t     *dictionary;
    
    dictionary = wi_dictionary();

    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dictionary), wi_dictionary_runtime_id(), "");
    
    dictionary = wi_mutable_dictionary();
    
    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dictionary), wi_dictionary_runtime_id(), "");
    
    dictionary = wi_dictionary_with_data_and_keys(WI_STR("1"), WI_STR("foo"), WI_STR("2"), WI_STR("bar"), NULL);

    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dictionary), wi_dictionary_runtime_id(), "");
    
    dictionary = wi_mutable_dictionary_with_data_and_keys(WI_STR("1"), WI_STR("foo"), WI_STR("2"), WI_STR("bar"), NULL);
    
    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dictionary), wi_dictionary_runtime_id(), "");
    
    dictionary = wi_autorelease(wi_dictionary_init_with_data_and_keys(wi_dictionary_alloc(), WI_STR("1"), WI_STR("foo"), WI_STR("2"), WI_STR("bar"), NULL));

    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dictionary), wi_dictionary_runtime_id(), "");
}



void wi_test_dictionary_serialization(void) {
#ifdef WI_PLIST
    wi_dictionary_t     *dictionary1, *dictionary2;
    wi_string_t         *path;
    
    dictionary1 = wi_dictionary_with_plist_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-dictionary-tests-1.plist")));
    
    WI_TEST_ASSERT_NOT_NULL(dictionary1, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(dictionary1), wi_dictionary_runtime_id(), "");
    
    dictionary1 = wi_dictionary_with_plist_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-dictionary-tests-2.plist")));
    
    WI_TEST_ASSERT_NULL(dictionary1, "");
    
    dictionary1 = wi_dictionary_with_plist_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-dictionary-tests-3.plist")));
    
    WI_TEST_ASSERT_NULL(dictionary1, "");

    dictionary1 = wi_dictionary_with_data_and_keys(WI_STR("1"), WI_STR("foo"), WI_STR("2"), WI_STR("bar"), NULL);
    path = wi_fs_temporary_path_with_template(WI_STR("/tmp/libwired-test.plist.XXXXXXX"));
    
    wi_dictionary_write_to_file(dictionary1, path);

    dictionary2 = wi_dictionary_with_plist_file(path);
    
    wi_fs_delete_path(path);

    WI_TEST_ASSERT_EQUAL_INSTANCES(dictionary1, dictionary2, "");
#endif
}



void wi_test_dictionary_runtime_functions(void) {
    wi_dictionary_t             *dictionary1;
    wi_mutable_dictionary_t     *dictionary2;

    dictionary1 = wi_dictionary_with_data_and_keys(WI_STR("1"), WI_STR("foo"), WI_STR("2"), WI_STR("bar"), NULL);
    dictionary2 = wi_autorelease(wi_mutable_copy(dictionary1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(dictionary1, dictionary2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(dictionary1), wi_hash(dictionary2), "");
    
    wi_mutable_dictionary_remove_data_for_key(dictionary2, WI_STR("bar"));
    wi_mutable_dictionary_set_data_for_key(dictionary2, WI_STR("2"), WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(dictionary1, dictionary2, "");
    
    wi_mutable_dictionary_remove_data_for_key(dictionary2, WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(dictionary1, dictionary2, "");
    
    dictionary2 = wi_autorelease(wi_dictionary_init_with_capacity_and_callbacks(wi_mutable_dictionary_alloc(), 0, wi_dictionary_null_key_callbacks, wi_dictionary_null_value_callbacks));
    
    wi_mutable_dictionary_set_data_for_key(dictionary2, (void *) 1, "foo");
    wi_mutable_dictionary_set_data_for_key(dictionary2, (void *) 2, "bar");
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(dictionary1, dictionary2, "");

    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(dictionary1), WI_STR("foo"), 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(dictionary2), WI_STR("0x1"), 0), WI_NOT_FOUND, "");
}



void wi_test_dictionary_instances(void) {
    wi_dictionary_t     *dictionary;
    wi_array_t          *array;
    
    dictionary = wi_dictionary_with_data_and_keys(WI_STR("1"), WI_STR("foo"), WI_STR("2"), WI_STR("bar"), NULL);
    
    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_dictionary_count(dictionary), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_dictionary_contains_key(dictionary, WI_STR("foo")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary, WI_STR("foo")), WI_STR("1"), "");
    WI_TEST_ASSERT_TRUE(wi_dictionary_contains_key(dictionary, WI_STR("bar")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary, WI_STR("bar")), WI_STR("2"), "");
    
    array = wi_dictionary_all_keys(dictionary);

    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("bar")), "");
    
    array = wi_dictionary_all_values(dictionary);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("1")), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, WI_STR("2")), "");
    
    array = wi_dictionary_keys_sorted_by_value(dictionary, wi_string_compare);

    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("bar"), "");
    
    array = wi_dictionary_keys_sorted_by_value(wi_dictionary(), wi_string_compare);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 0U, "");
}



void wi_test_dictionary_scalars(void) {
    wi_mutable_dictionary_t     *dictionary;
    wi_array_t                  *array;
    
    dictionary = wi_autorelease(wi_dictionary_init_with_capacity_and_callbacks(wi_mutable_dictionary_alloc(), 0, wi_dictionary_null_key_callbacks, wi_dictionary_null_value_callbacks));
    
    wi_mutable_dictionary_set_data_for_key(dictionary, "1", "foo");
    wi_mutable_dictionary_set_data_for_key(dictionary, "2", "bar");
    
    WI_TEST_ASSERT_NOT_NULL(dictionary, "");
    WI_TEST_ASSERT_EQUALS(wi_dictionary_count(dictionary), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_dictionary_contains_key(dictionary, "foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary, "foo"), "1", "");
    WI_TEST_ASSERT_TRUE(wi_dictionary_contains_key(dictionary, "bar"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary, "bar"), "2", "");
    
    array = wi_dictionary_all_keys(dictionary);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, "foo"), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, "bar"), "");
    
    array = wi_dictionary_all_values(dictionary);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, "1"), "");
    WI_TEST_ASSERT_TRUE(wi_array_contains_data(array, "2"), "");
    
    array = wi_dictionary_keys_sorted_by_value(dictionary, (wi_compare_func_t *) strcmp);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), "foo", "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), "bar", "");
    
    array = wi_dictionary_keys_sorted_by_value(wi_dictionary(), (wi_compare_func_t *) strcmp);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 0U, "");
}



void wi_test_dictionary_enumeration(void) {
    wi_mutable_dictionary_t     *dictionary;
    wi_enumerator_t             *enumerator;
    wi_string_t                 *string;
    wi_number_t                 *number;
    wi_uinteger_t               i;
    
    dictionary = wi_mutable_dictionary();
    
    for(i = 1; i <= 10000; i++)
        wi_mutable_dictionary_set_data_for_key(dictionary, wi_string_with_format(WI_STR("%u"), i), WI_INT32(i));
    
    enumerator = wi_dictionary_key_enumerator(dictionary);
    i = 0;
    
    while((number = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_TRUE(wi_number_int32(number) >= 1, "");
        WI_TEST_ASSERT_TRUE(wi_number_int32(number) <= 10000, "");
        
        i++;
    }

    WI_TEST_ASSERT_EQUALS(i, 10000, "");

    enumerator = wi_dictionary_data_enumerator(dictionary);
    i = 0;
    
    while((string = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_TRUE(wi_string_int32(string) >= 1, "");
        WI_TEST_ASSERT_TRUE(wi_string_int32(string) <= 10000, "");
        
        i++;
    }

    for(i = 1; i <= 10000; i++)
        wi_mutable_dictionary_remove_data_for_key(dictionary, WI_INT32(i));
}



void wi_test_dictionary_mutation(void) {
    wi_mutable_dictionary_t     *dictionary1, *dictionary2;
    
    dictionary1 = wi_mutable_dictionary();
    
    wi_mutable_dictionary_set_data_for_key(dictionary1, WI_STR("1"), WI_STR("foo"));
    wi_mutable_dictionary_set_data_for_key(dictionary1, WI_STR("2"), WI_STR("bar"));
    
    WI_TEST_ASSERT_EQUALS(wi_dictionary_count(dictionary1), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary1, WI_STR("foo")), WI_STR("1"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary1, WI_STR("bar")), WI_STR("2"), "");
    
    wi_mutable_dictionary_set_data_for_key(dictionary1, WI_STR("1"), WI_STR("foo"));
    wi_mutable_dictionary_set_data_for_key(dictionary1, WI_STR("2"), WI_STR("bar"));
    
    WI_TEST_ASSERT_EQUALS(wi_dictionary_count(dictionary1), 2U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary1, WI_STR("foo")), WI_STR("1"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_dictionary_data_for_key(dictionary1, WI_STR("bar")), WI_STR("2"), "");
    
    dictionary2 = wi_mutable_copy(dictionary1);
    
    wi_mutable_dictionary_set_data_for_key(dictionary2, WI_STR("3"), WI_STR("baz"));
    
    wi_mutable_dictionary_set_dictionary(dictionary1, dictionary2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(dictionary1, dictionary2, "");
    
    wi_mutable_dictionary_remove_all_data(dictionary1);

    WI_TEST_ASSERT_EQUALS(wi_dictionary_count(dictionary1), 0U, "");
}
