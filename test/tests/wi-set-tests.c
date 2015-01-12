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
WI_TEST_EXPORT void						wi_test_set_copying(void);
WI_TEST_EXPORT void						wi_test_set_enumeration(void);
WI_TEST_EXPORT void						wi_test_set_mutation(void);
WI_TEST_EXPORT void						wi_test_set_scalars(void);


void wi_test_set_creation(void) {
    wi_array_t  *array;
    wi_set_t    *set;
    
    set = wi_set();

    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(set), wi_set_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 0U, "");
    
    set = wi_set_with_data(WI_STR("foo"), WI_STR("bar"), NULL);

    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("bar")), "");
    WI_TEST_ASSERT_TRUE(wi_string_index_of_string(wi_description(set), WI_STR("foo"), 0) != WI_NOT_FOUND, "");
    
    set = wi_autorelease(wi_set_init_with_data(wi_set_alloc(), WI_STR("foo"), WI_STR("bar"), NULL));
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("bar")), "");
    
    array = wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    set = wi_autorelease(wi_set_init_with_array(wi_set_alloc(), array));
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 2U, "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("bar")), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_set_all_data(set), array, "");
}



void wi_test_set_copying(void) {
    wi_set_t            *set1;
    wi_mutable_set_t    *set2;
    
    set1 = wi_set_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    set2 = wi_mutable_copy(set1);
    
    WI_TEST_ASSERT_EQUALS(wi_hash(set1), wi_hash(set2), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(set1, set2, "");
    
    wi_mutable_set_remove_data(set2, WI_STR("bar"));
    wi_mutable_set_add_data(set2, WI_STR("baz"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(set1, set2, "");
}



void wi_test_set_enumeration(void) {
    wi_set_t            *set;
    wi_enumerator_t     *enumerator;
    wi_string_t         *string;
    wi_uinteger_t       i = 0;
    
    set = wi_set_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    enumerator = wi_set_data_enumerator(set);
    
    while((string = wi_enumerator_next_data(enumerator))) {
        if(i == 0)
            WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("foo"), "");
        else if(i == 0)
            WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("bar"), "");
        
        i++;
    }
}



void wi_test_set_mutation(void) {
	wi_mutable_set_t    *set;
    wi_array_t          *array;
	
	set = wi_mutable_set();
	
	WI_TEST_ASSERT_NOT_NULL(set, "");
	
	wi_mutable_set_add_data(set, WI_STR("foo"));
	
	WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
	WI_TEST_ASSERT_FALSE(wi_set_contains_data(set, WI_STR("bar")), "");
	WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set, WI_STR("foo")), 1U, "");
	
	set = wi_autorelease(wi_set_init_with_capacity(wi_mutable_set_alloc(), 0, true));

	WI_TEST_ASSERT_NOT_NULL(set, "");
	
	wi_mutable_set_add_data(set, WI_STR("foo"));
	wi_mutable_set_add_data(set, WI_STR("foo"));

	WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
	WI_TEST_ASSERT_FALSE(wi_set_contains_data(set, WI_STR("bar")), "");
	WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set, WI_STR("foo")), 2U, "");

	wi_mutable_set_remove_data(set, WI_STR("foo"));

	WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
	WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set, WI_STR("foo")), 1U, "");
	
	wi_mutable_set_remove_data(set, WI_STR("foo"));
	
	WI_TEST_ASSERT_FALSE(wi_set_contains_data(set, WI_STR("foo")), "");
	WI_TEST_ASSERT_EQUALS(wi_set_count_for_data(set, WI_STR("foo")), 0U, "");

    array = wi_array_with_data(WI_STR("foo"), WI_STR("bar"), NULL);
    
    wi_mutable_set_add_data_from_array(set, array);

    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("foo")), "");
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("bar")), "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 2U, "");

    wi_mutable_set_set_set(set, wi_set_with_data(WI_STR("1"), NULL));
    
    WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, WI_STR("1")), "");
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 1U, "");
}



void wi_test_set_scalars(void) {
    wi_mutable_set_t    *set;
    wi_enumerator_t     *enumerator;
    wi_uinteger_t       i;
    wi_boolean_t        bools[10000];
    
    set = wi_set_init_with_capacity_and_callbacks(wi_mutable_set_alloc(), 0, false, wi_set_null_callbacks);
    
    WI_TEST_ASSERT_NOT_NULL(set, "");
    
    for(i = 1; i <= 10000; i++) {
        wi_mutable_set_add_data(set, (void *) i);
        
        bools[i - 1] = false;
    }
    
    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 10000U, "");
    WI_TEST_ASSERT_TRUE(wi_string_index_of_string(wi_description(set), WI_STR("0x1234"), 0) != WI_NOT_FOUND, "");
    
    enumerator = wi_set_data_enumerator(set);
    
    while((i = (wi_uinteger_t) wi_enumerator_next_data(enumerator)))
        bools[i - 1] = true;

    for(i = 1; i <= 10000; i++) {
        WI_TEST_ASSERT_TRUE(wi_set_contains_data(set, (void *) i), "");
        WI_TEST_ASSERT_TRUE(bools[i - 1], "");
    }
    
    wi_mutable_set_remove_all_data(set);

    WI_TEST_ASSERT_EQUALS(wi_set_count(set), 0U, "");
}
