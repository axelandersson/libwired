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

WI_TEST_EXPORT void                     wi_test_error(void);


void wi_test_error(void) {
    wi_string_t     *string;
    wi_boolean_t    result;
    
    result = wi_filesystem_file_exists_at_path(WI_STR("/non/existing/path"), NULL);
    
    WI_TEST_ASSERT_FALSE(result, "");
    WI_TEST_ASSERT_EQUALS(wi_error_domain(), WI_ERROR_DOMAIN_ERRNO, "");
    WI_TEST_ASSERT_EQUALS(wi_error_code(), ENOENT, "");
    WI_TEST_ASSERT_EQUAL_INSTANCES(wi_error_string(), WI_STR("No such file or directory"), "");
    
    string = wi_string_with_format(WI_STR("%m"));

    WI_TEST_ASSERT_EQUAL_INSTANCES(string, WI_STR("No such file or directory"), "");
}
