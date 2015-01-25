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

#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

#include <wired/wi-enumerator.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>

struct _wi_enumerator {
    wi_runtime_base_t                   base;
    
    wi_runtime_instance_t               *collection;
    wi_enumerator_func_t                *func;
    wi_enumerator_context_t             context;
};

static void                             _wi_enumerator_dealloc(wi_runtime_instance_t *);
static wi_string_t *                    _wi_enumerator_description(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_enumerator_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_enumerator_runtime_class = {
    "wi_enumerator_t",
    _wi_enumerator_dealloc,
    NULL,
    NULL,
    _wi_enumerator_description,
    NULL
};



void wi_enumerator_register(void) {
    _wi_enumerator_runtime_id = wi_runtime_register_class(&_wi_enumerator_runtime_class);
}



void wi_enumerator_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_enumerator_runtime_id(void) {
    return _wi_enumerator_runtime_id;
}



#pragma mark -

wi_enumerator_t * wi_enumerator_alloc(void) {
    return wi_runtime_create_instance(_wi_enumerator_runtime_id, sizeof(wi_enumerator_t));
}



wi_enumerator_t * wi_enumerator_init_with_collection(wi_enumerator_t *enumerator, wi_runtime_instance_t *collection, wi_enumerator_func_t *func) {
    enumerator->collection  = wi_retain(collection);
    enumerator->func        = func;
    
    return enumerator;
}



static void _wi_enumerator_dealloc(wi_runtime_instance_t *instance) {
    wi_enumerator_t     *enumerator = instance;
    
    wi_release(enumerator->collection);
}



static wi_string_t * _wi_enumerator_description(wi_runtime_instance_t *instance) {
    wi_enumerator_t     *enumerator = instance;

    return wi_string_with_format(WI_STR("<%@ %p>{collection = %@}"),
        wi_runtime_class_name(enumerator),
        enumerator,
        enumerator->collection);
}



#pragma mark -

wi_array_t * wi_enumerator_all_data(wi_enumerator_t *enumerator) {
    wi_mutable_array_t                  *array;
    wi_array_callbacks_t                callbacks, array_callbacks;
    wi_dictionary_value_callbacks_t     dictionary_callbacks;
    wi_set_callbacks_t                  set_callbacks;
    void                                *data;
    wi_uinteger_t                       count;
    
    if(wi_runtime_id(enumerator->collection) == wi_array_runtime_id()) {
        array_callbacks         = wi_array_callbacks(enumerator->collection);
        count                   = wi_array_count(enumerator->collection);
        
        callbacks.retain        = array_callbacks.retain;
        callbacks.release       = array_callbacks.release;
        callbacks.is_equal      = array_callbacks.is_equal;
        callbacks.description   = array_callbacks.description;
    }
    else if(wi_runtime_id(enumerator->collection) == wi_dictionary_runtime_id()) {
        count                   = wi_dictionary_count(enumerator->collection);
        dictionary_callbacks    = wi_dictionary_value_callbacks(enumerator->collection);
        
        callbacks.retain        = dictionary_callbacks.retain;
        callbacks.release       = dictionary_callbacks.release;
        callbacks.is_equal      = dictionary_callbacks.is_equal;
        callbacks.description   = dictionary_callbacks.description;
    }
    else if(wi_runtime_id(enumerator->collection) == wi_set_runtime_id()) {
        count                   = wi_set_count(enumerator->collection);
        set_callbacks           = wi_set_callbacks(enumerator->collection);
        
        callbacks.retain        = set_callbacks.retain;
        callbacks.release       = set_callbacks.release;
        callbacks.is_equal      = set_callbacks.is_equal;
        callbacks.description   = set_callbacks.description;
    }
    else {
        count                   = 0;
        callbacks               = wi_array_default_callbacks;
    }
    
    array = wi_array_init_with_capacity_and_callbacks(wi_mutable_array_alloc(), count, callbacks);
    
    while((data = wi_enumerator_next_data(enumerator)))
        wi_mutable_array_add_data(array, data);
    
    wi_runtime_make_immutable(array);
    
    return array;
}



void * wi_enumerator_next_data(wi_enumerator_t *enumerator) {
    void    *data;
    
    if(!(*enumerator->func)(enumerator->collection, &enumerator->context, &data))
        return NULL;
    
    return data;
}



wi_boolean_t wi_enumerator_get_next_data(wi_enumerator_t *enumerator, void **data) {
    return (*enumerator->func)(enumerator->collection, &enumerator->context, data);
}
