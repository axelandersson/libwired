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

WI_TEST_EXPORT void                     wi_test_string_creation(void);
WI_TEST_EXPORT void                     wi_test_string_runtime_functions(void);
WI_TEST_EXPORT void                     wi_test_string_comparison(void);
WI_TEST_EXPORT void                     wi_test_string_constant(void);
WI_TEST_EXPORT void                     wi_test_string_formatting(void);
WI_TEST_EXPORT void                     wi_test_string_accessors(void);
WI_TEST_EXPORT void                     wi_test_string_appending(void);
WI_TEST_EXPORT void                     wi_test_string_inserting(void);
WI_TEST_EXPORT void                     wi_test_string_replacing(void);
WI_TEST_EXPORT void                     wi_test_string_deleting(void);
WI_TEST_EXPORT void                     wi_test_string_substrings(void);
WI_TEST_EXPORT void                     wi_test_string_splitting(void);
WI_TEST_EXPORT void                     wi_test_string_searching(void);
WI_TEST_EXPORT void                     wi_test_string_case(void);
WI_TEST_EXPORT void                     wi_test_string_paths(void);
WI_TEST_EXPORT void                     wi_test_string_conversion(void);
WI_TEST_EXPORT void                     wi_test_string_serialization(void);
WI_TEST_EXPORT void                     wi_test_string_mutation_setting(void);
WI_TEST_EXPORT void                     wi_test_string_mutation_appending(void);
WI_TEST_EXPORT void                     wi_test_string_mutation_inserting(void);
WI_TEST_EXPORT void                     wi_test_string_mutation_replacing(void);
WI_TEST_EXPORT void                     wi_test_string_mutation_deleting(void);
WI_TEST_EXPORT void                     wi_test_string_mutation_paths(void);

static wi_string_t *                    _wi_test_string_creation_with_arguments(wi_string_t *, ...);
static wi_string_t *                    _wi_test_string_appending_with_arguments(wi_string_t *, wi_string_t *, ...);
static void                             _wi_test_string_mutation_setting_with_arguments(wi_mutable_string_t *, wi_string_t *, ...);
static void                             _wi_test_string_mutation_appending_with_arguments(wi_mutable_string_t *, wi_string_t *, ...);


void wi_test_string_creation(void) {
    wi_string_t     *string;
    wi_data_t       *data;
    
    string = wi_string();
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 0U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), ""), 0, "");
    
    string = wi_string_with_utf8_string("hello world");
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
    
    string = wi_string_with_format(WI_STR("%s"), "hello world");

    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
    
    string = _wi_test_string_creation_with_arguments(WI_STR("%s"), "hello world");
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
    
    data = wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ="));
    string = wi_string_with_utf8_data(data);
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
    
    string = wi_string_with_utf8_bytes(wi_data_bytes(data), wi_data_length(data));
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
    
    string = wi_string_with_utf8_contents_of_file(WI_STR("/non/existing/file.txt"));
    
    WI_TEST_ASSERT_NULL(string, "");
    
    string = wi_string_with_utf8_contents_of_file(wi_string_by_appending_path_component(wi_test_fixture_path, WI_STR("wi-string-tests-1.txt")));
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 12U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world\n"), 0, "");
    
    string = wi_mutable_string();
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 0U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), ""), 0, "");
    
    string = wi_mutable_string_with_format(WI_STR("%s"), "hello world");
    
    WI_TEST_ASSERT_NOT_NULL(string, "");
    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
}



static wi_string_t * _wi_test_string_creation_with_arguments(wi_string_t *format, ...) {
    wi_string_t     *string;
    va_list         ap;
    
    va_start(ap, format);
    string = wi_string_with_format_and_arguments(format, ap);
    va_end(ap);
    
    return string;
}



void wi_test_string_runtime_functions(void) {
    wi_string_t             *string1;
    wi_mutable_string_t     *string2;
    
    string1 = wi_string_with_utf8_string("hello world");
    string2 = wi_autorelease(wi_mutable_copy(string1));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string1, string2, "");
    WI_TEST_ASSERT_EQUALS(wi_hash(string1), wi_hash(string2), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(string1), wi_string_runtime_id(), "");
    WI_TEST_ASSERT_EQUALS(wi_runtime_id(string2), wi_string_runtime_id(), "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(string1) & WI_RUNTIME_OPTION_IMMUTABLE, "");
    WI_TEST_ASSERT_TRUE(wi_runtime_options(string2) & WI_RUNTIME_OPTION_MUTABLE, "");
    
    wi_mutable_string_append_string(string2, WI_STR("hello world"));
    
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(string1, string2, "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_description(string1), WI_STR("hello world"), "");
}



void wi_test_string_comparison(void) {
    WI_TEST_ASSERT_EQUALS(wi_string_compare(WI_STR("hello world"), WI_STR("hello world")), 0, "");
    WI_TEST_ASSERT_EQUALS(wi_string_compare(WI_STR("HELLO WORLD"), WI_STR("hello world")), -32, "");
    WI_TEST_ASSERT_EQUALS(wi_string_case_insensitive_compare(WI_STR("HELLO WORLD"), WI_STR("hello world")), 0, "");
}



void wi_test_string_constant(void) {
    WI_TEST_ASSERT_EQUALS(WI_STR("hello world"), WI_STR("hello world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(WI_STR("hello world"), WI_STR("hello world"), "");
    WI_TEST_ASSERT_NOT_EQUALS(WI_STR("hello world"), WI_STR("hello another world"), "");
    WI_TEST_ASSERT_NOT_EQUAL_INSTANCES(WI_STR("hello world"), WI_STR("hello another world"), "");
}



void wi_test_string_formatting(void) {
    wi_string_t     *string;
    int             n;
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_format(WI_STR("'%@' '%@' '%#@'"), WI_STR("hello world"), NULL, NULL),
                                   WI_STR("'hello world' '(null)' ''"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_format(WI_STR("'%s' '%s' '%#s'"), "hello world", NULL, NULL),
                                   WI_STR("'hello world' '(null)' ''"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_format(WI_STR("'%d' '%u' '%p' '%.5f'"), -5, 5, 0xAC1DFEED, 3.1415926),
                                   WI_STR("'-5' '5' '0xac1dfeed' '3.14159'"), "");

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_format(WI_STR("'%@' '%n'"), WI_STR("hello world"), &n),
                                   WI_STR("'hello world' ''"), "");

    WI_TEST_ASSERT_EQUALS(n, 11, "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_format(WI_STR("'%*s'"), 15, "hello world"),
                                   WI_STR("'    hello world'"), "");

    string = wi_error_string();

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_with_format(WI_STR("'%m'")), wi_string_with_format(WI_STR("'%@'"), string), "");
}



void wi_test_string_accessors(void) {
    wi_string_t     *string;
    
    string = wi_string_with_utf8_string("hello world");

    WI_TEST_ASSERT_EQUALS(wi_string_length(string), 11U, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_string_utf8_string(string), "hello world"), 0, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_utf8_data(string), wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ=")), "");
    WI_TEST_ASSERT_EQUALS(wi_string_character_at_index(string, 0), 'h', "");
    WI_TEST_ASSERT_EQUALS(wi_string_character_at_index(string, 6), 'w', "");
}



void wi_test_string_appending(void) {
    wi_data_t   *data;
    
    data = wi_data_with_base64_string(WI_STR("aGVsbG8gd29ybGQ="));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_appending_string(WI_STR("hello world"), WI_STR("hello world")),
                                   WI_STR("hello worldhello world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_appending_format(WI_STR("hello world"), WI_STR("%s"), "hello world"),
                                   WI_STR("hello worldhello world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(_wi_test_string_appending_with_arguments(WI_STR("hello world"), WI_STR("%s"), "hello world"),
                                   WI_STR("hello worldhello world"), "");
}



static wi_string_t * _wi_test_string_appending_with_arguments(wi_string_t *string, wi_string_t *format, ...) {
    wi_string_t     *newstring;
    va_list         ap;
    
    va_start(ap, format);
    newstring = wi_string_by_appending_format_and_arguments(string, format, ap);
    va_end(ap);
    
    return newstring;
}



void wi_test_string_inserting(void) {
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_inserting_string_at_index(WI_STR("hello world"), WI_STR("hello world"), 0),
                                   WI_STR("hello worldhello world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_inserting_string_at_index(WI_STR("hello world"), WI_STR("hello world"), 6),
                                   WI_STR("hello hello worldworld"), "");
}



void wi_test_string_replacing(void) {
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_replacing_characters_in_range_with_string(WI_STR("hello world"), wi_make_range(6, 5), WI_STR("WORLD")),
                                   WI_STR("hello WORLD"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_replacing_string_with_string(WI_STR("hello world"), WI_STR("world"), WI_STR("WORLD"), 0),
                                   WI_STR("hello WORLD"), "");
}



void wi_test_string_deleting(void) {
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_characters_in_range(WI_STR("hello world"), wi_make_range(5, 6)),
                                   WI_STR("hello"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_characters_from_index(WI_STR("hello world"), 5),
                                   WI_STR("hello"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_characters_to_index(WI_STR("hello world"), 6),
                                   WI_STR("world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_surrounding_whitespace(WI_STR("   hello world   ")),
                                   WI_STR("hello world"), "");
}



void wi_test_string_substrings(void) {
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_substring_with_range(WI_STR("hello world"), wi_make_range(6, 5)),
                                   WI_STR("world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_substring_from_index(WI_STR("hello world"), 6),
                                   WI_STR("world"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_substring_to_index(WI_STR("hello world"), 5),
                                   WI_STR("hello"), "");
}




void wi_test_string_splitting(void) {
    wi_array_t  *array;
    
    array = wi_string_components_separated_by_string(WI_STR("foo.bar.baz"), WI_STR("."));
    
    WI_TEST_ASSERT_EQUALS(wi_array_count(array), 3U, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 0), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 1), WI_STR("bar"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_array_data_at_index(array, 2), WI_STR("baz"), "");
}



void wi_test_string_searching(void) {
    wi_range_t  range;
    
    range = wi_string_range_of_string(WI_STR("hello world"), WI_STR("hello"), 0);
    
    WI_TEST_ASSERT_EQUALS(range.location, 0U, "");
    WI_TEST_ASSERT_EQUALS(range.length, 5U, "");

    range = wi_string_range_of_string_in_range(WI_STR("hello world"), WI_STR("hello"), 0, wi_make_range(6, 5));
    
    WI_TEST_ASSERT_EQUALS(range.location, WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUALS(range.length, 0U, "");
    
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string(WI_STR("hello world"), WI_STR("world"), 0), 6U, "");
    
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string_in_range(WI_STR("hello world"), WI_STR("hello"), 0, wi_make_range(6, 5)), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string_in_range(WI_STR("hello world"), WI_STR("HELLO"), WI_STRING_CASE_INSENSITIVE, wi_make_range(0, 11)), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string_in_range(WI_STR("hello world"), WI_STR("HELLO"), WI_STRING_SMART_CASE_INSENSITIVE, wi_make_range(0, 11)), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string_in_range(WI_STR("hello world hello"), WI_STR("hello"), WI_STRING_BACKWARDS, wi_make_range(0, 17)), 12U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_string_in_range(WI_STR("hello world hello"), WI_STR("HELLO"), WI_STRING_BACKWARDS | WI_STRING_CASE_INSENSITIVE, wi_make_range(0, 17)), 12U, "");

    WI_TEST_ASSERT_EQUALS(wi_string_index_of_char(WI_STR("hello world"), 'h', 0), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_char(WI_STR("hello world"), 'H', 0), WI_NOT_FOUND, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_char(WI_STR("hello world"), 'H', WI_STRING_CASE_INSENSITIVE), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_index_of_char(WI_STR("hello world"), 'H', WI_STRING_SMART_CASE_INSENSITIVE), WI_NOT_FOUND, "");

    WI_TEST_ASSERT_TRUE(wi_string_contains_string(WI_STR("hello world"), WI_STR("hello"), 0), "");
    WI_TEST_ASSERT_FALSE(wi_string_contains_string(WI_STR("hello world"), WI_STR("foo"), 0), "");
    
    WI_TEST_ASSERT_TRUE(wi_string_has_prefix(WI_STR("hello world"), WI_STR("hello")), "");
    WI_TEST_ASSERT_FALSE(wi_string_has_prefix(WI_STR("hello world"), WI_STR("foo")), "");
    
    WI_TEST_ASSERT_TRUE(wi_string_has_suffix(WI_STR("hello world"), WI_STR("world")), "");
    WI_TEST_ASSERT_FALSE(wi_string_has_suffix(WI_STR("hello world"), WI_STR("foo")), "");
}



void wi_test_string_case(void) {
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_lowercase_string(WI_STR("FOO")), WI_STR("foo"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_uppercase_string(WI_STR("bar")), WI_STR("BAR"), "");
}



void wi_test_string_paths(void) {
    wi_string_t     *path;
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_path_components(WI_STR("/usr/local/wired")),
                                   wi_array_with_data(WI_STR("/"), WI_STR("usr"), WI_STR("local"), WI_STR("wired"), NULL), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_normalizing_path(WI_STR("/")), WI_STR("/"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_normalizing_path(WI_STR("////usr/././local/../local/../local/wired///")),
                                   WI_STR("/usr/local/wired"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_resolving_symbolic_links_in_path(WI_STR("/")), WI_STR("/"), "");
    
    path = wi_string_by_expanding_tilde_in_path(WI_STR("~/wired"));
    
    WI_TEST_ASSERT_TRUE(wi_string_length(path) > wi_string_length(WI_STR("~/wired")), "");
    WI_TEST_ASSERT_TRUE(wi_string_has_prefix(path, WI_STR("/")), "");
    
    path = wi_string_by_expanding_tilde_in_path(WI_STR("~root/wired"));
    
    WI_TEST_ASSERT_TRUE(wi_string_length(path) >= wi_string_length(WI_STR("~root/wired")), "");
    WI_TEST_ASSERT_TRUE(wi_string_has_prefix(path, WI_STR("/")), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_appending_path_component(WI_STR(""), WI_STR("wired")),
                                   WI_STR("wired"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_appending_path_component(WI_STR("/usr/local/"), WI_STR("/wired")),
                                   WI_STR("/usr/local/wired"), "");

    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_appending_path_components(WI_STR("/usr/local"), wi_array_with_data(WI_STR("wired"), NULL)),
                                   WI_STR("/usr/local/wired"), "");
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_last_path_component(WI_STR("/usr/local/wired/")), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_last_path_component(WI_STR("/")), WI_STR("/"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_last_path_component(WI_STR("/wired/")), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_last_path_component(WI_STR("wired")), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_last_path_component(WI_STR("/")), WI_STR("/"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_last_path_component(WI_STR("/usr/local/wired")), WI_STR("/usr/local"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_last_path_component(WI_STR("/usr/local/wired/")), WI_STR("/usr/local"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_appending_path_extension(WI_STR("wired"), WI_STR("c")), WI_STR("wired.c"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_path_extension(WI_STR("wired.c")), WI_STR("c"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_path_extension(WI_STR("wired")), WI_STR(""), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_path_extension(WI_STR("wired.c")), WI_STR("wired"), "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_string_by_deleting_path_extension(WI_STR("wired")), WI_STR("wired"), "");
}



void wi_test_string_conversion(void) {
    WI_TEST_ASSERT_EQUALS(wi_string_bool(WI_STR("yes")), true, "");
    WI_TEST_ASSERT_EQUALS(wi_string_bool(WI_STR("no")), false, "");
    WI_TEST_ASSERT_EQUALS(wi_string_int32(WI_STR("2147483647")), 2147483647, "");
    WI_TEST_ASSERT_EQUALS(wi_string_int32(WI_STR("2147483648")), 0, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uint32(WI_STR("4294967295")), 4294967295U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uint32(WI_STR("4294967296")), 0U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_int64(WI_STR("9223372036854775807")), 9223372036854775807LL, "");
    WI_TEST_ASSERT_EQUALS(wi_string_int64(WI_STR("9223372036854775808")), 0LL, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uint64(WI_STR("18446744073709551615")), 18446744073709551615ULL, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uint64(WI_STR("18446744073709551616")), 0ULL, "");
    
#ifdef WI_32
    WI_TEST_ASSERT_EQUALS(wi_string_integer(WI_STR("2147483647")), 2147483647, "");
    WI_TEST_ASSERT_EQUALS(wi_string_integer(WI_STR("2147483648")), 0, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uinteger(WI_STR("4294967295")), 4294967295U, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uinteger(WI_STR("4294967296")), 0U, "");
#else
    WI_TEST_ASSERT_EQUALS(wi_string_integer(WI_STR("9223372036854775807")), 9223372036854775807LL, "");
    WI_TEST_ASSERT_EQUALS(wi_string_integer(WI_STR("9223372036854775808")), 0LL, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uinteger(WI_STR("18446744073709551615")), 18446744073709551615ULL, "");
    WI_TEST_ASSERT_EQUALS(wi_string_uinteger(WI_STR("18446744073709551616")), 0ULL, "");
#endif
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_string_float(WI_STR("3.40282346e38")), 3.40282346e38F, 0.0001, "");
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_string_float(WI_STR("3.40282347e38")), 0.0F, 0.0001, "");
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_string_double(WI_STR("1.7976931348623155e308")), 1.7976931348623155e308, 0.0001, "");
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(wi_string_double(WI_STR("1.7976931348623160e308")), 0.0, 0.0001, "");
}



void wi_test_string_serialization(void) {
    wi_string_t     *string1, *string2, *path;
    
    string1 = wi_string_with_utf8_string("hello world");
    
    WI_TEST_ASSERT_FALSE(wi_string_write_utf8_string_to_path(string1, WI_STR("/non/existing/file.string")), "");
    
    path = wi_filesystem_temporary_path_with_template(WI_STR("/tmp/libwired-test-string.XXXXXXX"));
    
    WI_TEST_ASSERT_TRUE(wi_string_write_utf8_string_to_path(string1, path), "");
    
    string2 = wi_string_with_utf8_contents_of_file(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string1, string2, "");
    
    wi_filesystem_delete_path(path);
}



void wi_test_string_mutation_setting(void) {
    wi_mutable_string_t     *string;
    
    string = wi_mutable_string();
    
    wi_mutable_string_set_string(string, WI_STR("hello world 1"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 1"), "");
    
    wi_mutable_string_set_format(string, WI_STR("hello world %d"), 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 2"), "");
    
    _wi_test_string_mutation_setting_with_arguments(string, WI_STR("hello world %d"), 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 3"), "");
}



static void _wi_test_string_mutation_setting_with_arguments(wi_mutable_string_t *string, wi_string_t *format, ...) {
    va_list     ap;
    
    va_start(ap, format);
    wi_mutable_string_set_format_and_arguments(string, format, ap);
    va_end(ap);
}



void wi_test_string_mutation_appending(void) {
    wi_mutable_string_t     *string;
    wi_data_t               *data;
    
    string = wi_mutable_string();
    
    wi_mutable_string_append_string(string, WI_STR("hello world 1"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 1"), "");
    
    wi_mutable_string_append_format(string, WI_STR("hello world %d"), 2);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 1hello world 2"), "");
    
    _wi_test_string_mutation_appending_with_arguments(string, WI_STR("hello world %d"), 3);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 1hello world 2hello world 3"), "");
}



static void _wi_test_string_mutation_appending_with_arguments(wi_mutable_string_t *string, wi_string_t *format, ...) {
    va_list     ap;
    
    va_start(ap, format);
    wi_mutable_string_append_format_and_arguments(string, format, ap);
    va_end(ap);
}



void wi_test_string_mutation_inserting(void) {
    wi_mutable_string_t     *string;
    
    string = wi_mutable_string();
    
    wi_mutable_string_insert_string_at_index(string, WI_STR("hello world 1"), 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 1"), "");
    
    wi_mutable_string_insert_string_at_index(string, WI_STR("hello world 2"), 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world 2hello world 1"), "");
}



void wi_test_string_mutation_replacing(void) {
    wi_mutable_string_t     *string;
    
    string = wi_mutable_string_with_format(WI_STR("hello world"));
    
    wi_mutable_string_replace_characters_in_range_with_string(string, wi_make_range(0, 5), WI_STR("HELLO"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("HELLO world"), "");
    
    string = wi_mutable_string_with_format(WI_STR("hello world"));
    
    wi_mutable_string_replace_string_with_string(string, WI_STR("world"), WI_STR("WORLD"), 0);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello WORLD"), "");
}



void wi_test_string_mutation_deleting(void) {
    wi_mutable_string_t     *string;
    
    string = wi_mutable_string_with_format(WI_STR("hello world"));
    
    wi_mutable_string_delete_characters_in_range(string, wi_make_range(0, 6));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("world"), "");
    
    string = wi_mutable_string_with_format(WI_STR("hello world"));
    
    wi_mutable_string_delete_characters_from_index(string, 5);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello"), "");
    
    string = wi_mutable_string_with_format(WI_STR("hello world"));
    
    wi_mutable_string_delete_characters_to_index(string, 6);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("world"), "");
    
    string = wi_mutable_string_with_format(WI_STR("   hello world   "));
    
    wi_mutable_string_delete_surrounding_whitespace(string);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("hello world"), "");
}



void wi_test_string_mutation_paths(void) {
    wi_mutable_string_t     *path;
    
    path = wi_mutable_string_with_format(WI_STR("/"));
    
    wi_mutable_string_normalize_path(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/"), "");
    
    wi_mutable_string_resolve_symbolic_links_in_path(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/"), "");
    
    path = wi_mutable_string_with_format(WI_STR("////usr/././local/../local/../local/wired///"));
    
    wi_mutable_string_normalize_path(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/usr/local/wired"), "");
    
    path = wi_mutable_string_with_format(WI_STR("~/wired"));

    wi_mutable_string_expand_tilde_in_path(path);
    
    WI_TEST_ASSERT_TRUE(wi_string_length(path) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_string_has_prefix(path, WI_STR("/")), "");
    
    path = wi_mutable_string_with_format(WI_STR("~root/wired"));
    
    wi_mutable_string_expand_tilde_in_path(path);
    
    WI_TEST_ASSERT_TRUE(wi_string_length(path) > 0, "");
    WI_TEST_ASSERT_TRUE(wi_string_has_prefix(path, WI_STR("/")), "");
    
    path = wi_mutable_string_with_format(WI_STR(""));
    
    wi_mutable_string_append_path_component(path, WI_STR("wired"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("wired"), "");
    
    path = wi_mutable_string_with_format(WI_STR("/usr/local/"));
    
    wi_mutable_string_append_path_component(path, WI_STR("/wired"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/usr/local/wired"), "");
    
    path = wi_mutable_string_with_format(WI_STR("/usr/local/"));
    
    wi_mutable_string_append_path_components(path, wi_array_with_data(WI_STR("wired"), NULL));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/usr/local/wired"), "");
    
    path = wi_mutable_string_with_format(WI_STR("/"));
    
    wi_mutable_string_delete_last_path_component(path);

    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/"), "");
    
    path = wi_mutable_string_with_format(WI_STR("/usr/local/wired"));
    
    wi_mutable_string_delete_last_path_component(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/usr/local"), "");
    
    path = wi_mutable_string_with_format(WI_STR("/usr/local/wired/"));
    
    wi_mutable_string_delete_last_path_component(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("/usr/local"), "");
    
    path = wi_mutable_string_with_format(WI_STR("wired"));
    
    wi_mutable_string_append_path_extension(path, WI_STR("c"));
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("wired.c"), "");
    
    path = wi_mutable_string_with_format(WI_STR("wired.c"));
    
    wi_mutable_string_delete_path_extension(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("wired"), "");
    
    path = wi_mutable_string_with_format(WI_STR("wired"));
    
    wi_mutable_string_delete_path_extension(path);
    
    WI_TEST_ASSERT_EQUAL_INSTANCES(path, WI_STR("wired"), "");
}
