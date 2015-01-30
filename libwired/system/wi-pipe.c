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

#include <sys/param.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>

#include <wired/wi-assert.h>
#include <wired/wi-pipe.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>

#define _WI_PIPE_ASSERT_OPEN(pipe) \
    WI_ASSERT((pipe)->rd >= 0 && (pipe)->wd >= 0, "%@ is not open", (pipe))


struct _wi_pipe {
    wi_runtime_base_t                   base;

    int                                 rd, wd;
};


static void                             _wi_pipe_dealloc(wi_runtime_instance_t *);
static wi_string_t *                    _wi_pipe_description(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_pipe_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_pipe_runtime_class = {
    "wi_pipe_t",
    _wi_pipe_dealloc,
    NULL,
    NULL,
    _wi_pipe_description,
    NULL
};



void wi_pipe_register(void) {
    _wi_pipe_runtime_id = wi_runtime_register_class(&_wi_pipe_runtime_class);
}



void wi_pipe_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_pipe_runtime_id(void) {
    return _wi_pipe_runtime_id;
}



#pragma mark -

wi_pipe_t * wi_pipe(void) {
    return wi_autorelease(wi_pipe_init(wi_pipe_alloc()));
}



#pragma mark -

wi_pipe_t * wi_pipe_alloc(void) {
    return wi_runtime_create_instance(_wi_pipe_runtime_id, sizeof(wi_pipe_t));
}



wi_pipe_t * wi_pipe_init(wi_pipe_t *pipe_) {
    int     fds[2];
    
    if(pipe(fds) < 0) {
        wi_error_set_errno(errno);
        
        wi_release(pipe_);
        
        return NULL;
    }
    
    pipe_->rd = fds[0];
    pipe_->wd = fds[1];
    
    return pipe_;
}



static void _wi_pipe_dealloc(wi_runtime_instance_t *instance) {
    wi_pipe_t   *pipe = instance;
    
    wi_pipe_close(pipe);
}



static wi_string_t * _wi_pipe_description(wi_runtime_instance_t *instance) {
    wi_pipe_t   *pipe = instance;
    
    return wi_string_with_format(WI_STR("<%@ %p>{read descriptor = %d, write descriptor = %d}"),
      wi_runtime_class_name(pipe),
      pipe,
      pipe->rd,
      pipe->wd);
}



#pragma mark -

int wi_pipe_descriptor_for_reading(wi_pipe_t *pipe) {
    return pipe->rd;
}



int wi_pipe_descriptor_for_writing(wi_pipe_t *pipe) {
    return pipe->wd;
}



#pragma mark -

wi_data_t * wi_pipe_read(wi_pipe_t *pipe, wi_uinteger_t length) {
    wi_mutable_data_t   *data;
    char                buffer[WI_PIPE_BUFFER_SIZE];
    wi_integer_t        bytes = -1;
    
    _WI_PIPE_ASSERT_OPEN(pipe);
    
    data = wi_data_init_with_capacity(wi_mutable_data_alloc(), length);
    
    while(length > 0) {
        bytes = wi_pipe_read_bytes(pipe, buffer, WI_MIN(sizeof(buffer), length));
        
        if(bytes <= 0)
            break;
        
        wi_mutable_data_append_bytes(data, buffer, bytes);
        
        length -= bytes;
    }
    
    if(bytes < 0) {
        wi_release(data);
        
        data = NULL;
    }
    
    wi_runtime_make_immutable(data);
    
    return wi_autorelease(data);
}



wi_data_t * wi_pipe_read_to_end_of_pipe(wi_pipe_t *pipe) {
    wi_mutable_data_t   *data;
    char                buffer[WI_PIPE_BUFFER_SIZE];
    wi_integer_t        bytes;
    
    _WI_PIPE_ASSERT_OPEN(pipe);
    
    data = wi_data_init_with_capacity(wi_mutable_data_alloc(), WI_PIPE_BUFFER_SIZE);
    
    while(true) {
        bytes = wi_pipe_read_bytes(pipe, buffer, sizeof(buffer));
        
        if(bytes <= 0)
            break;
        
        wi_mutable_data_append_bytes(data, buffer, bytes);
    }
    
    if(bytes < 0) {
        wi_release(data);
        
        data = NULL;
    }
    
    wi_runtime_make_immutable(data);
    
    return wi_autorelease(data);
}



wi_integer_t wi_pipe_read_bytes(wi_pipe_t *pipe, void *buffer, wi_uinteger_t length) {
    wi_uinteger_t   offset;
    wi_integer_t    bytes;
    
    _WI_PIPE_ASSERT_OPEN(pipe);
    
    offset = 0;
    
    while(offset < length) {
        bytes = read(pipe->rd, buffer + offset, length - offset);
        
        if(bytes > 0) {
            offset += bytes;
        } else {
            if(bytes == 0) {
                return offset;
            } else {
                if(errno == EINTR) {
                    continue;
                } else {
                    wi_error_set_errno(errno);
                    
                    return -1;
                }
            }
        }
    }
    
    return offset;
}



#pragma mark -

wi_integer_t wi_pipe_write(wi_pipe_t *pipe, wi_data_t *data) {
    return wi_pipe_write_bytes(pipe, wi_data_bytes(data), wi_data_length(data));
}



wi_integer_t wi_pipe_write_bytes(wi_pipe_t *pipe, const void *buffer, wi_uinteger_t length) {
    wi_uinteger_t   offset;
    wi_integer_t    bytes;
    
    _WI_PIPE_ASSERT_OPEN(pipe);
    
    offset = 0;
    
    while(offset < length) {
        bytes = write(pipe->wd, buffer + offset, length - offset);
        
        if(bytes > 0) {
            offset += bytes;
        } else {
            if(bytes == 0) {
                return offset;
            } else {
                if(errno == EINTR) {
                    continue;
                } else {
                    wi_error_set_errno(errno);
                    
                    return -1;
                }
            }
        }
    }
    
    return offset;
}



#pragma mark -

void wi_pipe_close(wi_pipe_t *pipe) {
    if(pipe->rd >= 0) {
        close(pipe->rd);
        
        pipe->rd = -1;
    }
    
    if(pipe->wd >= 0) {
        close(pipe->wd);
        
        pipe->wd = -1;
    }
}
