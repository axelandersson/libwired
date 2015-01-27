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

#ifndef WI_FILESYSTEM_H
#define WI_FILESYSTEM_H 1

#include <sys/param.h>
#include <wired/wi-base.h>
#include <wired/wi-date.h>
#include <wired/wi-file.h>
#include <wired/wi-fsenumerator.h>
#include <wired/wi-runtime.h>

#define WI_PATH_SIZE                        MAXPATHLEN


enum _wi_file_type {
    WI_FILE_REGULAR,
    WI_FILE_DIRECTORY,
    WI_FILE_SYMBOLIC_LINK,
    WI_FILE_SOCKET,
    WI_FILE_PIPE,
    WI_FILE_UNKNOWN
};
typedef enum _wi_file_type                  wi_file_type_t;

struct _wi_file_stats {
    uint32_t                                filesystem_id;
    uint64_t                                file_id;
    wi_file_type_t                          file_type;
    uint64_t                                size;
    uint32_t                                posix_permissions;
    uint32_t                                reference_count;
    uint32_t                                user_id;
    wi_string_t                             *user;
    uint32_t                                group_id;
    wi_string_t                             *group;
    wi_date_t                               *creation_date;
    wi_date_t                               *modification_date;
};
typedef struct _wi_file_stats               wi_file_stats_t;

struct _wi_filesystem_stats {
    uint32_t                                filesystem_id;
    uint64_t                                size;
    uint64_t                                free_size;
    uint64_t                                nodes;
    uint64_t                                free_nodes;
};
typedef struct _wi_filesystem_stats         wi_filesystem_stats_t;

typedef void                                wi_filesystem_delete_path_callback_t(wi_string_t *);
typedef void                                wi_filesystem_copy_path_callback_t(wi_string_t *, wi_string_t *);


WI_EXPORT wi_string_t *                     wi_filesystem_temporary_path_with_template(wi_string_t *);

WI_EXPORT wi_boolean_t                      wi_filesystem_create_directory_at_path(wi_string_t *);
WI_EXPORT wi_boolean_t                      wi_filesystem_change_directory_to_path(wi_string_t *);

WI_EXPORT wi_boolean_t                      wi_filesystem_file_exists_at_path(wi_string_t *, wi_boolean_t *);

WI_EXPORT wi_boolean_t                      wi_filesystem_copy_path(wi_string_t *, wi_string_t *);
WI_EXPORT wi_boolean_t                      wi_filesystem_copy_path_with_callback(wi_string_t *, wi_string_t *, wi_filesystem_copy_path_callback_t);
WI_EXPORT wi_boolean_t                      wi_filesystem_delete_path(wi_string_t *);
WI_EXPORT wi_boolean_t                      wi_filesystem_delete_path_with_callback(wi_string_t *, wi_filesystem_delete_path_callback_t *);
WI_EXPORT wi_boolean_t                      wi_filesystem_rename_path(wi_string_t *, wi_string_t *);
WI_EXPORT wi_boolean_t                      wi_filesystem_create_symbolic_link_from_path(wi_string_t *, wi_string_t *);

WI_EXPORT wi_boolean_t                      wi_filesystem_get_file_stats_for_path(wi_string_t *, wi_file_stats_t *);
WI_EXPORT wi_boolean_t                      wi_filesystem_get_filesystem_stats_for_path(wi_string_t *, wi_filesystem_stats_t *);

WI_EXPORT wi_array_t *                      wi_filesystem_directory_contents_at_path(wi_string_t *);
WI_EXPORT wi_fsenumerator_t *               wi_filesystem_directory_enumerator_at_path(wi_string_t *);

#endif /* WI_FILESYSTEM_H */
