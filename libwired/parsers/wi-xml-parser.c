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

#ifndef WI_XML

int wi_xml_dummy = 0;

#else

#include <string.h>

#include <wired/wi-data.h>
#include <wired/wi-date.h>
#include <wired/wi-dictionary.h>
#include <wired/wi-macros.h>
#include <wired/wi-number.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>
#include <wired/wi-string-encoding.h>
#include <wired/wi-xml-parser.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlerror.h>
#include <libxml/xpath.h>

struct _wi_xml_node {
    wi_runtime_base_t                   base;
    
    wi_string_t                         *element;
    wi_string_t                         *text;
    wi_mutable_dictionary_t             *attributes;
    wi_mutable_array_t                  *children;
};


static wi_boolean_t                     _wi_xml_parser_parse_node(wi_xml_node_t *, xmlNodePtr);
static wi_string_t *                    _wi_xml_parser_string_from_chars(const xmlChar *);

static wi_xml_node_t *                  _wi_xml_node_alloc(void);
static wi_xml_node_t *                  _wi_xml_node_init(wi_xml_node_t *);
static void                             _wi_xml_node_dealloc(wi_runtime_instance_t *);
static wi_string_t *                    _wi_xml_node_description(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_xml_node_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_xml_node_runtime_class = {
    "wi_xml_node_t",
    _wi_xml_node_dealloc,
    NULL,
    NULL,
    _wi_xml_node_description,
    NULL
};



void wi_xml_node_register(void) {
    _wi_xml_node_runtime_id = wi_runtime_register_class(&_wi_xml_node_runtime_class);
}



void wi_xml_node_initialize(void) {
}



#pragma mark -

wi_xml_node_t * wi_xml_parser_read_node_from_path(wi_string_t *path) {
    wi_xml_node_t   *xml_node;
    xmlDocPtr       doc;
    xmlNodePtr      node;
    
    doc = xmlReadFile(wi_string_utf8_string(path), NULL, 0);
    
    if(!doc) {
        wi_error_set_libxml2_error();
        
        return NULL;
    }
    
    node = xmlDocGetRootElement(doc);
    xml_node = _wi_xml_node_init(_wi_xml_node_alloc());
    
    if(!_wi_xml_parser_parse_node(xml_node, node)) {
        xmlFreeDoc(doc);
        
        return NULL;
    }
    
    return wi_autorelease(xml_node);
}



wi_xml_node_t * wi_xml_parser_node_from_data(wi_data_t *data) {
    wi_xml_node_t   *xml_node;
    xmlDocPtr       doc;
    xmlNodePtr      node;
    
    doc = xmlReadMemory(wi_data_bytes(data), wi_data_length(data), NULL, NULL, 0);
    
    if(!doc) {
        wi_error_set_libxml2_error();
        
        return NULL;
    }
    
    node = xmlDocGetRootElement(doc);
    xml_node = _wi_xml_node_init(_wi_xml_node_alloc());
    
    if(!_wi_xml_parser_parse_node(xml_node, node)) {
        xmlFreeDoc(doc);
        
        return NULL;
    }
    
    return wi_autorelease(xml_node);
}



static wi_boolean_t _wi_xml_parser_parse_node(wi_xml_node_t *xml_node, xmlNodePtr node) {
    wi_xml_node_t   *child_xml_node;
    wi_string_t     *string;
    xmlNodePtr      child_node;
    xmlAttr         *attribute;
    xmlChar         *value;
    
    xml_node->element = wi_retain(_wi_xml_parser_string_from_chars(node->name));
    
    value = xmlNodeListGetString(node->doc, node->children, 1);
    
    if(value) {
        string = _wi_xml_parser_string_from_chars(value);
        
        if(wi_string_length(wi_string_by_deleting_surrounding_whitespace(string)) > 0)
            xml_node->text = wi_retain(string);
        
        xmlFree(value);
    }
    
    for(attribute = node->properties; attribute != NULL; attribute = attribute->next) {
        value = xmlNodeListGetString(attribute->doc, attribute->children, 1);
        
        if(value) {
            wi_mutable_dictionary_set_data_for_key(xml_node->attributes,
                                                   _wi_xml_parser_string_from_chars(value),
                                                   _wi_xml_parser_string_from_chars(attribute->name));
            
            xmlFree(value);
        }
    }
    
    for(child_node = node->children; child_node != NULL; child_node = child_node->next) {
        if(child_node->type == XML_ELEMENT_NODE) {
            child_xml_node = wi_autorelease(_wi_xml_node_init(_wi_xml_node_alloc()));
            
            if(!_wi_xml_parser_parse_node(child_xml_node, child_node))
                return false;
            
            wi_mutable_array_add_data(xml_node->children, child_xml_node);
        }
    }
    
    return true;
}



static wi_string_t * _wi_xml_parser_string_from_chars(const xmlChar *chars) {
    return wi_string_with_utf8_string((const char *) chars);
}



#pragma mark -

wi_runtime_id_t wi_xml_node_runtime_id(void) {
    return _wi_xml_node_runtime_id;
}



#pragma mark -

static wi_xml_node_t * _wi_xml_node_alloc(void) {
    return wi_runtime_create_instance(_wi_xml_node_runtime_id, sizeof(wi_xml_node_t));
}



static wi_xml_node_t * _wi_xml_node_init(wi_xml_node_t *xml_node) {
    xml_node->attributes = wi_dictionary_init(wi_mutable_dictionary_alloc());
    xml_node->children = wi_array_init(wi_mutable_array_alloc());
    
    return xml_node;
}



static void _wi_xml_node_dealloc(wi_runtime_instance_t *instance) {
    wi_xml_node_t   *xml_node = instance;
    
    wi_release(xml_node->element);
    wi_release(xml_node->text);
    wi_release(xml_node->attributes);
    wi_release(xml_node->children);
}



static wi_string_t * _wi_xml_node_description(wi_runtime_instance_t *instance) {
    wi_xml_node_t   *xml_node = instance;

    return wi_string_with_format(WI_STR("<%@ %p>{element = %@, text = %@, attributes = %@, children = %@}"),
        wi_runtime_class_name(xml_node),
        xml_node,
        xml_node->element,
        xml_node->text,
        xml_node->attributes,
        xml_node->children);
}



#pragma mark -

wi_string_t * wi_xml_node_element(wi_xml_node_t *xml_node) {
    return xml_node->element;
}



wi_string_t * wi_xml_node_text(wi_xml_node_t *xml_node) {
    return xml_node->text;
}



wi_dictionary_t * wi_xml_node_attributes(wi_xml_node_t *xml_node) {
    return wi_autorelease(wi_copy(xml_node->attributes));
}



wi_uinteger_t wi_xml_node_number_of_children(wi_xml_node_t *xml_node) {
    return wi_array_count(xml_node->children);
}



wi_xml_node_t * wi_xml_node_child_at_index(wi_xml_node_t *xml_node, wi_uinteger_t index) {
    return WI_ARRAY(xml_node->children, index);
}

#endif
