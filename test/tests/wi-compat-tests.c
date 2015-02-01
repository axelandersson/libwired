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
#include <stdlib.h>
#include <string.h>

WI_TEST_EXPORT void                     wi_test_compat_strings_separating(void);
WI_TEST_EXPORT void                     wi_test_compat_strings_copy_and_concat(void);
WI_TEST_EXPORT void                     wi_test_compat_strings_searching(void);
WI_TEST_EXPORT void                     wi_test_compat_strings_formatting(void);
WI_TEST_EXPORT void                     wi_test_compat_tmpfile(void);
WI_TEST_EXPORT void                     wi_test_compat_timegm(void);


void wi_test_compat_strings_separating(void) {
    char            *s, *ss, *string;
    wi_uinteger_t   i = 0;

    s = ss = wi_strdup("foo.bar.baz");
    
    while((string = wi_strsep(&s, "."))) {
        if(i == 0)
            WI_TEST_ASSERT_EQUALS(strcmp(string, "foo"), 0, "");
        else if(i == 1)
            WI_TEST_ASSERT_EQUALS(strcmp(string, "bar"), 0, "");
        else if(i == 2)
            WI_TEST_ASSERT_EQUALS(strcmp(string, "baz"), 0, "");
        
        i++;
    }
    
    WI_TEST_ASSERT_NULL(s, "");
    WI_TEST_ASSERT_EQUALS(memcmp(ss, "foo\0bar\0baz", strlen("foo.bar.baz")), 0, "");
    
    free(ss);
}



void wi_test_compat_strings_copy_and_concat(void) {
    char    string[16];
    size_t  bytes;
    
    bytes = wi_strlcpy(string, "hello world hello world hello world hello world", sizeof(string));
    
    WI_TEST_ASSERT_EQUALS(strcmp(string, "hello world hel"), 0, "");
    WI_TEST_ASSERT_EQUALS(bytes, 47, "");
    
    bytes = wi_strlcpy(string, "hello world", sizeof(string));
    
    WI_TEST_ASSERT_EQUALS(strcmp(string, "hello world"), 0, "");
    WI_TEST_ASSERT_EQUALS(bytes, 11, "");
    
    bytes = wi_strlcat(string, "hello world", sizeof(string));
    
    WI_TEST_ASSERT_EQUALS(strcmp(string, "hello worldhell"), 0, "");
    WI_TEST_ASSERT_EQUALS(bytes, 22, "");
    
    bytes = wi_strlcat(string, "hello world", sizeof(string));

    WI_TEST_ASSERT_EQUALS(strcmp(string, "hello worldhell"), 0, "");
    WI_TEST_ASSERT_EQUALS(bytes, 26, "");
}



void wi_test_compat_strings_searching(void) {
    WI_TEST_ASSERT_NULL(wi_strcasestr("FOOBARFOO", "baz"), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strcasestr("FOOBARFOO", "bar"), "BARFOO"), 0, "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strcasestr("FOOBARFOO", "foo"), "FOOBARFOO"), 0, "");
    
    WI_TEST_ASSERT_NULL(wi_strncasestr("FOOBARFOO", "baz", 9), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strncasestr("FOOBARFOO", "bar", 9), "BARFOO"), 0, "");
    WI_TEST_ASSERT_NULL(wi_strncasestr("FOOBARFOO", "bar", 3), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strncasestr("FOOBARFOO", "foo", 9), "FOOBARFOO"), 0, "");

    WI_TEST_ASSERT_NULL(wi_strrnstr("foobarfoo", "baz", 9), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strrnstr("foobarfoo", "bar", 9), "barfoo"), 0, "");
    WI_TEST_ASSERT_NULL(wi_strrnstr("foobarfoo", "bar", 3), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strrnstr("foobarfoo", "foo", 9), "foo"), 0, "");
    
    WI_TEST_ASSERT_NULL(wi_strrncasestr("FOOBARFOO", "baz", 9), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strrncasestr("FOOBARFOO", "bar", 9), "BARFOO"), 0, "");
    WI_TEST_ASSERT_NULL(wi_strrncasestr("FOOBARFOO", "bar", 3), "");
    WI_TEST_ASSERT_EQUALS(strcmp(wi_strrncasestr("FOOBARFOO", "foo", 9), "FOO"), 0, "");
}



void wi_test_compat_strings_formatting(void) {
    char    *string;
    int     bytes;
    
    bytes = wi_asprintf(&string, "foo%s", "bar");

    WI_TEST_ASSERT_EQUALS(strcmp(string, "foobar"), 0, "");
    WI_TEST_ASSERT_EQUALS(bytes, 6, "");
    
    wi_free(string);
}



void wi_test_compat_tmpfile(void) {
    FILE    *fp;
    
    fp = wi_tmpfile();
    
    WI_TEST_ASSERT_NOT_NULL(fp, "");
    
    fclose(fp);
}



void wi_test_compat_timegm(void) {
    struct tm   tm;
    time_t      time;
    
    memset(&tm, 0, sizeof(struct tm));
    
    tm.tm_sec   = 0;
    tm.tm_min   = 0;
    tm.tm_hour  = 0;
    tm.tm_mday  = 1;
    tm.tm_mon   = 0;
    tm.tm_year  = 100;
    
    time = wi_timegm(&tm);
    
    WI_TEST_ASSERT_EQUALS(time, 946684800, "");
}
