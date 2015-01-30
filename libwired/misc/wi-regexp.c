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

#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>

#include <wired/wi-assert.h>
#include <wired/wi-compat.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-regexp.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

#define WI_REGEXP_MAX_MATCH_COUNT       64


struct _wi_regexp {
    wi_runtime_base_t                   base;
    
    wi_string_t                         *pattern;
    wi_regexp_options_t                 options;
    
    regex_t                             regex;
    wi_boolean_t                        compiled;
};


static void                             _wi_regexp_dealloc(wi_runtime_instance_t *);
static wi_runtime_instance_t *          _wi_regexp_copy(wi_runtime_instance_t *);
static wi_boolean_t                     _wi_regexp_is_equal(wi_runtime_instance_t *, wi_runtime_instance_t *);
static wi_string_t *                    _wi_regexp_description(wi_runtime_instance_t *);
static wi_hash_code_t                   _wi_regexp_hash(wi_runtime_instance_t *);

static wi_boolean_t                     _wi_regexp_compile(wi_regexp_t *);


static wi_runtime_id_t                  _wi_regexp_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_regexp_runtime_class = {
    "wi_regexp_t",
    _wi_regexp_dealloc,
    _wi_regexp_copy,
    _wi_regexp_is_equal,
    _wi_regexp_description,
    _wi_regexp_hash
};



void wi_regexp_register(void) {
    _wi_regexp_runtime_id = wi_runtime_register_class(&_wi_regexp_runtime_class);
}



void wi_regexp_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_regexp_runtime_id(void) {
    return _wi_regexp_runtime_id;
}



#pragma mark -

wi_regexp_t * wi_regexp_with_pattern(wi_string_t *pattern, wi_regexp_options_t options) {
    return wi_autorelease(wi_regexp_init_with_pattern(wi_regexp_alloc(), pattern, options));
}



#pragma mark -

wi_regexp_t * wi_regexp_alloc(void) {
    return wi_runtime_create_instance(_wi_regexp_runtime_id, sizeof(wi_regexp_t));
}



wi_regexp_t * wi_regexp_init_with_pattern(wi_regexp_t *regexp, wi_string_t *pattern, wi_regexp_options_t options) {
    regexp->pattern = wi_copy(pattern);
    regexp->options = options;
    
    if(!_wi_regexp_compile(regexp)) {
        wi_release(regexp);
        
        return NULL;
    }
    
    return regexp;
}



static void _wi_regexp_dealloc(wi_runtime_instance_t *instance) {
    wi_regexp_t     *regexp = instance;
    
    if(regexp->compiled)
        regfree(&regexp->regex);

    wi_release(regexp->pattern);
}



static wi_runtime_instance_t * _wi_regexp_copy(wi_runtime_instance_t *instance) {
    wi_regexp_t     *regexp = instance;
    
    return wi_regexp_init_with_pattern(wi_regexp_alloc(), regexp->pattern, regexp->options);
}



static wi_boolean_t _wi_regexp_is_equal(wi_runtime_instance_t *instance1, wi_runtime_instance_t *instance2) {
    wi_regexp_t     *regexp1 = instance1;
    wi_regexp_t     *regexp2 = instance2;

    return wi_is_equal(regexp1->pattern, regexp2->pattern) && (regexp1->options == regexp2->options);
}



static wi_string_t * _wi_regexp_description(wi_runtime_instance_t *instance) {
    wi_regexp_t     *regexp = instance;
    
    return wi_string_with_format(WI_STR("<%@ %p>{pattern = %@}"),
                                 wi_runtime_class_name(regexp),
                                 regexp,
                                 regexp->pattern);
}



static wi_hash_code_t _wi_regexp_hash(wi_runtime_instance_t *instance) {
    wi_regexp_t     *regexp = instance;
    
    return wi_hash(regexp->pattern) + regexp->options;
}



#pragma mark -

static wi_boolean_t _wi_regexp_compile(wi_regexp_t *regexp) {
    int     options, error;
    
    options = REG_EXTENDED;
    
    if(regexp->options & WI_REGEXP_CASE_INSENSITIVE)
        options |= REG_ICASE;
    
    if(regexp->options & WI_REGEXP_NEWLINE_SENSITIVE)
        options |= REG_NEWLINE;
    
    error = regcomp(&regexp->regex, wi_string_utf8_string(regexp->pattern), options);
    
    if(error != 0) {
        wi_error_set_regex_error(&regexp->regex, error);
        
        return false;
    }
    
    regexp->compiled = true;
    
    return true;
}



#pragma mark -

wi_string_t * wi_regexp_pattern(wi_regexp_t *regexp) {
    return regexp->pattern;
}



wi_regexp_options_t wi_regexp_options(wi_regexp_t *regexp) {
    return regexp->options;
}



wi_uinteger_t wi_regexp_number_of_capture_groups(wi_regexp_t *regexp) {
    return regexp->regex.re_nsub;
}



#pragma mark -

wi_uinteger_t wi_regexp_number_of_matches_in_string(wi_regexp_t *regexp, wi_string_t *string) {
    return wi_regexp_get_matches_in_string(regexp, string, NULL, 0);
}



wi_range_t wi_regexp_range_of_first_match_in_string(wi_regexp_t *regexp, wi_string_t *string) {
    wi_regexp_match_t   matches[1];
    
    wi_regexp_get_matches_in_string(regexp, string, matches, 1);
    
    return matches[0].range;
}



wi_string_t * wi_regexp_string_of_first_match_in_string(wi_regexp_t *regexp, wi_string_t *string) {
    wi_regexp_match_t   matches[1];
    
    wi_regexp_get_matches_in_string(regexp, string, matches, 1);
    
    if(matches[0].range.location == WI_NOT_FOUND)
        return NULL;
    
    return wi_string_substring_with_range(string, matches[0].range);
}



wi_uinteger_t wi_regexp_get_matches_in_string(wi_regexp_t *regexp, wi_string_t *string, wi_regexp_match_t *matches, wi_uinteger_t size) {
    const char          *utf8_string;
    wi_regexp_match_t   match;
    regmatch_t          regmatch, regmatches[WI_REGEXP_MAX_MATCH_COUNT];
    wi_uinteger_t       i, count, offset;
    int                 error;
    
    utf8_string = wi_string_utf8_string(string);
    count = 0;
    offset = 0;
    
    while(strlen(utf8_string + offset) > 0) {
        error = regexec(&regexp->regex, utf8_string + offset, WI_REGEXP_MAX_MATCH_COUNT, regmatches, 0);
        
        if(error != 0) {
            wi_error_set_regex_error(&regexp->regex, error);
            
            break;
        }
        
        for(i = 0; i < WI_REGEXP_MAX_MATCH_COUNT; i++) {
            regmatch = regmatches[i];
            
            if(regmatch.rm_so == -1 || regmatch.rm_eo == -1)
                break;
            
            if(matches && count < size)
                matches[count].range = wi_make_range(regmatch.rm_so + offset, regmatch.rm_eo - regmatch.rm_so);
            
            count++;
        }
        
        offset += regmatches[0].rm_eo;
    }
    
    if(matches) {
        for(i = count; i < size; i++)
            matches[i].range = wi_make_range(WI_NOT_FOUND, 0);
    }
    
    return count;
}



#pragma mark -

wi_uinteger_t wi_regexp_replace_matches_in_string(wi_regexp_t *regexp, wi_mutable_string_t *string, wi_string_t *template) {
    wi_string_t         *originalstring, *substring;
    wi_regexp_match_t   matches[WI_REGEXP_MAX_MATCH_COUNT];
    wi_range_t          range;
    wi_uinteger_t       count, replacements;
    wi_integer_t        i;
    
    replacements = 0;
    
    if(wi_regexp_number_of_capture_groups(regexp) > 0) {
        originalstring = wi_autorelease(wi_copy(string));
        
        wi_mutable_string_set_string(string, template);
        
        count = wi_regexp_get_matches_in_string(regexp, originalstring, matches, WI_REGEXP_MAX_MATCH_COUNT);
        
        for(i = count - 1; i >= 0; i--) {
            substring = wi_string_substring_with_range(originalstring, matches[i].range);
            range = wi_string_range_of_string(string, wi_string_with_format(WI_STR("$%u"), i), 0);
            
            if(range.location != WI_NOT_FOUND) {
                wi_mutable_string_replace_characters_in_range_with_string(string, range, substring);
                
                replacements++;
            }
        }
    } else {
        count = wi_regexp_get_matches_in_string(regexp, string, matches, WI_REGEXP_MAX_MATCH_COUNT);
        
        for(i = count - 1; i >= 0; i--) {
            wi_mutable_string_replace_characters_in_range_with_string(string, matches[i].range, template);
            
            replacements++;
        }
    }
    
    return replacements;
}



wi_string_t * wi_regexp_string_by_replacing_matches_in_string(wi_regexp_t *regexp, wi_string_t *string, wi_string_t *template) {
    wi_mutable_string_t     *newstring;
    
    newstring = wi_mutable_copy(string);
    
    wi_regexp_replace_matches_in_string(regexp, newstring, template);
    
    wi_runtime_make_immutable(newstring);
    
    return wi_autorelease(newstring);
}
