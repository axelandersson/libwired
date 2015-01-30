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

#ifndef WI_FILESYSTEM_EVENTS

int wi_filesystem_events_dummy = 1;

#else

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifdef HAVE_SYS_EVENT_H
#include <sys/event.h>
#endif

#ifdef HAVE_SYS_INOTIFY_H
#include <sys/inotify.h>
#endif

#ifdef HAVE_INOTIFYTOOLS_INOTIFY_H
#include <inotifytools/inotify.h>
#endif

#include <wired/wi-error.h>
#include <wired/wi-filesystem-events.h>
#include <wired/wi-pool.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>

#if defined(HAVE_SYS_EVENT_H)
#define _WI_FILESYSTEM_EVENTS_KQUEUE        1
#elif defined(HAVE_SYS_INOTIFY_H) || defined(HAVE_INOTIFYTOOLS_INOTIFY_H)
#define _WI_FILESYSTEM_EVENTS_INOTIFY       1
#endif

#ifdef _WI_FILESYSTEM_EVENTS_INOTIFY
#define _WI_FILESYSTEM_EVENTS_INOTIFY_MASK  (IN_CREATE | IN_DELETE | IN_MOVE)
#endif


struct _wi_filesystem_events {
    wi_runtime_base_t                       base;
    
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    int                                     kqueue;
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    int                                     inotify;
#endif
    
    wi_filesystem_events_callback_t         *callback;
    wi_mutable_set_t                        *paths;
    wi_mutable_dictionary_t                 *fds_for_paths;

#ifdef _WI_FILESYSTEM_EVENTS_INOTIFY
    wi_mutable_dictionary_t                 *paths_for_fds;
#endif
};


static void                                 _wi_filesystem_events_dealloc(wi_runtime_instance_t *);


static wi_runtime_id_t                      _wi_filesystem_events_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t                   _wi_filesystem_events_runtime_class = {
    "wi_filesystem_events_t",
    _wi_filesystem_events_dealloc,
    NULL,
    NULL,
    NULL,
    NULL
};



void wi_filesystem_events_register(void) {
    _wi_filesystem_events_runtime_id = wi_runtime_register_class(&_wi_filesystem_events_runtime_class);
}



void wi_filesystem_events_initialize(void) {
}



#pragma mark -    

wi_runtime_id_t wi_filesystem_events_runtime_id(void) {
    return _wi_filesystem_events_runtime_id;
}



#pragma mark -

wi_filesystem_events_t * wi_filesystem_events(void) {
    return wi_autorelease(wi_filesystem_events_init(wi_filesystem_events_alloc()));
}



#pragma mark -

wi_filesystem_events_t * wi_filesystem_events_alloc(void) {
    return wi_runtime_create_instance(_wi_filesystem_events_runtime_id, sizeof(wi_filesystem_events_t));
}



wi_filesystem_events_t * wi_filesystem_events_init(wi_filesystem_events_t *filesystem_events) {
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    filesystem_events->kqueue = kqueue();
    
    if(filesystem_events->kqueue < 0) {
        wi_error_set_errno(errno);
        
        wi_release(filesystem_events);
        
        return NULL;
    }
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    filesystem_events->inotify = inotify_init();

    if(filesystem_events->inotify < 0) {
        wi_error_set_errno(errno);
        
        wi_release(filesystem_events);
        
        return NULL;
    }
#endif
    
    filesystem_events->paths = wi_set_init_with_capacity(wi_mutable_set_alloc(), 0, true);
    filesystem_events->fds_for_paths = wi_dictionary_init_with_capacity_and_callbacks(wi_mutable_dictionary_alloc(), 0,
        wi_dictionary_default_key_callbacks, wi_dictionary_null_value_callbacks);

#ifdef _WI_FILESYSTEM_EVENTS_INOTIFY
    filesystem_events->paths_for_fds = wi_dictionary_init_with_capacity_and_callbacks(wi_mutable_dictionary_alloc(), 0,
        wi_dictionary_null_key_callbacks, wi_dictionary_default_value_callbacks);
#endif
    
    return filesystem_events;
}



static void _wi_filesystem_events_dealloc(wi_runtime_instance_t *instance) {
    wi_filesystem_events_t   *filesystem_events = instance;
    
    wi_filesystem_events_remove_all_paths(filesystem_events);
    
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    close(filesystem_events->kqueue);
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    close(filesystem_events->inotify);
#endif
    
    wi_release(filesystem_events->paths);
    wi_release(filesystem_events->fds_for_paths);

#ifdef _WI_FILESYSTEM_EVENTS_INOTIFY
    wi_release(filesystem_events->paths_for_fds);
#endif
}



#pragma mark -

wi_boolean_t wi_filesystem_events_run_with_timeout(wi_filesystem_events_t *filesystem_events, wi_time_interval_t timeout) {
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    struct kevent           event;
    struct timespec         ts;
    int                     result;
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    wi_string_t             *path;
    struct inotify_event    *event;
    struct timeval          tv;
    char                    buffer[1024];
    fd_set                  rfds;
    ssize_t                 i, result;
    int                     state;
#endif
    
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    do {
        ts = wi_dtots(timeout);
        result = kevent(filesystem_events->kqueue, NULL, 0, &event, 1, (timeout > 0.0) ? &ts : NULL);
        
        if(result < 0) {
            wi_error_set_errno(errno);
            
            return false;
        }
        else if(result > 0) {
            if(event.filter == EVFILT_VNODE) {
                if(filesystem_events->callback)
                    (*filesystem_events->callback)(event.udata);
            }
        }
    } while(timeout == 0.0);
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    do {
        FD_ZERO(&rfds);
        FD_SET(filesystem_events->inotify, &rfds);

        tv = wi_dtotv(timeout);
        state = select(filesystem_events->inotify + 1, &rfds, NULL, NULL, (timeout > 0.0) ? &tv : NULL);

        if(state < 0) {
            wi_error_set_errno(errno);

            return false;
        }
        else if(state > 0) {
            result = read(filesystem_events->inotify, buffer, sizeof(buffer));

            if(result < 0) {
                wi_error_set_errno(errno);

                return false;
            }
            else if(result > 0) {
                i = 0;

                while(i < result) {
                    event = (struct inotify_event *) &buffer[i];
                    path = wi_dictionary_data_for_key(filesystem_events->paths_for_fds, (void *) (intptr_t) event->wd);

                    if(_WI_FILESYSTEM_EVENTS_INOTIFY_MASK & event->mask && path)
                        (*filesystem_events->callback)(path);

                    i += sizeof(*event) + event->len;
                }
            }
        }
    } while(timeout == 0.0);
#endif
    
    return true;
}



#pragma mark -

void wi_filesystem_events_set_callback(wi_filesystem_events_t *filesystem_events, wi_filesystem_events_callback_t *callback) {
    filesystem_events->callback = callback;
}



#pragma mark -

wi_boolean_t wi_filesystem_events_add_path(wi_filesystem_events_t *filesystem_events, wi_string_t *path) {
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    struct kevent   ev;
    int             fd;
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    int             fd;
#endif
    
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
    if(!wi_set_contains_data(filesystem_events->paths, path)) {
        fd = open(wi_string_utf8_string(path),
#ifdef O_EVTONLY
                  O_EVTONLY,
#else
                  O_RDONLY,
#endif
                  0);
        
        if(fd < 0) {
            wi_error_set_errno(errno);
            
            return false;
        }
        
        EV_SET(&ev, fd, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, NOTE_WRITE | NOTE_DELETE | NOTE_RENAME, 0, path);
        
        if(kevent(filesystem_events->kqueue, &ev, 1, NULL, 0, NULL) < 0) {
            wi_error_set_errno(errno);
            
            close(fd);
            
            return false;
        }
        
        wi_mutable_dictionary_set_data_for_key(filesystem_events->fds_for_paths, (void *) (intptr_t) fd, path);
    }
    
    wi_mutable_set_add_data(filesystem_events->paths, path);
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
    if(!wi_set_contains_data(filesystem_events->paths, path)) {
        fd = inotify_add_watch(filesystem_events->inotify, wi_string_cstring(path), _WI_FILESYSTEM_EVENTS_INOTIFY_MASK);

        if(fd < 0) {
            wi_error_set_errno(errno);
            
            return false;
        }

        wi_mutable_dictionary_set_data_for_key(filesystem_events->fds_for_paths, (void *) (intptr_t) fd, path);
        wi_mutable_dictionary_set_data_for_key(filesystem_events->paths_for_fds, path, (void *) (intptr_t) fd);
    }

    wi_mutable_set_add_data(filesystem_events->paths, path);
#endif
    
    return true;
}



void wi_filesystem_events_remove_path(wi_filesystem_events_t *filesystem_events, wi_string_t *path) {
    int     fd;
    
    if(wi_set_count_for_data(filesystem_events->paths, path) == 1) {
        fd = (int) (intptr_t) wi_dictionary_data_for_key(filesystem_events->fds_for_paths, path);
        
        if(fd == 0)
            return;
        
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
        close(fd);
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
        inotify_rm_watch(filesystem_events->inotify, fd);

        wi_mutable_dictionary_remove_data_for_key(filesystem_events->fds_for_paths, (void *) (intptr_t) path);
#endif
        
        wi_mutable_dictionary_remove_data_for_key(filesystem_events->fds_for_paths, path);
    }
    
    wi_mutable_set_remove_data(filesystem_events->paths, path);
}



void wi_filesystem_events_remove_all_paths(wi_filesystem_events_t *filesystem_events) {
    wi_enumerator_t     *enumerator;
    int                 fd;
    
    enumerator = wi_dictionary_data_enumerator(filesystem_events->fds_for_paths);
    
    while((fd = (int) (intptr_t) wi_enumerator_next_data(enumerator))) {
#if defined(_WI_FILESYSTEM_EVENTS_KQUEUE)
        close(fd);
#elif defined(_WI_FILESYSTEM_EVENTS_INOTIFY)
        inotify_rm_watch(filesystem_events->inotify, fd);
#endif
    }
    
    wi_mutable_set_remove_all_data(filesystem_events->paths);
    wi_mutable_dictionary_remove_all_data(filesystem_events->fds_for_paths);

#ifdef _WI_FILESYSTEM_EVENTS_INOTIFY
    wi_mutable_dictionary_remove_all_data(filesystem_events->paths_for_fds);
#endif
}

#endif
