/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef MUTEX_H
#define MUTEX_H

#include <pthread.h>
#include <errno.h>

class Mutex
{
    friend class Condition;
    
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
    
    void init(int type) {
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, type);
	pthread_mutex_init(&mutex, &attr);
    }

    void destroy() {
	pthread_mutexattr_destroy(&attr);
	pthread_mutex_destroy(&mutex);
    }
    
public:
    enum {
	DEFAULT = PTHREAD_MUTEX_DEFAULT,
	FAST = PTHREAD_MUTEX_NORMAL,
	RECURSIVE = PTHREAD_MUTEX_RECURSIVE,
	ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK
    };
    
    Mutex(int type = DEFAULT) {
	init(type);
    }

    void reset(int type = DEFAULT) {
	destroy();
	init(type);
    }
    
    ~Mutex() {
	destroy();
    }

    bool lock() {
	if (pthread_mutex_lock(&mutex) == EDEADLK)
	    return true;
	return false;
    }

    bool tryLock() {
	if (pthread_mutex_trylock(&mutex) == EBUSY)
	    return false;
	return true;
    }

#if 0
    bool timedLock(const timespec& delay) {
	if (pthread_mutex_timedlock_np(&mutex, &delay) == EBUSY)
	    return false;
	return true;
    }
#endif

    void unlock() {
	pthread_mutex_unlock(&mutex);
    }
};

class RWLock
{
    pthread_rwlock_t rwlock;    
    
public:
    RWLock() {
	pthread_rwlock_init(&rwlock, NULL);
    }

    ~RWLock() {
	pthread_rwlock_destroy(&rwlock);
    }

    void readLock() {
	pthread_rwlock_rdlock(&rwlock);
    }
    
    void writeLock() {
	pthread_rwlock_wrlock(&rwlock);
    }

    bool tryReadLock() {
	if (pthread_rwlock_tryrdlock(&rwlock) == EBUSY)
	    return false;
	return true;
    }

    bool tryWriteLock() {
	if (pthread_rwlock_trywrlock(&rwlock) == EBUSY)
	    return false;
	return true;
    }

#if 0
    bool timedReadLock(const struct timespec* delay) {
	if (pthread_rwlock_timedrdlock_np(&rwlock, delay) == EBUSY)
	    return false;
	return true;
    }
    
    bool timedWriteLock(const timespec& delay) {
	if (pthread_rwlock_timedwrlock_np(&rwlock, &delay) == EBUSY)
	    return false;
	return true;
    }
#endif
    
    void unlock() {
	pthread_rwlock_unlock(&rwlock);
    }
};

class Condition
{
    pthread_cond_t cond;

public:
    Condition() {
	pthread_cond_init(&cond, NULL);
    }

    ~Condition() {
	pthread_cond_destroy(&cond);
    }

    void broadcast() {
	pthread_cond_broadcast(&cond);
    }

    void signal() {
	pthread_cond_signal(&cond);
    }

    void wait(Mutex& mutex) {
	pthread_cond_wait(&cond, &mutex.mutex);
    }

    
    bool timedWait(Mutex& mutex, const timespec& abstime) {
	if (pthread_cond_timedwait(&cond, &mutex.mutex, &abstime) == ETIMEDOUT)
	    return false;
	return true;
    }

#if 0
    static void calcExpiration(const timespec& delta,
			       timespec& abstime) {
	pthread_get_expiration_np(&delta, &abstime);
    }
#endif
};

#endif /* MUTEX_H */
