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
#include <wired/wi-private.h>

WI_TEST_EXPORT void                     wi_test_enumerator_instances_enumeration(void);
WI_TEST_EXPORT void                     wi_test_enumerator_instances_all_data(void);
WI_TEST_EXPORT void                     wi_test_enumerator_scalars_enumeration(void);
WI_TEST_EXPORT void                     wi_test_enumerator_scalars_all_data(void);

static wi_boolean_t                     _wi_test_enumerator_instance_function(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);
static wi_boolean_t                     _wi_test_enumerator_scalar_function(wi_runtime_instance_t *, wi_enumerator_context_t *, void **);


void wi_test_enumerator_instances_enumeration(void) {
    wi_enumerator_t     *enumerator;
    wi_array_t          *array, *all_data;
    wi_number_t         *number;
    wi_uinteger_t       i;
    
    array = wi_array_with_data(WI_STR("foo"), NULL);
    
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), array, _wi_test_enumerator_instance_function));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(enumerator), wi_enumerator_runtime_id(), "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(enumerator), WI_STR("foo"), 0), WI_NOT_FOUND, "");
    
    i = 1;
    
    while((number = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_EQUAL_INSTANCES(number, WI_INT32(i), "");
        
        i++;
    }
    
    WI_TEST_ASSERT_NULL(wi_enumerator_next_data(enumerator), "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(wi_enumerator_all_data(enumerator)), 0U, "");

    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), array, _wi_test_enumerator_instance_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 0), WI_INT32(1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 1), WI_INT32(2), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 2), WI_INT32(3), "");
}



void wi_test_enumerator_instances_all_data(void) {
    wi_enumerator_t     *enumerator;
    wi_array_t          *array, *all_data;
    wi_dictionary_t     *dictionary;
    wi_set_t            *set;

    array = wi_array();
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), array, _wi_test_enumerator_instance_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 0), WI_INT32(1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 1), WI_INT32(2), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 2), WI_INT32(3), "");
    
    dictionary = wi_dictionary();
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), dictionary, _wi_test_enumerator_instance_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 0), WI_INT32(1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 1), WI_INT32(2), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 2), WI_INT32(3), "");
    
    set = wi_set();
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), set, _wi_test_enumerator_instance_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 0), WI_INT32(1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 1), WI_INT32(2), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 2), WI_INT32(3), "");
    
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), WI_STR("foo"), _wi_test_enumerator_instance_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 0), WI_INT32(1), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 1), WI_INT32(2), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(all_data, 2), WI_INT32(3), "");
}



static wi_boolean_t _wi_test_enumerator_instance_function(wi_runtime_instance_t *instance, wi_enumerator_context_t *context, void **data) {
    if(context->index == 3)
        return false;
    
    *data = WI_INT32(++context->index);
    
    return true;
}



void wi_test_enumerator_scalars_enumeration(void) {
    wi_enumerator_t     *enumerator;
    wi_mutable_array_t  *array;
    wi_array_t          *all_data;
    void                *number;
    wi_uinteger_t       i;
    
    array = wi_array_init_with_capacity_and_callbacks(wi_mutable_array(), 0, wi_array_null_callbacks);
    wi_mutable_array_add_data(array, (void *) 1);
    
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), array, _wi_test_enumerator_scalar_function));
    
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(enumerator), wi_enumerator_runtime_id(), "");
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(enumerator), WI_STR("0x1"), 0), WI_NOT_FOUND, "");
    
    i = 1;
    
    while((number = wi_enumerator_next_data(enumerator))) {
        WI_TEST_ASSERT_EQUALS(number, (void *) i, "");
        
        i++;
    }
    
    WI_TEST_ASSERT_NULL(wi_enumerator_next_data(enumerator), "");
    WI_TEST_ASSERT_EQUALS(wi_array_count(wi_enumerator_all_data(enumerator)), 0U, "");
    
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), array, _wi_test_enumerator_scalar_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 1), (void *) 2, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 2), (void *) 3, "");
}



void wi_test_enumerator_scalars_all_data(void) {
    wi_enumerator_t         *enumerator;
    wi_mutable_array_t      *array;
    wi_mutable_dictionary_t *dictionary;
    wi_mutable_set_t        *set;
    wi_array_t              *all_data;
    
    array = wi_autorelease(wi_array_init_with_capacity_and_callbacks(wi_array_alloc(), 0, wi_array_null_callbacks));
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), array, _wi_test_enumerator_scalar_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 1), (void *) 2, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 2), (void *) 3, "");
    
    dictionary = wi_autorelease(wi_dictionary_init_with_capacity_and_callbacks(wi_dictionary_alloc(), 0, wi_dictionary_null_key_callbacks, wi_dictionary_null_value_callbacks));
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), dictionary, _wi_test_enumerator_scalar_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 1), (void *) 2, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 2), (void *) 3, "");
    
    set = wi_autorelease(wi_set_init_with_capacity_and_callbacks(wi_set_alloc(), 0, false, wi_set_null_callbacks));
    enumerator = wi_autorelease(wi_enumerator_init_with_collection(wi_enumerator_alloc(), set, _wi_test_enumerator_scalar_function));
    all_data = wi_enumerator_all_data(enumerator);
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(all_data), 3U, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 0), (void *) 1, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 1), (void *) 2, "");
    WI_TEST_ASSERT_EQUALS(wi_array_data_at_index(all_data, 2), (void *) 3, "");
}



static wi_boolean_t _wi_test_enumerator_scalar_function(wi_runtime_instance_t *instance, wi_enumerator_context_t *context, void **data) {
    if(context->index == 3)
        return false;
    
    *data = (void *) ++context->index;
    
    return true;
}
