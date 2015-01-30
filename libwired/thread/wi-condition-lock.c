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

int wi_condition_lock_dummy = 1;

#else

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#include <wired/wi-assert.h>
#include <wired/wi-date.h>
#include <wired/wi-condition-lock.h>
#include <wired/wi-private.h>
#include <wired/wi-string.h>
#include <wired/wi-runtime.h>

struct _wi_condition_lock {
    wi_runtime_base_t                   base;

    pthread_mutex_t                     mutex;
    pthread_cond_t                      cond;
    
    int                                 condition;
};

static void                             _wi_condition_lock_dealloc(wi_runtime_instance_t *);

static wi_runtime_id_t                  _wi_condition_lock_runtime_id = WI_RUNTIME_ID_NULL;
static wi_runtime_class_t               _wi_condition_lock_runtime_class = {
    "wi_condition_lock_t",
    _wi_condition_lock_dealloc,
    NULL,
    NULL,
    NULL,
    NULL
};



void wi_condition_lock_register(void) {
    _wi_condition_lock_runtime_id = wi_runtime_register_class(&_wi_condition_lock_runtime_class);
}



void wi_condition_lock_initialize(void) {
}



#pragma mark -

wi_runtime_id_t wi_condition_lock_runtime_id(void) {
    return _wi_condition_lock_runtime_id;
}



#pragma mark -

wi_condition_lock_t * wi_condition_lock_alloc(void) {
    return wi_runtime_create_instance(_wi_condition_lock_runtime_id, sizeof(wi_condition_lock_t));
}



wi_condition_lock_t * wi_condition_lock_init(wi_condition_lock_t *lock) {
    return wi_condition_lock_init_with_condition(lock, 0);
}



wi_condition_lock_t * wi_condition_lock_init_with_condition(wi_condition_lock_t *lock, int condition) {
    pthread_mutexattr_t     attr;
    int                     err;
    
    if((err = pthread_mutexattr_init(&attr)) != 0)
        WI_ASSERT(false, "pthread_mutexattr_init: %s", strerror(err));
    if((err = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK)) != 0)
        WI_ASSERT(false, "pthread_mutexattr_settype: %s", strerror(err));
    if((err = pthread_mutex_init(&lock->mutex, &attr)) != 0)
        WI_ASSERT(false, "pthread_mutex_init: %s", strerror(err));
    if((err = pthread_mutexattr_destroy(&attr)) != 0)
        WI_ASSERT(false, "pthread_mutexattr_destroy: %s", strerror(err));

    if((err = pthread_cond_init(&lock->cond, NULL)) != 0)
        WI_ASSERT(false, "pthread_cond_init: %s", strerror(err));
    
    lock->condition = condition;

    return lock;
}



#pragma mark -

static void _wi_condition_lock_dealloc(wi_runtime_instance_t *instance) {
    wi_condition_lock_t     *lock = instance;
    int                     err;

    if((err = pthread_mutex_destroy(&lock->mutex)) != 0)
        WI_ASSERT(false, "pthread_mutex_destroy: %s", strerror(err));
    
    if((err = pthread_cond_destroy(&lock->cond)) != 0)
        WI_ASSERT(false, "pthread_cond_destroy: %s", strerror(err));
}



#pragma mark -

void wi_condition_lock_lock(wi_condition_lock_t *lock) {
    int     err;
    
    if((err = pthread_mutex_lock(&lock->mutex)) != 0)
        WI_ASSERT(false, "pthread_mutex_lock: %s", strerror(err));
}



wi_boolean_t wi_condition_lock_lock_when_condition(wi_condition_lock_t *lock, int condition, wi_time_interval_t time) {
    struct timespec     ts;
    int                 err;
    
    if((err = pthread_mutex_lock(&lock->mutex)) != 0)
        WI_ASSERT(false, "pthread_mutex_lock: %s", strerror(err));
    
    if(lock->condition != condition) {
        if(time > 0.0) {
            ts = wi_dtots(wi_time_interval() + time);
            
            do {
                err = pthread_cond_timedwait(&lock->cond, &lock->mutex, &ts);

                if(err != 0 && err != ETIMEDOUT)
                    WI_ASSERT(false, "pthread_cond_timedwait: %s", strerror(err));
            } while(lock->condition != condition && err != ETIMEDOUT);

            if(err == ETIMEDOUT) {
                if((err = pthread_mutex_unlock(&lock->mutex)) != 0)
                    WI_ASSERT(false, "pthread_mutex_unlock: %s", strerror(err));
                
                return false;
            }
        } else {
            do {
                if((err = pthread_cond_wait(&lock->cond, &lock->mutex)) != 0)
                    WI_ASSERT(false, "pthread_cond_wait: %s", strerror(err));
            } while(lock->condition != condition);
        }
    }
    
    return true;
}



wi_boolean_t wi_condition_lock_try_lock(wi_condition_lock_t *lock) {
    return (pthread_mutex_trylock(&lock->mutex) == 0);
}



wi_boolean_t wi_condition_lock_try_lock_when_condition(wi_condition_lock_t *lock, int condition) {
    if(!wi_condition_lock_try_lock(lock))
        return false;
    
    if(lock->condition == condition) {
        return true;
    } else {
        wi_condition_lock_unlock(lock);
        
        return false;
    }
}



void wi_condition_lock_unlock(wi_condition_lock_t *lock) {
    int     err;
    
    if((err = pthread_cond_broadcast(&lock->cond)) != 0)
        WI_ASSERT(false, "pthread_cond_broadcast: %s", strerror(err));

    if((err = pthread_mutex_unlock(&lock->mutex)) != 0)
        WI_ASSERT(false, "pthread_mutex_unlock: %s", strerror(err));
}



void wi_condition_lock_unlock_with_condition(wi_condition_lock_t *lock, int condition) {
    int     err;
    
    lock->condition = condition;
    
    if((err = pthread_cond_broadcast(&lock->cond)) != 0)
        WI_ASSERT(false, "pthread_cond_broadcast: %s", strerror(err));
    
    if((err = pthread_mutex_unlock(&lock->mutex)) != 0)
        WI_ASSERT(false, "pthread_mutex_unlock: %s", strerror(err));
}



int wi_condition_lock_condition(wi_condition_lock_t *lock) {
    return lock->condition;
}

#endif

