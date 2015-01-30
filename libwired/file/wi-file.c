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

#include <wired/wi-array.h>
#include <wired/wi-assert.h>
#include <wired/wi-byteorder.h>
#include <wired/wi-compat.h>
#include <wired/wi-digest.h>
#include <wired/wi-file.h>
#include <wired/wi-fts.h>
#include <wired/wi-lock.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>

#define _WI_FILE_ASSERT_OPEN(file) \
    WI_ASSERT((file)->fd >= 0, "%@ is not open", (file))


struct _wi_file {
    wi_runtime_base_t                   base;

    wi_string_t                         *path;
    int                                 fd;
    wi_file_offset_t                    offset;
};


static void                             _wi_file_dealloc(wi_runtime_instance_t *);
static wi_string_t *                    _wi_file_description(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_file_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_file_runtime_class = {
    "wi_file_t",
    _wi_file_dealloc,
    NULL,
    NULL,
    _wi_file_description,
    NULL
};



void wi_file_register(void) {
    _wi_file_runtime_id = wi_runtime_register_class(&_wi_file_runtime_class);
}



void wi_file_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_file_runtime_id(void) {
    return _wi_file_runtime_id;
}



#pragma mark -

wi_file_t * wi_file_for_reading(wi_string_t *path) {
    return wi_autorelease(wi_file_init_with_path(wi_file_alloc(), path, WI_FILE_READING));
}



wi_file_t * wi_file_for_writing(wi_string_t *path) {
    return wi_autorelease(wi_file_init_with_path(wi_file_alloc(), path, WI_FILE_WRITING));
}



wi_file_t * wi_file_for_updating(wi_string_t *path) {
    return wi_autorelease(wi_file_init_with_path(wi_file_alloc(), path, WI_FILE_READING | WI_FILE_WRITING | WI_FILE_UPDATING));
}



wi_file_t * wi_file_temporary_file(void) {
    return wi_autorelease(wi_file_init_temporary_file(wi_file_alloc()));
}



#pragma mark -

wi_file_t * wi_file_alloc(void) {
    return wi_runtime_create_instance(_wi_file_runtime_id, sizeof(wi_file_t));
}



wi_file_t * wi_file_init_with_path(wi_file_t *file, wi_string_t *path, wi_file_mode_t mode) {
    int     flags;

    if(mode & WI_FILE_WRITING)    
        flags = O_CREAT;
    else
        flags = 0;
    
    if((mode & WI_FILE_READING) && (mode & WI_FILE_WRITING))
        flags |= O_RDWR;
    else if(mode & WI_FILE_READING)
        flags |= O_RDONLY;
    else if(mode & WI_FILE_WRITING)
        flags |= O_WRONLY;
    
    if(mode & WI_FILE_WRITING) {
        if(mode & WI_FILE_UPDATING)
            flags |= O_APPEND;
        else
            flags |= O_TRUNC;
    }
        
    file->fd = open(wi_string_utf8_string(path), flags, 0666);
    
    if(file->fd < 0) {
        wi_error_set_errno(errno);

        wi_release(file);
        
        return NULL;
    }
    
    file->path = wi_retain(path);
    
    return file;
}



wi_file_t * wi_file_init_with_file_descriptor(wi_file_t *file, int fd) {
    file->fd = fd;
    
    return file;
}



wi_file_t * wi_file_init_temporary_file(wi_file_t *file) {
    FILE    *fp;
    
    fp = wi_tmpfile();
    
    if(!fp) {
        wi_error_set_errno(errno);

        wi_release(file);
        
        return NULL;
    }

    return wi_file_init_with_file_descriptor(file, fileno(fp));
}




static void _wi_file_dealloc(wi_runtime_instance_t *instance) {
    wi_file_t   *file = instance;
    
    wi_file_close(file);
    
    wi_release(file->path);
}



static wi_string_t * _wi_file_description(wi_runtime_instance_t *instance) {
    wi_file_t   *file = instance;
    
    return wi_string_with_format(WI_STR("<%@ %p>{descriptor = %d, path = %@}"),
      wi_runtime_class_name(file),
      file,
      file->fd,
      file->path);
}



#pragma mark -

wi_string_t * wi_file_path(wi_file_t *file) {
    return file->path;
}



int wi_file_descriptor(wi_file_t *file) {
    return file->fd;
}



#pragma mark -

wi_data_t * wi_file_read(wi_file_t *file, wi_uinteger_t length) {
    wi_mutable_data_t   *data;
    char                buffer[WI_FILE_BUFFER_SIZE];
    wi_integer_t        bytes = -1;
    
    _WI_FILE_ASSERT_OPEN(file);
    
    data = wi_data_init_with_capacity(wi_mutable_data_alloc(), length);
    
    while(length > 0) {
        bytes = wi_file_read_bytes(file, buffer, WI_MIN(sizeof(buffer), length));
        
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



wi_data_t * wi_file_read_to_end_of_file(wi_file_t *file) {
    wi_mutable_data_t   *data;
    char                buffer[WI_FILE_BUFFER_SIZE];
    wi_integer_t        bytes;
    
    _WI_FILE_ASSERT_OPEN(file);
    
    data = wi_data_init_with_capacity(wi_mutable_data_alloc(), WI_FILE_BUFFER_SIZE);
    
    while(true) {
        bytes = wi_file_read_bytes(file, buffer, sizeof(buffer));
        
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



wi_integer_t wi_file_read_bytes(wi_file_t *file, void *buffer, wi_uinteger_t length) {
    wi_uinteger_t   offset;
    wi_integer_t    bytes;
    
    _WI_FILE_ASSERT_OPEN(file);
    
    offset = 0;
    
    while(offset < length) {
        bytes = read(file->fd, buffer + offset, length - offset);
        
        if(bytes > 0) {
            offset += bytes;
            file->offset += bytes;
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

wi_integer_t wi_file_write(wi_file_t *file, wi_data_t *data) {
    return wi_file_write_bytes(file, wi_data_bytes(data), wi_data_length(data));
}



wi_integer_t wi_file_write_bytes(wi_file_t *file, const void *buffer, wi_uinteger_t length) {
    wi_uinteger_t   offset;
    wi_integer_t    bytes;
    
    _WI_FILE_ASSERT_OPEN(file);
    
    offset = 0;
    
    while(offset < length) {
        bytes = write(file->fd, buffer + offset, length - offset);
        
        if(bytes > 0) {
            offset += bytes;
            file->offset += bytes;
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

void wi_file_seek(wi_file_t *file, wi_file_offset_t offset) {
    off_t   r;
    
    _WI_FILE_ASSERT_OPEN(file);
    
    r = lseek(file->fd, (off_t) offset, SEEK_SET);
    
    if(r >= 0)
        file->offset = r;
}



wi_file_offset_t wi_file_seek_to_end_of_file(wi_file_t *file) {
    off_t   r;
    
    _WI_FILE_ASSERT_OPEN(file);
    
    r = lseek(file->fd, 0, SEEK_END);
    
    if(r >= 0)
        file->offset = r;

    return file->offset;
}



wi_file_offset_t wi_file_offset(wi_file_t *file) {
    return file->offset;
}



#pragma mark -

wi_boolean_t wi_file_truncate(wi_file_t *file, wi_file_offset_t offset) {
    _WI_FILE_ASSERT_OPEN(file);
    
    if(ftruncate(file->fd, offset) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



void wi_file_close(wi_file_t *file) {
    if(file->fd >= 0) {
        (void) close(file->fd);
        
        file->fd = -1;
    }
}
