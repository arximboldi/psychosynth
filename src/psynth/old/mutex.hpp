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

/**
 * A  mutex  is  a  MUTual  EXclusion device, and is useful for protecting
 * shared data structures from concurrent modifications, and  implementing
 * critical sections and monitors.
 *
 * A  mutex  has  two possible states: unlocked (not owned by any thread),
 * and locked (owned by one thread). A mutex can never  be  owned  by  two
 * different  threads  simultaneously. A thread attempting to lock a mutex
 * that is already locked by another thread is suspended until the  owning
 * thread unlocks the mutex first.
 */
class mutex
{
    friend class condition;

    pthread_mutex_t m_mutex;
    pthread_mutexattr_t attr;

    void init (int type) {
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, type);
	pthread_mutex_init(&m_mutex, &attr);
    }

    void destroy () {
	pthread_mutexattr_destroy(&attr);
	pthread_mutex_destroy(&m_mutex);
    }

public:

    /**
     * Mutex type.
     */
    enum type {
	/** Default mutex.*/
	DEFAULT = PTHREAD_MUTEX_DEFAULT,
	/** Fast implementation but does not allow recursive locking. */
	FAST = PTHREAD_MUTEX_NORMAL,
	/** Mutex that allows recursive locks. */
	RECURSIVE = PTHREAD_MUTEX_RECURSIVE,
	/** Error checking mutex. */
	ERRORCHECK = PTHREAD_MUTEX_ERRORCHECK
    };

    /**
     * Constuctor.
     * @param Mutex type. Defaults to @c DEFAULT.
     */
    mutex (int type = DEFAULT) {
	init(type);
    }

    /** Destructor. */
    ~mutex () {
	destroy();
    }

    /**
     * Rebuilds the mutex.
     * @param type New mutex type.
     */
    void reset (int type = DEFAULT) {
	destroy();
	init(type);
    }

    /**
     * Locks the mutex. If a second thread calls @c lock it will block until
     * the owner of the mutex -the first thread that called lock- calls
     * @c unlock.
     */
    bool lock () const {
	if (pthread_mutex_lock((pthread_mutex_t*) &m_mutex) == EDEADLK)
	    return true;
	return false;
    }

    /**
     * Tries to lock the mutex.
     * @return @c true if we now own the mutex or @c false if the mutex is
     * already locked.
     */
    bool try_lock () const {
	if (pthread_mutex_trylock((pthread_mutex_t*)&m_mutex) == EBUSY)
	    return false;
	return true;
    }

#if 0
    bool timed_lock (const timespec& delay) const {
	if (pthread_mutex_timedlock_np((pthread_mutex_t*)&m_mutex, &delay) == EBUSY)
	    return false;
	return true;
    }
#endif

    /**
     * Unlocks the mutex.
     */
    void unlock() const {
	pthread_mutex_unlock((pthread_mutex_t*)&m_mutex);
    }
};

/**
 * A lock that implements mutual exclusion for writers but allows multiple
 * readers.
 * @see Mutex
 */
class rwlock
{
    pthread_rwlock_t m_rwlock;
    // pthread_rwlockattr_t attr;

    void init (int type) {
	/*
	pthread_rwlockattr_init(&attr);
	pthread_rwlockattr_settype(&attr, type);
	*/
	pthread_rwlock_init(&m_rwlock, NULL);
    }

    void destroy () {
	/*
	pthread_rwlockattr_destroy(&attr);
	*/
	pthread_rwlock_destroy(&m_rwlock);
    }

public:
    enum type {
	DEFAULT
#if 0
	/** Default @c RWLock type. */
	DEFAULT = PTHREAD_RWLOCK_DEFAULT,
	/** Give preference to readers. */
	PREFER_READER = PTHREAD_RWLOCK_PREFER_READER,
	/** Give preference to writers. */
	PREFER_WRITER = PTHREAD_RWLOCK_PREFER_WRITER,
	/** Total number of @c RWLock types. */
	N_TYPE
#endif
    };

    /**
     * Constructor.
     * @param type The type of lock, defaults to @c DEFAULT.
     */
    rwlock(int type = DEFAULT) {
	init(type);
    }

    /** Destructor. */
    ~rwlock() {
	destroy();
    }

    /**
     * Locks as a reader. It will block it is currently locked
     * by a writer or if any writer is willing to lock it when using
     * @c PREFER_WRITER type.
     */
    void read_lock() const {
	pthread_rwlock_rdlock((pthread_rwlock_t*)&m_rwlock);
    }

    /**
     * Locks as a writer. It will block if the lock is locked is
     * already locked by anyone, reader or writer.
     */
    void write_lock() const {
	pthread_rwlock_wrlock((pthread_rwlock_t*)&m_rwlock);
    }

    /**
     * Tries to do a @c readLock.
     * @return @c true if we could lock the @c RWLock and @c false if it was already
     * locked.
     */
    bool try_read_lock() const {
	if (pthread_rwlock_tryrdlock((pthread_rwlock_t*)&m_rwlock) == EBUSY)
	    return false;
	return true;
    }

    /**
     * Tries to do a @c writeLock.
     * @return @c true if we could lock the @c RWLock and @c false if it was already
     * locked.
     */
    bool try_write_lock() {
	if (pthread_rwlock_trywrlock((pthread_rwlock_t*)&m_rwlock) == EBUSY)
	    return false;
	return true;
    }

#if 0
    bool timed_read_lock(const struct timespec* delay) const {
	if (pthread_rwlock_timedrdlock_np((pthread_rwlock_t*)&m_rwlock, delay) == EBUSY)
	    return false;
	return true;
    }

    bool timed_write_lock(const timespec& delay) const {
	if (pthread_rwlock_timedwrlock_np((pthread_rwlock_t*)&m_rwlock, &delay) == EBUSY)
	    return false;
	return true;
    }
#endif

    /**
     * Unlocks the @c RWLock.
     */
    void unlock () const {
	pthread_rwlock_unlock((pthread_rwlock_t*)&m_rwlock);
    }
};

/**
 * Condition variable.
 */
class condition
{
    pthread_cond_t cond;

public:
    /** Constructor. */
    condition () {
	pthread_cond_init(&cond, NULL);
    }

    /** Destructor. */
    ~condition () {
	pthread_cond_destroy(&cond);
    }

    /**
     * Restarts threads that are waiting on the condition variable.
     */
    void broadcast () const {
	pthread_cond_broadcast((pthread_cond_t*)&cond);
    }

    /**
     * Restarts one thread that is waiting on the condition variable.
     */
    void signal () const {
	pthread_cond_signal((pthread_cond_t*)&cond);
    }

    /**
     * Starts waiting on the condition. First it unlocks the @a mutex -which
     * @b must be previously locked- and blocks on the condition until it is
     * signaled. The @a mutex is locked again before returning.
     * @param mutex A lock.
     */
    void wait (mutex& mutex) const {
	pthread_cond_wait((pthread_cond_t*)&cond,
			  (pthread_mutex_t*)&mutex.m_mutex);
    }

    /**
     * Waits until the condition is signaled or the time has expired.
     * @param mutex The mutex for the synchronization protocol, used in the same
     * way as in @c wait.
     * @param abstime The maximum time to wait.
     * @return True if the mutex was signaled during the wait or false if
     * we returned because of a timeout.
     * @see wait
     */
    bool timed_wait(mutex& mutex, const timespec& abstime) const {
	if (pthread_cond_timedwait((pthread_cond_t*)&cond,
				   (pthread_mutex_t*)&mutex.m_mutex,
				   &abstime) == ETIMEDOUT)
	    return false;
	return true;
    }

#if 0
    static void calc_expiration(const timespec& delta,
			       timespec& abstime) const {
	pthread_get_expiration_np(&delta, &abstime);
    }
#endif
};

} /* namespace psynth */

#endif /* PSYNTH_MUTEX_H */
