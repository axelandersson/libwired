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
#include <math.h>

WI_TEST_EXPORT void                     wi_test_byteorder(void);


static void                             _wi_test_byteorder_value(char *, double);


void wi_test_byteorder(void) {
    char    *buffer;
    double  value;
    
    buffer = wi_malloc(32);
    
    _wi_test_byteorder_value(buffer, 0.0);
    _wi_test_byteorder_value(buffer, -0.0);

    _wi_test_byteorder_value(buffer, SCHAR_MAX);
    _wi_test_byteorder_value(buffer, SCHAR_MIN);
    _wi_test_byteorder_value(buffer, UCHAR_MAX);
    _wi_test_byteorder_value(buffer, CHAR_MAX);
    _wi_test_byteorder_value(buffer, CHAR_MIN);
    _wi_test_byteorder_value(buffer, USHRT_MAX);
    _wi_test_byteorder_value(buffer, SHRT_MAX);
    _wi_test_byteorder_value(buffer, SHRT_MIN);
    _wi_test_byteorder_value(buffer, UINT_MAX);
    _wi_test_byteorder_value(buffer, INT_MAX);
    _wi_test_byteorder_value(buffer, INT_MIN);
    _wi_test_byteorder_value(buffer, ULONG_MAX);
    _wi_test_byteorder_value(buffer, LONG_MAX);
    _wi_test_byteorder_value(buffer, LONG_MIN);
    _wi_test_byteorder_value(buffer, ULLONG_MAX);
    _wi_test_byteorder_value(buffer, LLONG_MAX);
    _wi_test_byteorder_value(buffer, LLONG_MIN);

    _wi_test_byteorder_value(buffer, M_E);
    _wi_test_byteorder_value(buffer, -M_E);
    _wi_test_byteorder_value(buffer, M_LOG2E);
    _wi_test_byteorder_value(buffer, -M_LOG2E);
    _wi_test_byteorder_value(buffer, M_LOG10E);
    _wi_test_byteorder_value(buffer, -M_LOG10E);
    _wi_test_byteorder_value(buffer, M_LN2);
    _wi_test_byteorder_value(buffer, -M_LN2);
    _wi_test_byteorder_value(buffer, M_LN10);
    _wi_test_byteorder_value(buffer, -M_LN10);
    _wi_test_byteorder_value(buffer, M_PI);
    _wi_test_byteorder_value(buffer, -M_PI);
    _wi_test_byteorder_value(buffer, M_PI_2);
    _wi_test_byteorder_value(buffer, -M_PI_2);
    _wi_test_byteorder_value(buffer, M_PI_4);
    _wi_test_byteorder_value(buffer, -M_PI_4);
    _wi_test_byteorder_value(buffer, M_1_PI);
    _wi_test_byteorder_value(buffer, -M_1_PI);
    _wi_test_byteorder_value(buffer, M_2_PI);
    _wi_test_byteorder_value(buffer, -M_2_PI);
    _wi_test_byteorder_value(buffer, M_2_SQRTPI);
    _wi_test_byteorder_value(buffer, -M_2_SQRTPI);
    _wi_test_byteorder_value(buffer, M_SQRT2);
    _wi_test_byteorder_value(buffer, -M_SQRT2);
    _wi_test_byteorder_value(buffer, M_SQRT1_2);
    _wi_test_byteorder_value(buffer, -M_SQRT1_2);

    wi_free(buffer);
}



static void _wi_test_byteorder_value(char *buffer, double value) {
    double  result;
    
    wi_write_double_to_ieee754(buffer, 0, value);
    
    result = wi_read_double_from_ieee754(buffer, 0);
    
    WI_TEST_ASSERT_EQUALS_WITH_ACCURACY(value, result, 0.000000000000000001, "");
}
