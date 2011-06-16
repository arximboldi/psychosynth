/**
 *  Time-stamp:  <2011-06-13 11:10:35 raskolnikov>
 *
 *  @file        threads.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Apr  7 11:29:08 2009
 *
 *  This module provides simplified and abstracted threading models
 *  that can be used for policy-based designs. It is inspired
 *  by Andrei Alexandrescu's implementation at his book Modern C++ Design.
 *
 *  To make the code directly compatible with Boost.Threads we use
 *  the Boost mutex interface as threading policy. As such, a MutexPolicy
 *  must have the following two methods:
 *
 *       - Mutex::lock ()    -> Locks the mutex
 *       - Mutex::unlock ()  -> Unlocks the mutex
 *
 *  A mutex must also the find, as boost::mutex does, a exception safe
 *  RAII wrapper to lock the mutex under the name of:
 *
 *       - Mutex::scoped_lock with at least one constructor accepting
 *         a reference to a Mutex instance.
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_THREADS_H_
#define PSYNTH_THREADS_H_

#include <cassert>
#include <psynth/base/util.hpp>

#define PSYNTH_DEFAULT_THREADING         \
    psynth::base::default_object_lockable::type
#define PSYNTH_DEFAULT_NONOBJ_THREADING  \
    psynth::base::default_class_lockable::type

#ifndef PSYNTH_DEFAULT_MUTEX
#include <mutex>
#define PSYNTH_DEFAULT_MUTEX             std::recursive_mutex
#endif

namespace psynth
{
namespace base
{

/**
 * Utility to avoid writing too many types when locking mutexes.
 */
template <class Mutex, typename... Args>
std::unique_lock<Mutex> make_unique_lock (Mutex& lock, Args... args)
{
    return std::unique_lock<Mutex> (lock, args...);
}

/**
 *  try_lock_guard, lock_guard and no_lock_guard provide a way to
 *  design policies that are not bases just on object or class
 *  locking, but fine-grainedly control wether some internal construct
 *  should have exclusion, probabilistic exclusion or no exclusion at
 *  all.
 */
template <class Mutex = std::mutex>
struct try_lock_guard
{
    typedef std::unique_lock<Mutex> guard_type;

    guard_type default_guard ()
    { return guard_type (_mutex, std::try_to_lock); }

    template <typename... Args>
    guard_type guard (Args... args)
    { return guard_type (_mutex, args...); }

    void lock ()
    { _mutex.lock (); }

    bool try_lock ()
    { return _mutex.try_lock (); }

    void unlock ()
    { _mutex.unlock (); }
    
private:
    Mutex _mutex;
};


/**
 * @see try_lock_guard
 */
template <class Mutex = std::mutex>
struct lock_guard : public try_lock_guard<Mutex>
{
    typedef std::unique_lock<Mutex> guard_type;
    
    guard_type default_guard ()
    { return guard_type (_mutex); }

private:
    Mutex _mutex;
};


/**
 * @see try_lock_guard
 */
template <class Mutex = std::mutex>
struct no_lock_guard
{
    struct guard_type
    {
        bool owns_lock () { return true; }        
    };
    
    guard_type default_guard () { return {}; }
    template <typename... Args>
    guard_type guard (Args...) { assert (false); return {}; }
    void lock () { assert (false); }
    bool try_lock () { assert (false); return false; }
    void unlock () { assert (false); };
};


template <class T>
class no_threading
{
public:
    typedef T volatile_type;

    struct lock
    {
	lock () {}
	explicit lock (const no_threading& host) {}
	explicit lock (const no_threading* host) {}
    };
	
private:
};

template <class T>
class lockable_base
{
public:
    typedef volatile T volatile_type;
};

template <class T, class Mutex = PSYNTH_DEFAULT_MUTEX>
class object_lockable : public lockable_base<T>
{
public:
    class lock : public boost::noncopyable
    {
    public:
	lock (const object_lockable& host)
	    : _host (host)
	{ _host._mutex.lock (); }

	lock (const object_lockable* host)
	    : _host (*host)
	{ _host._mutex.lock (); }
	
	~lock ()
	{ _host._mutex.unlock (); }

    private:
	const object_lockable& _host;
    };
    
private:
    mutable Mutex _mutex;
};

template <class T, class Mutex = PSYNTH_DEFAULT_MUTEX>
class class_lockable : public lockable_base<T>
{
public:
    class lock : public boost::noncopyable
    {
    public:
	lock ()
	{ s_init._mutex.lock (); }

	lock (const class_lockable& host)
	{ s_init._mutex.lock (); }

	lock (const class_lockable* host)
	{ s_init._mutex.lock (); }
	
	~lock ()
	{ s_init._mutex.unlock (); }
    };
	
private:
    struct initializer
    {
	bool  _is_init;
	Mutex _mutex;

	initializer ()
	    : _is_init (false)
	{ _is_init = true; }

	~initializer ()
	{ assert (_is_init); }
    };

    static initializer s_init;
};

template<class T, class M>
typename class_lockable<T, M>::initializer class_lockable<T, M>::s_init;

typedef
tpl_bind_snd<object_lockable, PSYNTH_DEFAULT_MUTEX>
default_object_lockable;

typedef
tpl_bind_snd<class_lockable, PSYNTH_DEFAULT_MUTEX>
default_class_lockable;

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_THREADS_H_ */
