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

#include <wired/wi-directory-enumerator.h>
#include <wired/wi-error.h>
#include <wired/wi-fts.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>

struct _wi_directory_enumerator {
    wi_runtime_base_t                   base;

    WI_FTS                              *fts;
    WI_FTSENT                           *ftsent;
};


static void                             _wi_directory_enumerator_dealloc(wi_runtime_instance_t *);


static wi_runtime_id_t                  _wi_directory_enumerator_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_directory_enumerator_runtime_class = {
    "wi_directory_enumerator_t",
    _wi_directory_enumerator_dealloc,
    NULL,
    NULL,
    NULL,
    NULL
};


void wi_directory_enumerator_register(void) {
    _wi_directory_enumerator_runtime_id = wi_runtime_register_class(&_wi_directory_enumerator_runtime_class);
}



void wi_directory_enumerator_initialize(void) {
}



#pragma mark -

wi_directory_enumerator_t * wi_directory_enumerator_alloc(void) {
    return wi_runtime_create_instance(_wi_directory_enumerator_runtime_id, sizeof(wi_directory_enumerator_t));
}



wi_directory_enumerator_t * wi_directory_enumerator_init_with_path(wi_directory_enumerator_t *directory_enumerator, wi_string_t *path) {
    char    *paths[2];

    paths[0] = (char *) wi_string_utf8_string(path);
    paths[1] = NULL;

    errno = 0;
    directory_enumerator->fts = wi_fts_open(paths, WI_FTS_NOSTAT | WI_FTS_LOGICAL, NULL);
    
    if(!directory_enumerator->fts || errno != 0) {
        wi_error_set_errno(errno);
        wi_release(directory_enumerator);

        return NULL;
    }

    return directory_enumerator;
}



static void _wi_directory_enumerator_dealloc(wi_runtime_instance_t *instance) {
    wi_directory_enumerator_t   *directory_enumerator = instance;

    if(directory_enumerator->fts)
        wi_fts_close(directory_enumerator->fts);
}



#pragma mark -

wi_directory_enumerator_status_t wi_directory_enumerator_get_next_path(wi_directory_enumerator_t *directory_enumerator, wi_string_t **path) {
    while((directory_enumerator->ftsent = wi_fts_read(directory_enumerator->fts))) {
        if(directory_enumerator->ftsent->fts_level == 0)
            continue;

        if(directory_enumerator->ftsent->fts_name[0] == '.') {
            wi_fts_set(directory_enumerator->fts, directory_enumerator->ftsent, WI_FTS_SKIP);
            
            continue;
        }
        
        switch(directory_enumerator->ftsent->fts_info) {
            case WI_FTS_DC:
                *path = wi_string_with_utf8_string(directory_enumerator->ftsent->fts_path);
                wi_error_set_errno(ELOOP);

                return WI_DIRECTORY_ENUMERATOR_ERROR;
                break;

            case WI_FTS_DNR:
            case WI_FTS_ERR:
                *path = wi_string_with_utf8_string(directory_enumerator->ftsent->fts_path);
                wi_error_set_errno(directory_enumerator->ftsent->fts_errno);

                return WI_DIRECTORY_ENUMERATOR_ERROR;
                break;

            case WI_FTS_DP:
                continue;
                break;

            default:
                *path = wi_string_with_utf8_string(directory_enumerator->ftsent->fts_path);

                return WI_DIRECTORY_ENUMERATOR_PATH;
                break;
        }
    }

    return WI_DIRECTORY_ENUMERATOR_EOF;
}



void wi_directory_enumerator_skip_descendents(wi_directory_enumerator_t *directory_enumerator) {
    if(directory_enumerator->ftsent)
        wi_fts_set(directory_enumerator->fts, directory_enumerator->ftsent, WI_FTS_SKIP);
}



wi_uinteger_t wi_directory_enumerator_level(wi_directory_enumerator_t *directory_enumerator) {
    return directory_enumerator->ftsent ? directory_enumerator->ftsent->fts_level : 0;
}
