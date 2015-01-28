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
#include <sys/stat.h>
#include <sys/mount.h>

#ifdef HAVE_SYS_ATTR_H
#include <sys/attr.h>
#endif

#ifdef HAVE_SYS_STATVFS_H
#include <sys/statvfs.h>
#endif

#ifdef HAVE_SYS_STATFS_H
#include <sys/statfs.h>
#endif

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
#include <wired/wi-date.h>
#include <wired/wi-filesystem.h>
#include <wired/wi-fts.h>
#include <wired/wi-lock.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-runtime.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

static wi_boolean_t                         _wi_filesystem_delete_file(wi_string_t *, wi_filesystem_delete_path_callback_t *);
static wi_boolean_t                         _wi_filesystem_delete_directory(wi_string_t *, wi_filesystem_delete_path_callback_t *);
static wi_boolean_t                         _wi_filesystem_copy_file(wi_string_t *, wi_string_t *, wi_filesystem_copy_path_callback_t);
static wi_boolean_t                         _wi_filesystem_copy_directory(wi_string_t *, wi_string_t *, wi_filesystem_copy_path_callback_t);


wi_string_t * wi_filesystem_temporary_path_with_template(wi_string_t *template) {
    char    path[WI_PATH_SIZE];
    
    wi_strlcpy(path, wi_string_utf8_string(template), sizeof(path));
    
    if(!mktemp(path))
        return NULL;
    
    return wi_string_with_utf8_string(path);
}



#pragma mark -

wi_boolean_t wi_filesystem_create_directory_at_path(wi_string_t *path) {
    if(mkdir(wi_string_utf8_string(path), 0777) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



wi_boolean_t wi_filesystem_change_current_directory_to_path(wi_string_t *path) {
    if(chdir(wi_string_utf8_string(path)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



wi_string_t * wi_filesystem_current_directory_path(void) {
    char    path[WI_PATH_SIZE];
    
    if(!getcwd(path, sizeof(path))) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return wi_string_with_utf8_string(path);
}



#pragma mark -

wi_boolean_t wi_filesystem_file_exists_at_path(wi_string_t *path, wi_boolean_t *is_directory) {
    wi_file_stats_t     stats;
    
    if(!wi_filesystem_get_file_stats_for_path(path, &stats))
        return false;
    
    if(is_directory)
        *is_directory = (stats.file_type == WI_FILE_DIRECTORY);
    
    return true;
}



#pragma mark -

wi_boolean_t wi_filesystem_copy_path(wi_string_t *frompath, wi_string_t *topath) {
    return wi_filesystem_copy_path_with_callback(frompath, topath, NULL);
}



wi_boolean_t wi_filesystem_copy_path_with_callback(wi_string_t *frompath, wi_string_t *topath, wi_filesystem_copy_path_callback_t callback) {
    wi_file_stats_t     stats;
    int                 err;
    wi_boolean_t        result;
    
    if(!wi_filesystem_get_file_stats_for_path(frompath, &stats))
        return false;
    
    if(wi_filesystem_file_exists_at_path(topath, NULL)) {
        wi_error_set_errno(EEXIST);
        
        return false;
    }
    
    if(stats.file_type == WI_FILE_DIRECTORY)
        result = _wi_filesystem_copy_directory(frompath, topath, callback);
    else
        result = _wi_filesystem_copy_file(frompath, topath, callback);
    
    if(!result) {
        err = errno;
        
        wi_filesystem_delete_path(topath);
        
        wi_error_set_errno(err);
    }
    
    return result;
}



static wi_boolean_t _wi_filesystem_copy_file(wi_string_t *frompath, wi_string_t *topath, wi_filesystem_copy_path_callback_t callback) {
    char            buffer[8192];
    int             fromfd = -1, tofd = -1;
    int             rbytes, wbytes;
    wi_boolean_t    result = false;
    
    fromfd = open(wi_string_utf8_string(frompath), O_RDONLY, 0);
    
    if(fromfd < 0)
        goto end;
    
    tofd = open(wi_string_utf8_string(topath), O_WRONLY | O_TRUNC | O_CREAT, 0666);
    
    if(tofd < 0)
        goto end;
    
    while((rbytes = read(fromfd, buffer, sizeof(buffer))) > 0) {
        wbytes = write(tofd, buffer, rbytes);
        
        if(rbytes != wbytes || wbytes < 0)
            goto end;
    }
    
    if(callback)
        (*callback)(frompath, topath);
    
    result = true;
    
end:
    if(tofd >= 0)
        close(tofd);
    
    if(fromfd >= 0)
        close(fromfd);
    
    return result;
}



static wi_boolean_t _wi_filesystem_copy_directory(wi_string_t *frompath, wi_string_t *topath, wi_filesystem_copy_path_callback_t callback) {
    WI_FTS                  *fts;
    WI_FTSENT               *p;
    wi_mutable_string_t     *newpath;
    wi_string_t             *path;
    char                    *paths[2];
    wi_uinteger_t           pathlength;
    wi_boolean_t            result = true;
    
    paths[0] = (char *) wi_string_utf8_string(frompath);
    paths[1] = NULL;
    
    fts = wi_fts_open(paths, WI_FTS_LOGICAL | WI_FTS_NOSTAT, NULL);
    
    if(!fts)
        return false;
    
    pathlength = wi_string_length(frompath);
    
    while((p = wi_fts_read(fts))) {
        path        = wi_string_init_with_utf8_string(wi_string_alloc(), p->fts_path);
        newpath     = wi_string_init_with_utf8_string(wi_mutable_string_alloc(), p->fts_path + pathlength);
        
        wi_mutable_string_insert_string_at_index(newpath, topath, 0);
        
        switch(p->fts_info) {
            case WI_FTS_NS:
            case WI_FTS_ERR:
            case WI_FTS_DNR:
                errno = p->fts_errno;
                result = false;
                break;
                
            case WI_FTS_DC:
            case WI_FTS_DP:
                break;
                
            case WI_FTS_D:
                if(!wi_filesystem_create_directory_at_path(newpath)) {
                    result = false;
                } else {
                    if(callback)
                        (*callback)(path, newpath);
                }
                break;
                
            default:
                if(!_wi_filesystem_copy_file(path, newpath, callback))
                    result = false;
                break;
        }
        
        wi_release(newpath);
        wi_release(path);
    }
    
    wi_fts_close(fts);
    
    return result;
}



wi_boolean_t wi_filesystem_delete_path(wi_string_t *path) {
    return wi_filesystem_delete_path_with_callback(path, NULL);
}



wi_boolean_t wi_filesystem_delete_path_with_callback(wi_string_t *path, wi_filesystem_delete_path_callback_t *callback) {
    wi_file_stats_t     stats;
    
    if(!wi_filesystem_get_file_stats_for_path(path, &stats))
        return false;
    
    if(stats.file_type == WI_FILE_DIRECTORY)
        return _wi_filesystem_delete_directory(path, callback);
    else
        return _wi_filesystem_delete_file(path, callback);
}



static wi_boolean_t _wi_filesystem_delete_file(wi_string_t *path, wi_filesystem_delete_path_callback_t *callback) {
    if(unlink(wi_string_utf8_string(path)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    } else {
        if(callback)
            (*callback)(path);
        
        return true;
    }
}



static wi_boolean_t _wi_filesystem_delete_directory(wi_string_t *path, wi_filesystem_delete_path_callback_t *callback) {
    WI_FTS          *fts;
    WI_FTSENT       *p;
    char            *paths[2];
    wi_boolean_t    result = true;
    int             err = 0;
    
    paths[0] = (char *) wi_string_utf8_string(path);
    paths[1] = NULL;
    
    errno = 0;

    fts = wi_fts_open(paths, WI_FTS_NOSTAT | WI_FTS_LOGICAL, NULL);

    if(!fts || errno != 0)
        return false;
    
    while((p = wi_fts_read(fts))) {
        switch(p->fts_info) {
            case WI_FTS_NS:
            case WI_FTS_ERR:
            case WI_FTS_DNR:
                err = p->fts_errno;
                result = false;
                break;
                
            case WI_FTS_DC:
                err = ELOOP;
                
                result = false;
                break;

            case WI_FTS_D:
                break;

            case WI_FTS_DP:
                if(rmdir(p->fts_path) < 0) {
                    err = errno;

                    result = false;
                } else {
                    if(callback)
                        (*callback)(wi_string_with_utf8_string(p->fts_path));
                }
                break;

            default:
                if(unlink(p->fts_path) < 0) {
                    err = errno;

                    result = false;
                } else {
                    if(callback)
                        (*callback)(wi_string_with_utf8_string(p->fts_path));
                }
                break;
        }
    }
    
    wi_fts_close(fts);
    
    if(err > 0)
        wi_error_set_errno(err);

    return result;
}



wi_boolean_t wi_filesystem_rename_path(wi_string_t *path, wi_string_t *newpath) {
    if(rename(wi_string_utf8_string(path), wi_string_utf8_string(newpath)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



wi_boolean_t wi_filesystem_create_symbolic_link_from_path(wi_string_t *frompath, wi_string_t *topath) {
    if(symlink(wi_string_utf8_string(frompath), wi_string_utf8_string(topath)) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    return true;
}



#pragma mark -

wi_boolean_t wi_filesystem_get_file_stats_for_path(wi_string_t *path, wi_file_stats_t *stats) {
    struct passwd       *user;
    struct group        *group;
#if defined(HAVE_STAT64) && !defined(_DARWIN_FEATURE_64_BIT_INODE)
    struct stat64        sb;
    
    if(lstat64(wi_string_utf8_string(path), &sb) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    if(stats) {
        stats->filesystem_id        = sb.st_dev;
        stats->file_id              = sb.st_ino;
        
        if(S_ISREG(sb.st_mode))
            stats->file_type        = WI_FILE_REGULAR;
        else if(S_ISDIR(sb.st_mode))
            stats->file_type        = WI_FILE_DIRECTORY;
        else if(S_ISLNK(sb.st_mode))
            stats->file_type        = WI_FILE_SYMBOLIC_LINK;
        else if(S_ISSOCK(sb.st_mode))
            stats->file_type        = WI_FILE_SOCKET;
        else if(S_ISFIFO(sb.st_mode))
            stats->file_type        = WI_FILE_PIPE;
        else
            stats->file_type        = WI_FILE_UNKNOWN;
        
        stats->size                 = sb.st_size;
        stats->posix_permissions    = sb.st_mode;
        stats->reference_count      = sb.st_nlink;
        stats->user_id              = sb.st_uid;
        
        if((user = getpwuid(sb.st_uid)))
            stats->user              = wi_string_with_utf8_string(user->pw_name);
        
        stats->group_id             = sb.st_gid;
        
        if((group = getgrgid(sb.st_gid)))
            stats->group            = wi_string_with_utf8_string(group->gr_name);
        
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIME
        stats->creation_date        = wi_date_with_time(sb.st_birthtime);
#else
        stats->creation_date        = wi_date_with_time(sb.st_ctime);
#endif
        
        stats->modification_date    = wi_date_with_time(sb.st_mtime);
    }
    
    return true;
#else
    struct stat        sb;
    
    if(lstat(wi_string_utf8_string(path), &sb) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }
    
    if(stats) {
        stats->filesystem_id        = sb.st_dev;
        stats->file_id              = sb.st_ino;
        
        if(S_ISREG(sb.st_mode))
            stats->file_type        = WI_FILE_REGULAR;
        else if(S_ISDIR(sb.st_mode))
            stats->file_type        = WI_FILE_DIRECTORY;
        else if(S_ISLNK(sb.st_mode))
            stats->file_type        = WI_FILE_SYMBOLIC_LINK;
        else if(S_ISSOCK(sb.st_mode))
            stats->file_type        = WI_FILE_SOCKET;
        else if(S_ISFIFO(sb.st_mode))
            stats->file_type        = WI_FILE_PIPE;
        else
            stats->file_type        = WI_FILE_UNKNOWN;
        
        stats->size                 = sb.st_size;
        stats->posix_permissions    = sb.st_mode;
        stats->reference_count      = sb.st_nlink;
        stats->user_id              = sb.st_uid;
        
        if((user = getpwuid(sb.st_uid)))
           stats->user              = wi_string_with_utf8_string(user->pw_name);
        
        stats->group_id             = sb.st_gid;
        
        if((group = getgrgid(sb.st_gid)))
            stats->group            = wi_string_with_utf8_string(group->gr_name);
        
#ifdef HAVE_STRUCT_STAT_ST_BIRTHTIME
        stats->creation_date        = wi_date_with_time(sb.st_birthtime);
#else
        stats->creation_date        = wi_date_with_time(sb.st_ctime);
#endif
        
        stats->modification_date    = wi_date_with_time(sb.st_mtime);
    }
    
    return true;
#endif
}



wi_boolean_t wi_filesystem_get_filesystem_stats_for_path(wi_string_t *path, wi_filesystem_stats_t *fsstats) {
#ifdef HAVE_STATVFS
    struct statvfs  sfvb;

    if(statvfs(wi_string_utf8_string(path), &sfvb) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }

    fsstats->filesystem_id  = sfvb.f_fsid;
    fsstats->size           = sfvb.f_blocks * sfvb.f_frsize;
    fsstats->free_size      = sfvb.f_bfree * sfvb.f_frsize;
    fsstats->nodes          = sfvb.f_files;
    fsstats->free_nodes     = sfvb.f_ffree;
#else
    struct statfs   sfb;

    if(statfs(wi_string_utf8_string(path), &sfb) < 0) {
        wi_error_set_errno(errno);
        
        return false;
    }

    fsstats->filesystem_id  = sfb.f_fsid;
    fsstats->size           = sfb.f_blocks * sfb.f_bsize;
    fsstats->free_size      = sfb.f_bfree * sfb.f_bsize;
    fsstats->nodes          = sfb.f_files;
    fsstats->free_nodes     = sfb.f_ffree;
#endif
    
    return true;
}



#pragma mark -

wi_array_t * wi_filesystem_directory_contents_at_path(wi_string_t *path) {
    wi_mutable_array_t        *contents;
    wi_string_t                *name;
    DIR                        *dir;
    struct dirent            *de, *dep;
    
    dir = opendir(wi_string_utf8_string(path));
    
    if(!dir) {
        wi_error_set_errno(errno);
        
        return NULL;
    }
    
    contents = wi_array_init_with_capacity(wi_mutable_array_alloc(), 100);
    
    de = wi_malloc(sizeof(struct dirent) + WI_PATH_SIZE);
    
    while(readdir_r(dir, de, &dep) == 0 && dep) {
        if(strcmp(dep->d_name, ".") != 0 && strcmp(dep->d_name, "..") != 0) {
            name = wi_string_init_with_utf8_string(wi_string_alloc(), dep->d_name);
            wi_mutable_array_add_data(contents, name);
            wi_release(name);
        }
    }
    
    wi_free(de);
    
    closedir(dir);
    
    wi_runtime_make_immutable(contents);
    
    return wi_autorelease(contents);
}



wi_fsenumerator_t * wi_filesystem_directory_enumerator_at_path(wi_string_t *path) {
    return wi_autorelease(wi_fsenumerator_init_with_path(wi_fsenumerator_alloc(), path));
}
