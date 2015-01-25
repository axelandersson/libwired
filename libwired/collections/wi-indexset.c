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

#include <wired/wi-array.h>
#include <wired/wi-assert.h>
#include <wired/wi-indexset.h>
#include <wired/wi-macros.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

struct _wi_indexset {
    wi_runtime_base_t                   base;
    
    wi_mutable_array_t                  *array;
};


static void                             _wi_indexset_dealloc(wi_runtime_instance_t *);
static wi_runtime_instance_t *          _wi_indexset_copy(wi_runtime_instance_t *);
static wi_boolean_t                     _wi_indexset_is_equal(wi_runtime_instance_t *, wi_runtime_instance_t *);
static wi_string_t *                    _wi_indexset_description(wi_runtime_instance_t *);
static wi_hash_code_t                   _wi_indexset_hash(wi_runtime_instance_t *);

static wi_integer_t                     _wi_index_compare(wi_runtime_instance_t *, wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_indexset_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_indexset_runtime_class = {
    "wi_indexset_t",
    _wi_indexset_dealloc,
    _wi_indexset_copy,
    _wi_indexset_is_equal,
    _wi_indexset_description,
    _wi_indexset_hash
};



void wi_indexset_register(void) {
    _wi_indexset_runtime_id = wi_runtime_register_class(&_wi_indexset_runtime_class);
}



void wi_indexset_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_indexset_runtime_id(void) {
    return _wi_indexset_runtime_id;
}



#pragma mark -

wi_indexset_t * wi_indexset(void) {
    return wi_autorelease(wi_indexset_init(wi_indexset_alloc()));
}



wi_indexset_t * wi_indexset_with_index(wi_uinteger_t index) {
    return wi_autorelease(wi_indexset_init_with_index(wi_indexset_alloc(), index));
}



wi_indexset_t * wi_indexset_with_indexes_in_range(wi_range_t range) {
    return wi_autorelease(wi_indexset_init_with_indexes_in_range(wi_indexset_alloc(), range));
}



wi_mutable_indexset_t * wi_mutable_indexset(void) {
    return wi_autorelease(wi_indexset_init(wi_mutable_indexset_alloc()));
}



#pragma mark -

wi_indexset_t * wi_indexset_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_indexset_runtime_id, sizeof(wi_indexset_t), WI_RUNTIME_OPTION_IMMUTABLE);
}



wi_mutable_indexset_t * wi_mutable_indexset_alloc(void) {
    return wi_runtime_create_instance_with_options(_wi_indexset_runtime_id, sizeof(wi_indexset_t), WI_RUNTIME_OPTION_MUTABLE);
}



wi_indexset_t * wi_indexset_init(wi_indexset_t *indexset) {
    indexset->array = wi_array_init_with_capacity_and_callbacks(wi_mutable_array_alloc(), 0, wi_array_null_callbacks);
    
    return indexset;
}



wi_indexset_t * wi_indexset_init_with_index(wi_indexset_t *indexset, wi_uinteger_t index) {
    indexset = wi_indexset_init(indexset);
    
    wi_mutable_array_add_data(indexset->array, (void *) index);
    
    return indexset;
}



wi_indexset_t * wi_indexset_init_with_indexes_in_range(wi_indexset_t *indexset, wi_range_t range) {
    wi_uinteger_t   index;
    
    indexset = wi_indexset_init(indexset);
    
    for(index = range.location; index < range.location + range.length; index++)
        wi_mutable_array_add_data(indexset->array, (void *) index);
    
    return indexset;
}



static void _wi_indexset_dealloc(wi_runtime_instance_t *instance) {
    wi_indexset_t   *indexset = instance;
    
    wi_release(indexset->array);
}



static wi_runtime_instance_t * _wi_indexset_copy(wi_runtime_instance_t *instance) {
    wi_indexset_t   *indexset = instance, *indexset_copy;
    
    indexset_copy = wi_indexset_init(wi_indexset_alloc());
    
    wi_mutable_array_set_array(indexset_copy->array, indexset->array);
    
    return indexset_copy;
}



static wi_boolean_t _wi_indexset_is_equal(wi_runtime_instance_t *instance1, wi_runtime_instance_t *instance2) {
    wi_indexset_t   *indexset1 = instance1;
    wi_indexset_t   *indexset2 = instance2;
    
    return wi_is_equal(indexset1->array, indexset2->array);
}



static wi_string_t * _wi_indexset_description(wi_runtime_instance_t *instance) {
    wi_indexset_t           *indexset = instance;
    wi_mutable_string_t     *string;
    wi_string_t             *description;
    void                    *data;
    wi_uinteger_t           i;
    
    string = wi_mutable_string_with_format(WI_STR("<%@ %p>{count = %lu, mutable = %u, indexes = (\n"),
        wi_runtime_class_name(indexset),
        indexset,
        wi_array_count(indexset->array),
        wi_runtime_options(indexset) & WI_RUNTIME_OPTION_MUTABLE ? 1 : 0);
    
    for(i = 0; i < wi_array_count(indexset->array); i++) {
        data = WI_ARRAY(indexset->array, i);
        
        wi_mutable_string_append_format(string, WI_STR("    %lu\n"), (wi_uinteger_t) data);
    }
    
    wi_mutable_string_append_string(string, WI_STR(")}"));
    
    wi_runtime_make_immutable(string);
    
    return string;
}



static wi_hash_code_t _wi_indexset_hash(wi_runtime_instance_t *instance) {
    wi_indexset_t   *indexset = instance;
    
    return wi_hash(indexset->array);
}


#pragma mark -

wi_uinteger_t wi_indexset_count(wi_indexset_t *indexset) {
    return wi_array_count(indexset->array);
}



#pragma mark -

wi_boolean_t wi_indexset_contains_index(wi_indexset_t *indexset, wi_uinteger_t index) {
    return wi_array_contains_data(indexset->array, (void *) index);
}



wi_boolean_t wi_indexset_contains_indexes(wi_indexset_t *indexset, wi_indexset_t *otherindexset) {
    wi_enumerator_t     *enumerator;
    wi_uinteger_t       index;
    
    enumerator = wi_array_data_enumerator(otherindexset->array);
    
    while(wi_enumerator_get_next_data(enumerator, (void **) &index)) {
        if(!wi_array_contains_data(indexset->array, (void *) index))
            return false;
    }
    
    return true;
}



wi_boolean_t wi_indexset_contains_indexes_in_range(wi_indexset_t *indexset, wi_range_t range) {
    wi_uinteger_t   index;
    
    for(index = range.location; index < range.location + range.length; index++) {
        if(!wi_array_contains_data(indexset->array, (void *) index))
            return false;
    }
    
    return true;
}



#pragma mark -

wi_uinteger_t wi_indexset_first_index(wi_indexset_t *indexset) {
    return (wi_uinteger_t) wi_array_first_data(indexset->array);
}



wi_uinteger_t wi_indexset_last_index(wi_indexset_t *indexset) {
    return (wi_uinteger_t) wi_array_last_data(indexset->array);
}



#pragma mark -

wi_enumerator_t * wi_indexset_index_enumerator(wi_indexset_t *indexset) {
    return wi_array_data_enumerator(indexset->array);
}



#pragma mark -

static wi_integer_t _wi_index_compare(wi_runtime_instance_t *instance1, wi_runtime_instance_t *instance2) {
    wi_uinteger_t   index1 = instance1;
    wi_uinteger_t   index2 = instance2;
    
    return (index1 > index2) ? 1 : ((index1 < index2) ? -1 : 0);
}



#pragma mark -

void wi_mutable_indexset_add_index(wi_mutable_indexset_t *indexset, wi_uinteger_t index) {
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    wi_mutable_array_add_data_sorted(indexset->array, (void *) index, _wi_index_compare);
}



void wi_mutable_indexset_add_indexes(wi_mutable_indexset_t *indexset, wi_indexset_t *otherindexset) {
    wi_enumerator_t     *enumerator;
    wi_uinteger_t       index;
    
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    enumerator = wi_array_data_enumerator(otherindexset->array);
    
    while(wi_enumerator_get_next_data(enumerator, (void **) &index))
        wi_mutable_array_add_data_sorted(indexset->array, (void *) index, _wi_index_compare);
}



void wi_mutable_indexset_add_indexes_in_range(wi_mutable_indexset_t *indexset, wi_range_t range) {
    wi_uinteger_t   index;
    
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    for(index = range.location; index < range.location + range.length; index++)
        wi_mutable_array_add_data_sorted(indexset->array, (void *) index, _wi_index_compare);
}



void wi_mutable_indexset_set_indexes(wi_mutable_indexset_t *indexset, wi_indexset_t *otherindexset) {
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    wi_mutable_array_set_array(indexset->array, otherindexset->array);
}



#pragma mark -

void wi_mutable_indexset_remove_index(wi_mutable_indexset_t *indexset, wi_uinteger_t index) {
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    wi_mutable_array_remove_data(indexset->array, (void *) index);
}



void wi_mutable_indexset_remove_indexes(wi_mutable_indexset_t *indexset, wi_indexset_t *otherindexset) {
    wi_enumerator_t     *enumerator;
    wi_uinteger_t       index;
    
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    enumerator = wi_array_data_enumerator(otherindexset->array);
    
    while(wi_enumerator_get_next_data(enumerator, (void **) &index))
        wi_mutable_array_remove_data(indexset->array, (void *) index);
}



void wi_mutable_indexset_remove_indexes_in_range(wi_mutable_indexset_t *indexset, wi_range_t range) {
    wi_uinteger_t   index;
    
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    for(index = range.location; index < range.location + range.length; index++)
        wi_mutable_array_remove_data(indexset->array, (void *) index);
}



void wi_mutable_indexset_remove_all_indexes(wi_mutable_indexset_t *indexset) {
    WI_RUNTIME_ASSERT_MUTABLE(indexset);
    
    wi_mutable_array_remove_all_data(indexset->array);
}
