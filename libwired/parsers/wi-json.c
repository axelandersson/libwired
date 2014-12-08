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

#include <ctype.h>
#include <string.h>

#include <wired/wi-data.h>
#include <wired/wi-date.h>
#include <wired/wi-dictionary.h>
#include <wired/wi-json.h>
#include <wired/wi-macros.h>
#include <wired/wi-number.h>
#include <wired/wi-null.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>

static wi_string_t *                _wi_json_quoted_string_for_string(wi_string_t *);
static wi_string_t *                _wi_json_number_string_for_string(wi_string_t *, wi_boolean_t *);
static wi_string_t *                _wi_json_boolean_or_null_string_for_string(wi_string_t *);

static wi_string_t *                _wi_json_string_for_dictionary(wi_dictionary_t *);
static wi_string_t *                _wi_json_string_for_array(wi_array_t *);
static wi_string_t *                _wi_json_string_for_key_and_value(wi_string_t *, wi_runtime_instance_t *);
static wi_string_t *                _wi_json_string_for_value(wi_runtime_instance_t *);
static wi_string_t *                _wi_json_string_for_string(wi_string_t *);
static wi_string_t *                _wi_json_string_for_number(wi_number_t *);
static wi_string_t *                _wi_json_string_for_null(wi_null_t *);


wi_runtime_instance_t * wi_json_read_instance_from_file(wi_string_t *path) {
    wi_string_t     *string;
	
    string = wi_autorelease(wi_string_init_with_contents_of_file(wi_string_alloc(), path));
	
	return wi_json_instance_for_string(string);
}



wi_runtime_instance_t * wi_json_instance_for_string(wi_string_t *string) {
	wi_runtime_instance_t       *top_container, *current_container, *current_underlying_container, *container, *current_value;
    wi_string_t                 *substring, *current_key;
    wi_uinteger_t               i;
    wi_boolean_t                isfloat;
    char                        ch;
    
    top_container = NULL;
    current_underlying_container = NULL;
    current_container = NULL;
    current_key = NULL;
    current_value = NULL;
    
    for(i = 0; i < wi_string_length(string); i++) {
        ch = wi_string_character_at_index(string, i);
        
        switch(ch) {
            case '{':
            case '[':
                if(ch == '{')
                    container = wi_mutable_dictionary();
                else
                    container = wi_mutable_array();
                
                if(current_container) {
                    if(wi_runtime_id(current_container) == wi_dictionary_runtime_id()) {
                        if(!current_key) {
                            wi_error_set_libwired_error_with_format(WI_ERROR_JSON_READFAILED,
                                WI_STR("Syntax error while reading collection"));
                            
                            return NULL;
                        }
                        
                        wi_mutable_dictionary_set_data_for_key(current_container, container, current_key);
                        
                        if(ch == '{')
                            current_key = NULL;
                    }
                    else if(wi_runtime_id(current_container) == wi_array_runtime_id()) {
                        wi_mutable_array_add_data(current_container, container);
                    }
                    
                    current_underlying_container = current_container;
                }
                
                current_container = container;
                
                if(!top_container)
                    top_container = current_container;
                break;
                
            case '}':
            case ']':
                current_container = current_underlying_container;
                current_key = NULL;
                break;
            
            case '"':
                substring = _wi_json_quoted_string_for_string(wi_string_substring_from_index(string, i));
                
                if(!substring) {
                    wi_log_info(WI_STR("no string parsed"));
                    return NULL;
                }
                
                if(!current_key)
                    current_key = substring;
                else
                    current_value = substring;
                
                if(current_value) {
                    if(wi_runtime_id(current_container) == wi_dictionary_runtime_id()) {
                        if(!current_key) {
                            wi_error_set_libwired_error_with_format(WI_ERROR_JSON_READFAILED,
                                WI_STR("Syntax error while reading string"));
                            
                            return NULL;
                        }
                        
                        wi_mutable_dictionary_set_data_for_key(current_container, current_value, current_key);
                        
                        current_key = NULL;
                    }
                    else if(wi_runtime_id(current_container) == wi_array_runtime_id()) {
                        wi_mutable_array_add_data(current_container, current_value);
                    }
                    
                    current_value = NULL;
                }
                
                i += wi_string_length(substring) + 1;
                break;
            
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                substring = _wi_json_number_string_for_string(wi_string_substring_from_index(string, i), &isfloat);
                
                if(!substring) {
                    wi_error_set_libwired_error_with_format(WI_ERROR_JSON_READFAILED,
                        WI_STR("Syntax error while reading number"));
                    
                    return NULL;
                }
                
                if(isfloat)
                    current_value = wi_number_with_double(wi_string_double(substring));
                else
                    current_value = wi_number_with_integer(wi_string_uinteger(substring));
                
                if(current_value) {
                    if(wi_runtime_id(current_container) == wi_dictionary_runtime_id()) {
                        if(!current_key) {
                            wi_error_set_libwired_error_with_format(WI_ERROR_JSON_READFAILED,
                                WI_STR("Syntax error while reading number"));
                            
                            return NULL;
                        }
                        
                        wi_mutable_dictionary_set_data_for_key(current_container, current_value, current_key);
                        
                        current_key = NULL;
                    }
                    else if(wi_runtime_id(current_container) == wi_array_runtime_id()) {
                        wi_mutable_array_add_data(current_container, current_value);
                    }
                
                    current_value = NULL;
                }
                
                i += wi_string_length(substring);
                break;
            
            case 't':
            case 'f':
            case 'n':
                substring = _wi_json_boolean_or_null_string_for_string(wi_string_substring_from_index(string, i));
                
                if(!substring) {
                    wi_error_set_libwired_error_with_format(WI_ERROR_JSON_READFAILED,
                        WI_STR("Syntax error while reading boolean or null"));
                    
                    return NULL;
                }
                
                if(wi_is_equal(substring, WI_STR("true")))
                    current_value = wi_number_with_bool(true);
                else if(wi_is_equal(substring, WI_STR("false")))
                    current_value = wi_number_with_bool(false);
                else if(wi_is_equal(substring, WI_STR("null")))
                    current_value = wi_null();
                
                if(current_value) {
                    if(wi_runtime_id(current_container) == wi_dictionary_runtime_id()) {
                        if(!current_key) {
                            wi_error_set_libwired_error_with_format(WI_ERROR_JSON_READFAILED,
                                WI_STR("Syntax error while reading boolean or null"));
                            
                            return NULL;
                        }
                        
                        wi_mutable_dictionary_set_data_for_key(current_container, current_value, current_key);
                        
                        current_key = NULL;
                    }
                    else if(wi_runtime_id(current_container) == wi_array_runtime_id()) {
                        wi_mutable_array_add_data(current_container, current_value);
                    }
                    
                    current_value = NULL;
                }
                
                i += wi_string_length(substring);
                break;
        }
    }
    
	return top_container;
}



#pragma mark -

wi_boolean_t wi_json_write_instance_to_file(wi_runtime_instance_t *instance, wi_string_t *path) {
	wi_string_t		*string;
	
	string = wi_json_string_for_instance(instance);
	
	if(!string)
		return false;
	
	return wi_string_write_to_file(string, path);
}



wi_string_t * wi_json_string_for_instance(wi_runtime_instance_t *instance) {
    wi_mutable_string_t     *string;
    
    string = wi_mutable_string();
    
    if(wi_runtime_id(instance) == wi_dictionary_runtime_id())
        wi_mutable_string_append_string(string, _wi_json_string_for_dictionary(instance));
    else if(wi_runtime_id(instance) == wi_array_runtime_id())
        wi_mutable_string_append_string(string, _wi_json_string_for_array(instance));
    
    return string;
}



#pragma mark -

static wi_string_t * _wi_json_quoted_string_for_string(wi_string_t *string) {
    wi_uinteger_t       i;
    wi_range_t          range;
    char                ch;
    
    range.location = 1;
    
    for(i = 1; i < wi_string_length(string); i++) {
        ch = wi_string_character_at_index(string, i);
        
        if(ch == '"') {
            range.length = i - 1;
            
            break;
        } else {
            if(ch == '\\' && i + 1 < wi_string_length(string)) {
                
            }
        }
    }
    
    return wi_string_substring_with_range(string, range);
}



static wi_string_t * _wi_json_number_string_for_string(wi_string_t *string, wi_boolean_t *isfloat) {
    wi_uinteger_t       i;
    wi_range_t          range;
    char                ch;
    
    *isfloat = false;
    
    range.location = 0;
    
    for(i = 0; i < wi_string_length(string); i++) {
        ch = wi_string_character_at_index(string, i);
        
        if(ch == '.')
            *isfloat = true;
        
        if(!isdigit(ch) && ch != '.' && ch != '-') {
            range.length = i;
            
            break;
        }
    }
    
    return wi_string_substring_with_range(string, range);
}



static wi_string_t * _wi_json_boolean_or_null_string_for_string(wi_string_t *string) {
    wi_range_t          range;
    
    range.location = 0;
    
    if(wi_string_has_prefix(string, WI_STR("t"))) {
        range.length = 4;
        
        if(!wi_is_equal(wi_string_substring_with_range(string, range), WI_STR("true")))
            return NULL;
    }
    else if(wi_string_has_prefix(string, WI_STR("f"))) {
        range.length = 5;
        
        if(!wi_is_equal(wi_string_substring_with_range(string, range), WI_STR("false")))
            return NULL;
    }
    else if(wi_string_has_prefix(string, WI_STR("null"))) {
        range.length = 4;
        
        if(!wi_is_equal(wi_string_substring_with_range(string, range), WI_STR("null")))
            return NULL;
    }
    
    return wi_string_substring_with_range(string, range);
}



#pragma mark -

static wi_string_t * _wi_json_string_for_dictionary(wi_dictionary_t *dictionary) {
    wi_mutable_array_t      *strings;
    wi_runtime_instance_t   *key, *value;
    wi_enumerator_t         *enumerator;
    
    strings = wi_mutable_array();
    enumerator = wi_dictionary_key_enumerator(dictionary);
    
    while((key = wi_enumerator_next_data(enumerator))) {
        if(wi_runtime_id(key) != wi_string_runtime_id()) {
            wi_error_set_libwired_error_with_format(WI_ERROR_JSON_WRITEFAILED,
                WI_STR("Dictionary keys that are not strings is not supported in JSON"));

            return NULL;
        }
        
        value = wi_dictionary_data_for_key(dictionary, key);
        
        wi_mutable_array_add_data(strings, _wi_json_string_for_key_and_value(key, value));
    }
    
    return wi_string_with_format(WI_STR("{%@}"), wi_array_components_joined_by_string(strings, WI_STR(", ")));
}



static wi_string_t * _wi_json_string_for_array(wi_array_t *array) {
    wi_mutable_array_t      *strings;
    wi_runtime_instance_t   *value;
    wi_enumerator_t         *enumerator;
    
    strings = wi_mutable_array();
    enumerator = wi_array_data_enumerator(array);
    
    while((value = wi_enumerator_next_data(enumerator)))
        wi_mutable_array_add_data(strings, _wi_json_string_for_value(value));
    
    return wi_string_with_format(WI_STR("[%@]"), wi_array_components_joined_by_string(strings, WI_STR(", ")));
}



static wi_string_t * _wi_json_string_for_key_and_value(wi_string_t *key, wi_runtime_instance_t *value) {
    return wi_string_with_format(WI_STR("\"%@\":%@"), key, _wi_json_string_for_value(value));
}



static wi_string_t * _wi_json_string_for_value(wi_runtime_instance_t *value) {
    if(wi_runtime_id(value) == wi_dictionary_runtime_id())
        return _wi_json_string_for_dictionary(value);
    else if(wi_runtime_id(value) == wi_array_runtime_id())
        return _wi_json_string_for_array(value);
    else if(wi_runtime_id(value) == wi_string_runtime_id())
        return _wi_json_string_for_string(value);
    else if(wi_runtime_id(value) == wi_number_runtime_id())
        return _wi_json_string_for_number(value);
    else if(wi_runtime_id(value) == wi_null_runtime_id())
        return _wi_json_string_for_null(value);
    
    wi_error_set_libwired_error_with_format(WI_ERROR_JSON_WRITEFAILED,
        WI_STR("Value of class %@ not supported in JSON"),
        wi_runtime_class_name(value));
    
    return NULL;
}



static wi_string_t * _wi_json_string_for_string(wi_string_t *value) {
    return wi_string_with_format(WI_STR("\"%@\""), value);
}



static wi_string_t * _wi_json_string_for_number(wi_number_t *value) {
    if(wi_number_type(value) == WI_NUMBER_BOOL) {
        if(wi_number_bool(value))
            return WI_STR("true");
        else
            return WI_STR("false");
    } else {
        return wi_number_string(value);
    }
}



static wi_string_t * _wi_json_string_for_null(wi_null_t *value) {
    return WI_STR("null");
}
