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

#if HAVE_SYSLOG_FACILITYNAMES
#define SYSLOG_NAMES
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <errno.h>

#include <wired/wi-base.h>
#include <wired/wi-compat.h>
#include <wired/wi-file.h>
#include <wired/wi-lock.h>
#include <wired/wi-log.h>
#include <wired/wi-process.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>
#include <wired/wi-system.h>

#define _WI_LOG_DATE_SIZE           32


static void                         _wi_log_vlog(wi_log_level_t, wi_string_t *, va_list);
static void                         _wi_log_get_date(char *);
static void                         _wi_log_truncate_file(const char *);


static wi_log_level_t               _wi_log_level = WI_LOG_INFO;

static wi_boolean_t                 _wi_log_stdout_enabled;
static wi_log_style_t               _wi_log_stdout_style;

static wi_boolean_t                 _wi_log_file_enabled;
static wi_string_t                  *_wi_log_file_path;
static wi_uinteger_t                _wi_log_file_limit;

static wi_boolean_t                 _wi_log_syslog_enabled;
static int                          _wi_log_syslog_facility;

static wi_boolean_t                 _wi_log_callback_enabled;
wi_log_callback_func_t              *_wi_log_callback_function;

static wi_uinteger_t                _wi_log_file_lines;
static wi_recursive_lock_t          *_wi_log_file_lock;
static wi_boolean_t                 _wi_log_in_callback;



void wi_log_register(void) {
}



void wi_log_initialize(void) {
    _wi_log_file_lock = wi_recursive_lock_init(wi_recursive_lock_alloc());
}



#pragma mark -

void wi_log_set_level(wi_log_level_t level) {
    _wi_log_level = level;
}



wi_log_level_t wi_log_level(void) {
    return _wi_log_level;
}



#pragma mark -

void wi_log_add_stdout_logger(wi_log_style_t style) {
    WI_ASSERT(!_wi_log_stdout_enabled, "can't have more than one stdout logger");
    
    _wi_log_stdout_enabled = true;
    _wi_log_stdout_style = style;
}



void wi_log_remove_stdout_logger(void) {
    _wi_log_stdout_enabled = false;
    
}



void wi_log_add_file_logger(wi_string_t *path, wi_uinteger_t limit) {
    WI_ASSERT(!_wi_log_file_enabled, "can't have more than one file logger");
    
    _wi_log_file_enabled = true;
    _wi_log_file_path = wi_retain(path);
    _wi_log_file_limit = limit;
}



void wi_log_remove_file_logger(void) {
    _wi_log_file_enabled = false;
    
    wi_release(_wi_log_file_path);
    _wi_log_file_path = NULL;
}



void wi_log_add_syslog_logger(int facility) {
    WI_ASSERT(!_wi_log_syslog_enabled, "can't have more than one syslog logger");
    
    _wi_log_syslog_enabled = true;
    _wi_log_syslog_facility = facility;
    
    openlog(wi_string_utf8_string(wi_process_name(wi_process())), LOG_PID | LOG_NDELAY, facility);
}



void wi_log_remove_syslog_logger(void) {
    _wi_log_syslog_enabled = false;
    
    closelog();
}



void wi_log_add_callback_logger(wi_log_callback_func_t function) {
    WI_ASSERT(!_wi_log_callback_enabled, "can't have more than one callback logger");
    
    _wi_log_callback_enabled = true;
    _wi_log_callback_function = function;
}



void wi_log_remove_callback_logger(void) {
    _wi_log_callback_enabled = false;
}



#pragma mark -

int wi_log_syslog_facility_with_name(wi_string_t *name) {
#if HAVE_SYSLOG_FACILITYNAMES
    const char  *utf8string;
    int         i;
    
    utf8string = wi_string_utf8_string(name);
    
    for(i = 0; facilitynames[i].c_name != NULL; i++) {
        if(strcmp(utf8string, facilitynames[i].c_name) == 0)
            break;
    }

    if(!facilitynames[i].c_name) {
        wi_error_set_libwired_error(WI_ERROR_LOG_NOSUCHFACILITY);
        
        return -1;
    }

    return facilitynames[i].c_val;
#else
    wi_error_set_errno(ENOTSUP);
    
    return -1;
#endif
}



#pragma mark -

static void _wi_log_vlog(wi_log_level_t level, wi_string_t *fmt, va_list ap) {
    wi_string_t     *string;
    FILE            *fp = NULL;
    const char      *utf8string, *name, *path, *prefix;
    char            date[_WI_LOG_DATE_SIZE];
    int             priority;
    
    if(_wi_log_in_callback)
        return;
    
    string = wi_string_init_with_format_and_arguments(wi_string_alloc(), fmt, ap);
    utf8string = wi_string_utf8_string(string);
    name = wi_string_utf8_string(wi_process_name(wi_process()));
    
    _wi_log_get_date(date);
    
    switch(level) {
        default:
        case WI_LOG_INFO:
            priority = LOG_INFO;
            prefix = "Info";
            break;
            
        case WI_LOG_WARN:
            priority = LOG_WARNING;
            prefix = "Warning";
            break;
            
        case WI_LOG_ERROR:
            priority = LOG_ERR;
            prefix = "Error";
            break;
            
        case WI_LOG_FATAL:
            priority = LOG_CRIT;
            prefix = "Fatal";
            break;
            
        case WI_LOG_DEBUG:
            priority = LOG_DEBUG;
            prefix = "Debug";
            break;
    }
    
    if(_wi_log_stdout_enabled) {
        switch(_wi_log_stdout_style) {
            case WI_LOG_DAEMON:
                printf("%s %s[%u]: %s: %s\n", date, name, (uint32_t) getpid(), prefix, utf8string);
                break;
                
            case WI_LOG_TOOL:
                printf("%s: %s\n", name, utf8string);
                break;
        }
        
        fflush(stdout);
    }

    if(_wi_log_syslog_enabled)
        syslog(priority, "%s", utf8string);

    if(_wi_log_file_enabled) {
        wi_recursive_lock_lock(_wi_log_file_lock);

        path = wi_string_utf8_string(_wi_log_file_path);

        fp = fopen(path, "a");

        if(fp) {
            fprintf(fp, "%s %s[%u]: %s: %s\n", date, name, (uint32_t) getpid(), prefix, utf8string);
            fclose(fp);
            
            if(_wi_log_file_lines > 0 && _wi_log_file_limit > 0) {
                if(_wi_log_file_lines >= (int) ((double) _wi_log_file_limit * 1.1)) {
                    _wi_log_truncate_file(path);
                    
                    _wi_log_file_lines = _wi_log_file_limit;
                }
            }
            
            _wi_log_file_lines++;
        } else {
            fprintf(stderr, "%s: %s: %s\n", name, path, strerror(errno));
        }

        wi_recursive_lock_unlock(_wi_log_file_lock);
    }

    if(_wi_log_callback_enabled) {
        _wi_log_in_callback = true;
        (*_wi_log_callback_function)(level, string);
        _wi_log_in_callback = false;
    }
    
    if(level == WI_LOG_FATAL)
        exit(1);

    wi_release(string);
}



static void _wi_log_get_date(char *string) {
    struct tm   tm;
    time_t      now;

    now = time(NULL);
    localtime_r(&now, &tm);
    strftime(string, _WI_LOG_DATE_SIZE, "%b %e %H:%M:%S", &tm);
}



static void _wi_log_truncate_file(const char *path) {
    wi_file_t       *file = NULL;
    FILE            *fp = NULL, *tmp = NULL;
    struct stat     sb;
    char            buffer[BUFSIZ];
    wi_integer_t    position, lines;
    int             n, ch = EOF;

    if(stat(path, &sb) < 0 || sb.st_size == 0)
        return;

    fp = fopen(path, "r");

    if(!fp)
        goto end;

    file = wi_file_init_temporary_file(wi_file_alloc());
    
    if(!file)
        goto end;
    
    tmp = fdopen(wi_file_descriptor(file), "w+");

    if(!tmp)
        goto end;

    lines = _wi_log_file_limit;

    for(position = sb.st_size - 2; lines > 0 && position >= 0; position--) {
        if(fseeko(fp, position, SEEK_SET) < 0)
            goto end;

        ch = fgetc(fp);

        if(ch == '\n')
            lines--;
        else if(ch == EOF && ferror(fp))
            goto end;
    }

    if(position < 0 && lines > 0 && ch != EOF)
        ungetc(ch, fp);

    while((n = fread(buffer, 1, sizeof(buffer), fp)))
        fwrite(buffer, 1, n, tmp);

    fp = freopen(path, "w", fp);

    if(!fp)
        goto end;

    rewind(tmp);

    while((n = fread(buffer, 1, sizeof(buffer), tmp)))
        fwrite(buffer, 1, n, fp);

end:
    if(fp)
        fclose(fp);

    wi_release(file);
}



#pragma mark -

void wi_log_debug(wi_string_t *fmt, ...) {
    va_list     ap;

    if(_wi_log_level >= WI_LOG_DEBUG) {
        va_start(ap, fmt);
        _wi_log_vlog(WI_LOG_DEBUG, fmt, ap);
        va_end(ap);
    }
}



void wi_log_info(wi_string_t *fmt, ...) {
    va_list     ap;

    if(_wi_log_level >= WI_LOG_INFO) {
        va_start(ap, fmt);
        _wi_log_vlog(WI_LOG_INFO, fmt, ap);
        va_end(ap);
    }
}



void wi_log_warn(wi_string_t *fmt, ...) {
    va_list     ap;

    if(_wi_log_level >= WI_LOG_WARN) {
        va_start(ap, fmt);
        _wi_log_vlog(WI_LOG_WARN, fmt, ap);
        va_end(ap);
    }
}



void wi_log_error(wi_string_t *fmt, ...) {
    va_list     ap;

    if(_wi_log_level >= WI_LOG_ERROR) {
        va_start(ap, fmt);
        _wi_log_vlog(WI_LOG_ERROR, fmt, ap);
        va_end(ap);
    }
}



void wi_log_fatal(wi_string_t *fmt, ...) {
    va_list     ap;

    va_start(ap, fmt);
    _wi_log_vlog(WI_LOG_FATAL, fmt, ap);
    va_end(ap);
}
