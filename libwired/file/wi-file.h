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

#ifndef WI_FILE_H
#define WI_FILE_H 1

#include <sys/types.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <stdio.h>
#include <wired/wi-base.h>
#include <wired/wi-runtime.h>

#define WI_PATH_SIZE                MAXPATHLEN
#define WI_FILE_BUFFER_SIZE         BUFSIZ


typedef uint64_t                    wi_file_offset_t;

enum _wi_file_mode {
    WI_FILE_READING                 = (1 << 0),
    WI_FILE_WRITING                 = (1 << 1),
    WI_FILE_UPDATING                = (1 << 2)
};
typedef enum _wi_file_mode          wi_file_mode_t;


WI_EXPORT wi_runtime_id_t           wi_file_runtime_id(void);

WI_EXPORT wi_file_t *               wi_file_for_reading(wi_string_t *);
WI_EXPORT wi_file_t *               wi_file_for_writing(wi_string_t *);
WI_EXPORT wi_file_t *               wi_file_for_updating(wi_string_t *);
WI_EXPORT wi_file_t *               wi_file_temporary_file(void);

WI_EXPORT wi_file_t *               wi_file_alloc(void);
WI_EXPORT wi_file_t *               wi_file_init_with_path(wi_file_t *, wi_string_t *, wi_file_mode_t);
WI_EXPORT wi_file_t *               wi_file_init_with_file_descriptor(wi_file_t *, int);
WI_EXPORT wi_file_t *               wi_file_init_temporary_file(wi_file_t *);

WI_EXPORT wi_string_t *             wi_file_path(wi_file_t *);
WI_EXPORT int                       wi_file_descriptor(wi_file_t *);

WI_EXPORT wi_data_t *               wi_file_read(wi_file_t *, wi_uinteger_t);
WI_EXPORT wi_data_t *               wi_file_read_to_end_of_file(wi_file_t *);
WI_EXPORT wi_integer_t              wi_file_read_bytes(wi_file_t *, void *, wi_uinteger_t);

WI_EXPORT wi_integer_t              wi_file_write(wi_file_t *, wi_data_t *);
WI_EXPORT wi_integer_t              wi_file_write_bytes(wi_file_t *, const void *, wi_uinteger_t);

WI_EXPORT void                      wi_file_seek(wi_file_t *, wi_file_offset_t);
WI_EXPORT wi_file_offset_t          wi_file_seek_to_end_of_file(wi_file_t *);
WI_EXPORT wi_file_offset_t          wi_file_offset(wi_file_t *);

WI_EXPORT wi_boolean_t              wi_file_truncate(wi_file_t *, wi_file_offset_t);
WI_EXPORT void                      wi_file_close(wi_file_t *);

#endif /* WI_FILE_H */
