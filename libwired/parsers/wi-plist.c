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

#ifndef WI_PLIST

int wi_plist_dummy = 0;

#else

#include <string.h>

#include <wired/wi-data.h>
#include <wired/wi-date.h>
#include <wired/wi-dictionary.h>
#include <wired/wi-macros.h>
#include <wired/wi-number.h>
#include <wired/wi-plist.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>
#include <wired/wi-string-encoding.h>
#include <wired/wi-xml-parser.h>

static wi_runtime_instance_t *          _wi_plist_instance_for_node(wi_xml_node_t *);
static wi_runtime_instance_t *          _wi_plist_instance_for_content_node(wi_xml_node_t *);
static wi_string_t *                    _wi_plist_xml_string_for_instance(wi_runtime_instance_t *, wi_uinteger_t);
static wi_string_t *                    _wi_plist_xml_string_for_string(wi_string_t *);



wi_runtime_instance_t * wi_plist_read_instance_from_path(wi_string_t *path) {
    wi_xml_node_t     *node;
    
    node = wi_xml_parser_read_node_from_path(path);
    
    if(!node)
        return NULL;
    
    return _wi_plist_instance_for_node(node);
}



wi_runtime_instance_t * wi_plist_instance_for_string(wi_string_t *string) {
    wi_xml_node_t   *node;
    
    node = wi_xml_parser_node_from_data(wi_string_utf8_data(string));
    
    if(!node)
        return NULL;
    
    return _wi_plist_instance_for_node(node);
}



#pragma mark -

wi_boolean_t wi_plist_write_instance_to_path(wi_runtime_instance_t *instance, wi_string_t *path) {
    wi_string_t     *string;
    
    string = wi_plist_string_for_instance(instance);
    
    if(!string)
        return false;
    
    return wi_string_write_utf8_string_to_path(string, path);
}



wi_string_t * wi_plist_string_for_instance(wi_runtime_instance_t *instance) {
    wi_mutable_string_t     *string;
    
    string = wi_mutable_string();
    
    wi_mutable_string_append_string(string, WI_STR("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"));
    wi_mutable_string_append_string(string, WI_STR("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n"));
    wi_mutable_string_append_string(string, WI_STR("<plist version=\"1.0\">\n"));
    wi_mutable_string_append_string(string, _wi_plist_xml_string_for_instance(instance, 0));
    wi_mutable_string_append_string(string, WI_STR("</plist>\n"));
    
    wi_runtime_make_immutable(string);
    
    return string;
}



#pragma mark -

static wi_runtime_instance_t * _wi_plist_instance_for_node(wi_xml_node_t *node) {
    wi_runtime_instance_t   *collection;
    wi_xml_node_t           *collection_node, *content_node;
    wi_string_t             *element;
    wi_dictionary_t         *attributes;
    wi_uinteger_t           i;
    
    element = wi_xml_node_element(node);
    attributes = wi_xml_node_attributes(node);
    
    if(!wi_is_equal(element, WI_STR("plist"))) {
        wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                WI_STR("Root node \"%@\" is not equal to \"plist\""),
                                                element);
        
        return NULL;
    }
    
    if(!wi_is_equal(wi_dictionary_data_for_key(attributes, WI_STR("version")), WI_STR("1.0"))) {
        wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                WI_STR("Unsuppported plist version \"%@\""),
                                                wi_dictionary_data_for_key(attributes, WI_STR("version")));
        
        return NULL;
    }
    
    if(wi_xml_node_number_of_children(node) != 1) {
        wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                WI_STR("Root node should have one content node"));
        
        return NULL;
    }
    
    return _wi_plist_instance_for_content_node(wi_xml_node_child_at_index(node, 0));
}



static wi_runtime_instance_t * _wi_plist_instance_for_content_node(wi_xml_node_t *node) {
    wi_runtime_instance_t   *instance, *value;
    wi_string_t             *element, *key;
    wi_xml_node_t           *child_node;
    wi_uinteger_t           i;
    
    element = wi_xml_node_element(node);

    if(wi_is_equal(element, WI_STR("dict"))) {
        instance = wi_mutable_dictionary();
        
        if(wi_xml_node_number_of_children(node) % 2 != 0) {
            wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                    WI_STR("Content node \"dict\" must contain an even number of nodes"));
            
            return NULL;
        }
        
        for(i = 0; i < wi_xml_node_number_of_children(node); i += 2) {
            child_node = wi_xml_node_child_at_index(node, i);
            
            if(!wi_is_equal(wi_xml_node_element(child_node), WI_STR("key"))) {
                wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                        WI_STR("Content node \"dict\" node \"%@\" is not equal to \"key\""),
                                                        wi_xml_node_element(child_node));
                
                return NULL;
            }
            
            key = _wi_plist_instance_for_content_node(child_node);
            
            if(!key)
                return NULL;
            
            if(wi_string_length(key) == 0) {
                wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                        WI_STR("Content node \"dict\" node \"key\" must not be empty"),
                                                        wi_xml_node_element(child_node));
                
                return NULL;
            }

            child_node = wi_xml_node_child_at_index(node, i + 1);
            
            if(wi_is_equal(wi_xml_node_element(child_node), WI_STR("key"))) {
                wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                        WI_STR("Content node \"dict\" node \"%@\" is equal to \"key\""),
                                                        wi_xml_node_element(child_node));
                
                return NULL;
            }
            
            value = _wi_plist_instance_for_content_node(child_node);
            
            if(!value)
                return NULL;
            
            wi_mutable_dictionary_set_data_for_key(instance, value, key);
        }
        
        wi_runtime_make_immutable(instance);
    }
    else if(wi_is_equal(element, WI_STR("array"))) {
        instance = wi_mutable_array();
        
        for(i = 0; i < wi_xml_node_number_of_children(node); i++) {
            child_node = wi_xml_node_child_at_index(node, i);
            value = _wi_plist_instance_for_content_node(child_node);
            
            wi_mutable_array_add_data(instance, value);
        }
        
        wi_runtime_make_immutable(instance);
    }
    else if(wi_is_equal(element, WI_STR("key")) || wi_is_equal(element, WI_STR("string"))) {
        instance = wi_xml_node_text(node);
    }
    else if(wi_is_equal(element, WI_STR("integer"))) {
        instance = wi_number_with_integer(wi_string_integer(wi_xml_node_text(node)));
    }
    else if(wi_is_equal(element, WI_STR("real"))) {
        instance = wi_number_with_double(wi_string_double(wi_xml_node_text(node)));
    }
    else if(wi_is_equal(element, WI_STR("true"))) {
        instance = wi_number_with_bool(true);
    }
    else if(wi_is_equal(element, WI_STR("false"))) {
        instance = wi_number_with_bool(false);
    }
    else if(wi_is_equal(element, WI_STR("date"))) {
        instance = wi_date_with_rfc3339_string(wi_xml_node_text(node));
    }
    else if(wi_is_equal(element, WI_STR("data"))) {
        instance = wi_data_with_base64_string(wi_xml_node_text(node));
    }
    else {
        wi_error_set_libwired_error_with_format(WI_ERROR_PLIST_READFAILED,
                                                WI_STR("Content node \"%@\" is not supported"),
                                                element);
        
        return NULL;
    }
    
    return instance;
}



static wi_string_t * _wi_plist_xml_string_for_instance(wi_runtime_instance_t *instance, wi_uinteger_t level) {
    wi_mutable_string_t     *string;
    wi_enumerator_t         *enumerator;
    wi_array_t              *keys;
    wi_string_t             *key, *value;
    wi_runtime_id_t         id;
    wi_number_type_t        type;
    wi_uinteger_t           i;
    
    string = wi_mutable_string();
    id = wi_runtime_id(instance);
    
    for(i = 0; i < level; i++)
        wi_mutable_string_append_string(string, WI_STR("\t"));
    
    if(id == wi_dictionary_runtime_id()) {
        if(wi_dictionary_count(instance) == 0) {
            wi_mutable_string_append_string(string, WI_STR("<dict/>\n"));
        } else {
            wi_mutable_string_append_format(string, WI_STR("<dict>\n"));
            
            keys = wi_array_by_sorting(wi_dictionary_all_keys(instance), wi_string_compare);
            enumerator = wi_array_data_enumerator(keys);
            
            while((key = wi_enumerator_next_data(enumerator))) {
                value = wi_dictionary_data_for_key(instance, key);
                
                for(i = 0; i < level + 1; i++)
                    wi_mutable_string_append_string(string, WI_STR("\t"));
                
                wi_mutable_string_append_format(string, WI_STR("<key>%@</key>\n"), _wi_plist_xml_string_for_string(key));
                wi_mutable_string_append_string(string, _wi_plist_xml_string_for_instance(value, level + 1));
            }
            
            for(i = 0; i < level; i++)
                wi_mutable_string_append_string(string, WI_STR("\t"));
            
            wi_mutable_string_append_string(string, WI_STR("</dict>\n"));
        }
    }
    else if(id == wi_array_runtime_id()) {
        if(wi_array_count(instance) == 0) {
            wi_mutable_string_append_string(string, WI_STR("<array/>\n"));
        } else {
            wi_mutable_string_append_format(string, WI_STR("<array>\n"));
            
            enumerator = wi_array_data_enumerator(instance);
            
            while((value = wi_enumerator_next_data(enumerator)))
                wi_mutable_string_append_string(string, _wi_plist_xml_string_for_instance(value, level + 1));
            
            for(i = 0; i < level; i++)
                wi_mutable_string_append_string(string, WI_STR("\t"));
            
            wi_mutable_string_append_string(string, WI_STR("</array>\n"));
        }
    }
    else if(id == wi_string_runtime_id()) {
        wi_mutable_string_append_format(string, WI_STR("<string>%@</string>\n"), _wi_plist_xml_string_for_string(instance));
    }
    else if(id == wi_number_runtime_id()) {
        type = wi_number_type(instance);
        
        if(type == WI_NUMBER_BOOL) {
            if(wi_number_bool(instance))
                wi_mutable_string_append_string(string, WI_STR("<true/>\n"));
            else
                wi_mutable_string_append_string(string, WI_STR("<false/>\n"));
        } else {
            if(type == WI_NUMBER_FLOAT || type == WI_NUMBER_DOUBLE)
                wi_mutable_string_append_format(string, WI_STR("<real>%@</real>\n"), wi_number_string(instance));
            else
                wi_mutable_string_append_format(string, WI_STR("<integer>%@</integer>\n"), wi_number_string(instance));
        }
    }
    else if(id == wi_date_runtime_id()) {
        wi_mutable_string_append_format(string, WI_STR("<date>%@</date>\n"), wi_date_string_with_format(instance, WI_STR("%Y-%m-%dT%H:%M:%SZ")));
    }
    else if(id == wi_data_runtime_id()) {
        wi_mutable_string_append_format(string, WI_STR("<data>%@</data>\n"), wi_data_base64_string(instance));
    }
    
    return string;
}



static wi_string_t * _wi_plist_xml_string_for_string(wi_string_t *string) {
    wi_mutable_string_t     *escaped_string;
    
    escaped_string = wi_mutable_copy(string);
    
    wi_mutable_string_replace_string_with_string(escaped_string, WI_STR("&"), WI_STR("&amp;"), 0);
    wi_mutable_string_replace_string_with_string(escaped_string, WI_STR("<"), WI_STR("&lt;"), 0);
    wi_mutable_string_replace_string_with_string(escaped_string, WI_STR(">"), WI_STR("&gt;"), 0);
    
    wi_runtime_make_immutable(escaped_string);
    
    return escaped_string;
}

#endif
