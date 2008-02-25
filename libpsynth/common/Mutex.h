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

#ifndef PSYNTH_MUTEX_H
#define PSYNTH_MUTEX_H

#include <pthread.h>
#include <errno.h>

namespace psynth
{

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

    bool lock() const {
	if (pthread_mutex_lock((pthread_mutex_t*)&mutex) == EDEADLK)
	    return true;
	return false;
    }

    bool tryLock() const {
	if (pthread_mutex_trylock((pthread_mutex_t*)&mutex) == EBUSY)
	    return false;
	return true;
    }

#if 0
    bool timedLock(const timespec& delay) const {
	if (pthread_mutex_timedlock_np((pthread_mutex_t*)&mutex, &delay) == EBUSY)
	    return false;
	return true;
    }
#endif

    void unlock() const {
	pthread_mutex_unlock((pthread_mutex_t*)&mutex);
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

    void readLock() const {
	pthread_rwlock_rdlock((pthread_rwlock_t*)&rwlock);
    }
    
    void writeLock() const {
	pthread_rwlock_wrlock((pthread_rwlock_t*)&rwlock);
    }

    bool tryReadLock() const {
	if (pthread_rwlock_tryrdlock((pthread_rwlock_t*)&rwlock) == EBUSY)
	    return false;
	return true;
    }

    bool tryWriteLock() {
	if (pthread_rwlock_trywrlock((pthread_rwlock_t*)&rwlock) == EBUSY)
	    return false;
	return true;
    }

#if 0
    bool timedReadLock(const struct timespec* delay) const {
	if (pthread_rwlock_timedrdlock_np((pthread_rwlock_t*)&rwlock, delay) == EBUSY)
	    return false;
	return true;
    }
    
    bool timedWriteLock(const timespec& delay) const {
	if (pthread_rwlock_timedwrlock_np((pthread_rwlock_t*)&rwlock, &delay) == EBUSY)
	    return false;
	return true;
    }
#endif
    
    void unlock() const {
	pthread_rwlock_unlock((pthread_rwlock_t*)&rwlock);
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

    void broadcast() const {
	pthread_cond_broadcast((pthread_cond_t*)&cond);
    }

    void signal() const {
	pthread_cond_signal((pthread_cond_t*)&cond);
    }

    void wait(Mutex& mutex) const {
	pthread_cond_wait((pthread_cond_t*)&cond,
			  (pthread_mutex_t*)&mutex.mutex);
    }
    
    bool timedWait(Mutex& mutex, const timespec& abstime) const {
	if (pthread_cond_timedwait((pthread_cond_t*)&cond,
				   (pthread_mutex_t*)&mutex.mutex,
				   &abstime) == ETIMEDOUT)
	    return false;
	return true;
    }

#if 0
    static void calcExpiration(const timespec& delta,
			       timespec& abstime) const {
	pthread_get_expiration_np(&delta, &abstime);
    }
#endif
};

} /* namespace psynth */

#endif /* PSYNTH_MUTEX_H */
