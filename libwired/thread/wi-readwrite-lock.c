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

#ifndef WI_PTHREADS

int wi_readwrite_lock_dummy = 1;

#else

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <wired/wi-assert.h>
#include <wired/wi-date.h>
#include <wired/wi-readwrite-lock.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>
#include <wired/wi-runtime.h>

struct _wi_readwrite_lock {
    wi_runtime_base_t                   base;
    
    pthread_rwlock_t                    rwlock;
};

static void                             _wi_readwrite_lock_dealloc(wi_runtime_instance_t *);

static wi_runtime_id_t                  _wi_readwrite_lock_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_readwrite_lock_runtime_class = {
    "wi_readwrite_lock_t",
    _wi_readwrite_lock_dealloc,
    NULL,
    NULL,
    NULL,
    NULL
};



void wi_readwrite_lock_register(void) {
    _wi_readwrite_lock_runtime_id = wi_runtime_register_class(&_wi_readwrite_lock_runtime_class);
}



void wi_readwrite_lock_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_readwrite_lock_runtime_id(void) {
    return _wi_readwrite_lock_runtime_id;
}



#pragma mark -

wi_readwrite_lock_t * wi_readwrite_lock_alloc(void) {
    return wi_runtime_create_instance(_wi_readwrite_lock_runtime_id, sizeof(wi_readwrite_lock_t));
}



wi_readwrite_lock_t * wi_readwrite_lock_init(wi_readwrite_lock_t *lock) {
    int     err;
    
    if((err = pthread_rwlock_init(&lock->rwlock, NULL)) != 0)
        WI_ASSERT(false, "pthread_rwlock_init: %s", strerror(err));
    
    return lock;
}



#pragma mark -

static void _wi_readwrite_lock_dealloc(wi_runtime_instance_t *instance) {
    wi_readwrite_lock_t     *lock = instance;
    int                     err;
    
    if((err = pthread_rwlock_destroy(&lock->rwlock)) != 0)
        WI_ASSERT(false, "pthread_rwlock_destroy: %s", strerror(err));
}



#pragma mark -

void wi_readwrite_lock_write_lock(wi_readwrite_lock_t *lock) {
    int     err;
    
    if((err = pthread_rwlock_wrlock(&lock->rwlock)) != 0)
        WI_ASSERT(false, "pthread_rwlock_wrlock: %s", strerror(err));
}



wi_boolean_t wi_readwrite_lock_try_write_lock(wi_readwrite_lock_t *lock) {
    return (pthread_rwlock_trywrlock(&lock->rwlock) == 0);
}



void wi_readwrite_lock_read_lock(wi_readwrite_lock_t *lock) {
    int     err;
    
    if((err = pthread_rwlock_rdlock(&lock->rwlock)) != 0)
        WI_ASSERT(false, "pthread_rwlock_rdlock: %s", strerror(err));
}



wi_boolean_t wi_readwrite_lock_try_read_lock(wi_readwrite_lock_t *lock) {
    return (pthread_rwlock_tryrdlock(&lock->rwlock) == 0);
}



void wi_readwrite_lock_unlock(wi_readwrite_lock_t *lock) {
    int     err;
    
    if((err = pthread_rwlock_unlock(&lock->rwlock)) != 0)
        WI_ASSERT(false, "pthread_rwlock_unlock: %s", strerror(err));
}

#endif
