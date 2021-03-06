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

#ifndef WI_STRING_ENCODING

int wi_iconv_dummy = 0;

#else

#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>
#include <wired/wi-string-encoding.h>
#include <wired/wi-system.h>

#include <iconv.h>

struct _wi_string_encoding {
    wi_runtime_base_t                   base;
    
    wi_string_t                         *charset;
    wi_mutable_string_t                 *target_encoding;
    wi_mutable_string_t                 *utf8_encoding;

    wi_uinteger_t                       options;
};


static void                             _wi_string_encoding_dealloc(wi_runtime_instance_t *);
static wi_string_t *                    _wi_string_encoding_description(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_string_encoding_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_string_encoding_runtime_class = {
    "wi_string_encoding_t",
    _wi_string_encoding_dealloc,
    NULL,
    NULL,
    _wi_string_encoding_description,
    NULL
};


void wi_string_encoding_register(void) {
    _wi_string_encoding_runtime_id = wi_runtime_register_class(&_wi_string_encoding_runtime_class);
}



void wi_string_encoding_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_string_encoding_runtime_id(void) {
    return _wi_string_encoding_runtime_id;
}



#pragma mark -

wi_string_encoding_t * wi_string_encoding_with_charset(wi_string_t *charset, wi_string_encoding_options_t options) {
    return wi_autorelease(wi_string_encoding_init_with_charset(wi_string_encoding_alloc(), charset, options));
}



#pragma mark -

wi_string_encoding_t * wi_string_encoding_alloc(void) {
    return wi_runtime_create_instance(_wi_string_encoding_runtime_id, sizeof(wi_string_encoding_t));
}



wi_string_encoding_t * wi_string_encoding_init_with_charset(wi_string_encoding_t *encoding, wi_string_t *charset, wi_string_encoding_options_t options) {
    encoding->charset           = wi_copy(charset);
    encoding->target_encoding   = wi_mutable_copy(charset);
    encoding->utf8_encoding     = wi_string_init_with_utf8_string(wi_mutable_string_alloc(), "UTF-8");
    encoding->options           = options;
    
    if(options & WI_STRING_ENCODING_IGNORE) {
        wi_mutable_string_append_string(encoding->target_encoding, WI_STR("//IGNORE"));
        wi_mutable_string_append_string(encoding->utf8_encoding, WI_STR("//IGNORE"));
    }
    
    if(options & WI_STRING_ENCODING_TRANSLITERATE) {
        wi_mutable_string_append_string(encoding->target_encoding, WI_STR("//TRANSLIT"));
        wi_mutable_string_append_string(encoding->utf8_encoding, WI_STR("//TRANSLIT"));
    }
    
    return encoding;
}



static void _wi_string_encoding_dealloc(wi_runtime_instance_t *instance) {
    wi_string_encoding_t   *encoding = instance;
    
    wi_release(encoding->charset);
    wi_release(encoding->target_encoding);
    wi_release(encoding->utf8_encoding);
}



static wi_string_t * _wi_string_encoding_description(wi_runtime_instance_t *instance) {
    wi_string_encoding_t   *encoding = instance;
    
    return wi_string_with_format(WI_STR("<%@ %p>{encoding = %@}"),
        wi_runtime_class_name(encoding),
        encoding,
        encoding->target_encoding);
}



#pragma mark -

wi_string_t * wi_string_encoding_charset(wi_string_encoding_t *encoding) {
    return encoding->charset;
}



wi_string_encoding_options_t wi_string_encoding_options(wi_string_encoding_t *encoding) {
    return encoding->options;
}



#pragma mark -

wi_string_t * wi_string_encoding_utf8_string_from_data(wi_string_encoding_t *encoding, wi_data_t *data) {
    return wi_string_encoding_utf8_string_from_bytes(encoding, wi_data_bytes(data), wi_data_length(data));
}



wi_string_t * wi_string_encoding_utf8_string_from_bytes(wi_string_encoding_t *encoding, const char *buffer, wi_uinteger_t size) {
    wi_string_t     *string;
    char            *inbuffer, *outbuffer, *outbufferp;
    wi_uinteger_t   inbytes, outbytes;
    size_t          bytes, inbytesleft, outbytesleft;
    iconv_t         iconvd;
    
    iconvd = iconv_open(wi_string_utf8_string(encoding->utf8_encoding), wi_string_utf8_string(encoding->target_encoding));
    
    if(iconvd == (iconv_t) -1) {
        wi_error_set_errno(errno);
        
        return NULL;
    }
    
    inbytes = inbytesleft = size;
    outbytes = outbytesleft = size * 4;
    
    inbuffer = (char *) buffer;
    outbuffer = outbufferp = wi_malloc(outbytes);
    
    bytes = iconv(iconvd, &inbuffer, &inbytesleft, &outbuffer, &outbytesleft);
    
    if(bytes == (size_t) -1) {
        wi_free(outbufferp);
        iconv_close(iconvd);
        
        wi_error_set_errno(errno);
        
        return NULL;
    }

    string = wi_string_init_with_utf8_bytes(wi_string_alloc(), outbufferp, outbytes - outbytesleft);
    
    wi_free(outbufferp);
    iconv_close(iconvd);
    
    return wi_autorelease(string);
}



wi_data_t * wi_string_encoding_data_from_utf8_bytes(wi_string_encoding_t *encoding, const char *buffer, wi_uinteger_t size) {
    wi_data_t       *data;
    char            *inbuffer, *outbuffer, *outbufferp;
    wi_uinteger_t   inbytes, outbytes;
    size_t          bytes, inbytesleft, outbytesleft;
    iconv_t         iconvd;
    
    iconvd = iconv_open(wi_string_utf8_string(encoding->target_encoding), wi_string_utf8_string(encoding->utf8_encoding));
    
    if(iconvd == (iconv_t) -1) {
        wi_error_set_errno(errno);
        
        return NULL;
    }
    
    inbytes = inbytesleft = size;
    outbytes = outbytesleft = size * 4;
    
    inbuffer = (char *) buffer;
    outbuffer = outbufferp = wi_malloc(outbytes);
    
    bytes = iconv(iconvd, &inbuffer, &inbytesleft, &outbuffer, &outbytesleft);
    
    if(bytes == (size_t) -1) {
        wi_free(outbufferp);
        iconv_close(iconvd);
        
        wi_error_set_errno(errno);
        
        return NULL;
    }
    
    data = wi_data_init_with_bytes(wi_data_alloc(), outbufferp, outbytes - outbytesleft);
    
    wi_free(outbufferp);
    iconv_close(iconvd);
    
    return wi_autorelease(data);
}


#endif
