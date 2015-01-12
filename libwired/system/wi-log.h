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

#ifndef WI_LOG_H
#define WI_LOG_H 1

#include <wired/wi-base.h>

#define WI_LOG_INSTANCE(object) \
    wi_log_info(WI_STR("%s = %@"), #object, (object))


enum _wi_log_level {
    WI_LOG_FATAL                    = 0,
    WI_LOG_ERROR,
    WI_LOG_WARN,
    WI_LOG_INFO,
    WI_LOG_DEBUG
};
typedef enum _wi_log_level          wi_log_level_t;

enum _wi_log_style {
    WI_LOG_DAEMON,
    WI_LOG_TOOL
};
typedef enum _wi_log_style          wi_log_style_t;


typedef void                        wi_log_callback_func_t(wi_log_level_t, wi_string_t *);


WI_EXPORT void                      wi_log_set_level(wi_log_level_t);
WI_EXPORT wi_log_level_t            wi_log_level(void);

WI_EXPORT void                      wi_log_add_stdout_logger(wi_log_style_t);
WI_EXPORT void                      wi_log_add_file_logger(wi_string_t *, wi_uinteger_t);
WI_EXPORT void                      wi_log_add_syslog_logger(int);
WI_EXPORT void                      wi_log_add_callback_logger(wi_log_callback_func_t);

WI_EXPORT int                       wi_log_syslog_facility_with_name(wi_string_t *);

WI_EXPORT void                      wi_log_debug(wi_string_t *, ...);
WI_EXPORT void                      wi_log_info(wi_string_t *, ...);
WI_EXPORT void                      wi_log_warn(wi_string_t *, ...);
WI_EXPORT void                      wi_log_error(wi_string_t *, ...);
WI_EXPORT void                      wi_log_fatal(wi_string_t *, ...);

#endif /* WI_LOG_H */
