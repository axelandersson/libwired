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

WI_TEST_EXPORT void                     wi_test_indexset_creation(void);
WI_TEST_EXPORT void                     wi_test_indexset_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_indexset_indexes(void);
WI_TEST_EXPORT void                     wi_test_indexset_enumeration_with_index(void);
WI_TEST_EXPORT void                     wi_test_indexset_enumeration_with_range(void);
WI_TEST_EXPORT void                     wi_test_indexset_mutation(void);


void wi_test_indexset_creation(void) {
    wi_indexset_t    *indexset;
    
    indexset = wi_indexset();

    WI_TEST_ASSERT_NOT_NULL(indexset, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset), 0U, "");
    
    indexset = wi_indexset_with_index(1);

    WI_TEST_ASSERT_NOT_NULL(indexset, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset), 1U, "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 1), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset), 1U, "");
    
    indexset = wi_indexset_with_indexes_in_range(wi_make_range(1, 3));
    
    WI_TEST_ASSERT_NOT_NULL(indexset, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset), 3U, "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 1), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 2), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 3), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset), 3U, "");
}



void wi_test_indexset_runtime_functions(void) {
    wi_indexset_t           *indexset1;
    wi_mutable_indexset_t   *indexset2;
    
    indexset1 = wi_indexset_with_indexes_in_range(wi_make_range(1, 3));
    indexset2 = wi_autorelease(wi_mutable_copy(indexset1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(indexset1, indexset2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(indexset1), wi_hash(indexset2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(indexset1), wi_indexset_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(indexset2), wi_indexset_runtime_id(), "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(indexset1) & WI_RUNTIME_OPTION_IMMUTABLE, "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(indexset2) & WI_RUNTIME_OPTION_MUTABLE, "");
    
    wi_mutable_indexset_remove_index(indexset2, 1);
    wi_mutable_indexset_add_index(indexset2, 4);
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(indexset1, indexset2, "");
    
    WI_TEST_ASSERT_NOT_EQUALS(wi_string_index_of_string(wi_description(indexset2), WI_STR("4"), 0), WI_NOT_FOUND, "");
}



void wi_test_indexset_indexes(void) {
    wi_indexset_t   *indexset;
    
    indexset = wi_indexset_with_indexes_in_range(wi_make_range(1, 3));
    
    WI_TEST_ASSERT_NOT_NULL(indexset, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset), 3U, "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 1), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 2), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset, 3), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset, 4), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_indexes(indexset, wi_indexset_with_index(1)), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_indexes(indexset, wi_indexset_with_index(4)), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_indexes_in_range(indexset, wi_make_range(2, 1)), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_indexes_in_range(indexset, wi_make_range(4, 1)), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset), 3U, "");
}



void wi_test_indexset_enumeration_with_index(void) {
    wi_mutable_indexset_t   *indexset;
    wi_enumerator_t         *enumerator;
    wi_uinteger_t           i, index;
    
    indexset = wi_mutable_indexset();
    
    for(i = 0; i < 1000; i++)
        wi_mutable_indexset_add_index(indexset, i);
    
    enumerator = wi_indexset_index_enumerator(indexset);
    i = 0;
    
    while(wi_enumerator_get_next_data(enumerator, (void **) &index)) {
        WI_TEST_ASSERT_EQUALS(index, i, "");
        
        i++;
    }
    
    for(i = 0; i < 1000; i++)
        wi_mutable_indexset_remove_index(indexset, i);
}



void wi_test_indexset_enumeration_with_range(void) {
    wi_indexset_t       *indexset;
    wi_enumerator_t     *enumerator;
    wi_uinteger_t       i, index;
    
    indexset = wi_indexset_with_indexes_in_range(wi_make_range(0, 1000));
    enumerator = wi_indexset_index_enumerator(indexset);
    i = 0;
    
    while(wi_enumerator_get_next_data(enumerator, (void **) &index)) {
        WI_TEST_ASSERT_EQUALS(index, i, "");
        
        i++;
    }
}



void wi_test_indexset_mutation(void) {
    wi_mutable_indexset_t     *indexset1, *indexset2;
    
    indexset1 = wi_mutable_indexset();
    
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset1), 0U, "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 1), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 2), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 3), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset1), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset1), 0U, "");
    
    wi_mutable_indexset_add_index(indexset1, 1);
    wi_mutable_indexset_add_indexes(indexset1, wi_indexset_with_index(2));
    wi_mutable_indexset_add_indexes_in_range(indexset1, wi_make_range(3, 1));
    
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset1), 3U, "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset1, 1), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset1, 2), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset1, 3), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset1), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset1), 3U, "");
    
    wi_mutable_indexset_remove_index(indexset1, 1);
    wi_mutable_indexset_remove_indexes(indexset1, wi_indexset_with_index(2));
    wi_mutable_indexset_remove_indexes_in_range(indexset1, wi_make_range(3, 1));
    
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset1), 0U, "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 1), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 2), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 3), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset1), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset1), 0U, "");

    wi_mutable_indexset_add_indexes_in_range(indexset1, wi_make_range(1, 3));
    
    indexset2 = wi_autorelease(wi_mutable_copy(indexset1));

    wi_mutable_indexset_add_index(indexset2, 5);
    wi_mutable_indexset_add_index(indexset2, 4);

    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset2), 5U, "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset2, 1), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset2, 2), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset2, 3), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset2, 4), "");
    WI_TEST_ASSERT_TRUE(wi_indexset_contains_index(indexset2, 5), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset2), 1U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset2), 5U, "");
    
    wi_mutable_indexset_set_indexes(indexset1, indexset2);

    WI_TEST_ASSERT_EQUAL_INSTANCES(indexset1, indexset2, "");
    
    wi_mutable_indexset_remove_all_indexes(indexset1);
    
    WI_TEST_ASSERT_EQUALS(wi_indexset_count(indexset1), 0U, "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 1), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 2), "");
    WI_TEST_ASSERT_FALSE(wi_indexset_contains_index(indexset1, 3), "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_first_index(indexset1), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_indexset_last_index(indexset1), 0U, "");
}
