/*
 *  File:       singleton.hpp
 *  Author:     Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  Date:       2007
 *  Time-stamp: <2010-10-19 01:29:13 raskolnikov>
 *
 *  This file implements a policy-based generic singleton. It is a
 *  simplification (less policy implementations are provided) of
 *  Loki singleton_holder class.
 *
 *  @todo Add more longetvity policies. The follow-to-death policy
 *        in the current Loki implementation seems interesting, as
 *        most singletons actually depend indirectly
 *        (through psynth::error) on Logger.
 */

/*
 *  Copyright (C) 2007, 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_SINGLETON_H
#define PSYNTH_SINGLETON_H

#include <new>

#include <psynth/base/exception.hpp>
#include <psynth/base/threads.hpp>

namespace psynth
{
namespace base
{

/* Default parameters forward declaration. */
template <class T>
class create_with_new;
template <class T>
class lifetime_atexit;

/**
 * This is a generic singleton implementation. Note that you should not
 * usually inherit from it but rather just typedef with T as desired
 * implementation of the singleton class. It can be parametrized with
 * varied policies:
 *
 * @param CreationPolicy  Specifies the allocation policy to use.
 * @param LifetimePolicy  Specifies the order of singleton creation and
 *                        other related issues
 * @param ThreadingPolicy Wether you want the implementation to be thread-safe
 *                        or not.
 */
template <
    class T,
    template <class> class CreationPolicy  = create_with_new,
    template <class> class LifetimePolicy  = lifetime_atexit,
    template <class> class ThreadingPolicy = PSYNTH_DEFAULT_NONOBJ_THREADING
    >
class singleton_holder
{
public:
    typedef T type;
    
    /**
     * Access the singleton only instance.
     * @return The singleton instance.
     */
    static T& self ();
    
private:
    /** Hidden constructor. */
    singleton_holder () {};
    /** Hidden destructor. */
    ~singleton_holder () {};
    /** Hidden copy constructor. */
    singleton_holder (singleton_holder const&) {};
    /** Hidden copy operator. */
    singleton_holder& operator= (singleton_holder const&) {};

    /** The single instance pointer type. */
    typedef typename ThreadingPolicy<T*>::volatile_type instance_type;
    /** The single instance pointer. */
    static instance_type m_instance;
    /** Did we kill the singleton already? */
    static bool m_destroyed;
    
    /** The destroy handler */
    static void destroy_singleton ();
};

/**
 * Creator singleton policy that uses \t new and \t delete to allocate
 * and free the singleton.
 */
template <typename T>
struct create_with_new
{
    static T* create ()
    {
	return new T;
    }
    
    static void destroy (T* var)
    {
	delete var;
    }
};

/**
 * Creator singleton policy that uses std::malloc and std::free
 * to allocate and free the singleton.
 */
template <typename T>
struct create_with_malloc
{
    /** Creator function. */
    static T* create ()
    {
	T* mem = std::malloc (sizeof (T));
	if (!mem)
	    return 0;
	mem = new (mem) T;
	return mem;
    }

    /** Destroyer function. */
    static void destroy (T* obj)
    {
	obj->~T ();
	std::free (obj);
    }
};

/**
 * Creator policy that creates the singleton in static memory.
 */
template <typename T>
struct create_with_static
{
    /** Creator function. */
    static T* create ()
    {
	static T object; /* Should we use Loki's MaxAlign trick? */
	return &object;
    }

    /** Destroyer function */
    static void destroy (T* obj)
    {
	obj->~T ();
    }
};

/**
 * Creator policy that uses a allocator satisfying the std::allocator
 * interface to build an destroy the singleton. Use the inner class
 * to instantiate it.
 * @param Alloc the actual allocator to use.
 */
template<template<class> class Alloc>
struct create_with
{
    /**
     * @see create_with
     */
    template <class T>
    struct alloc
    {
	static Alloc<T> allocator;

	/** The creator function */
	static T* create()
	{
	    return new (allocator.allocate (1)) T;
	}

	/** The destroyer function */
	static void destroy(T* p)
	{
	    p->~T();
	    allocator.deallocate (p, 1);
	}
    };
};

/**
 * Type of the destroy function to be registered on the lifetime
 * policies.
 */
typedef void (*destroy_func_t)();

/**
 * The error that lifetime policies should throw in case of dead
 * reference access.
 */
PSYNTH_DECLARE_ERROR (base_error, singleton_error);

/**
 * Lifetime singleton policy that destroys the singleton in the
 * atexit invocation sequence.
 *
 * @todo This is causing problems with pthreads doing cleanup
 *       on exit() too...
 */
template <class T>
struct lifetime_atexit
{
    /** Scheduler function. */
    static void schedule (T*, destroy_func_t fun)
    {
	std::atexit (fun);
    }

    /** Error handler function. */
    static void on_dead_reference ()
    {
	throw singleton_error ();
    }
};

/**
 * Lifetime singleton policy that destroys the singleton on the
 * static destructors invocation sequence.
 *
 * Some C++ compilers run destructors on static objects
 * after atexit. It could therefore be convenient to delay
 * destruction until then.
 */
template <class T>
struct lifetime_static_dtor
{
    /** Scheduler function. */
    static void schedule (T*, destroy_func_t fun)
    {
	static scheduler sched (fun);
    }

    /** Error handler function. */
    static void on_dead_reference ()
    {
	throw singleton_error ();
    }

private:
    /** Helper object. */
    struct scheduler
    {
	scheduler (destroy_func_t fun)
	    : m_fun (fun)
	{}

	~scheduler ()
	{
	    m_fun ();
	}

	destroy_func_t m_fun;
    };
};

/**
 * Singleton lifetime policy that re-creates the singleton if it has
 * already been destroyed.
 *
 * @todo Build autotool macro for PSYNTH_ATEXIT_FIXED
 */
template <class T>
class lifetime_phoenix
{
public:
    /** Scheduler funcion */
    static void schedule (T*, destroy_func_t pFun)
    {
#ifndef PSYNTH_ATEXIT_FIXED
	if (!m_destroyed)
#endif
	    std::atexit(pFun);
    }

    /** Error handler function */
    static void on_dead_reference ()
    {
#ifndef PSYNTH_ATEXIT_FIXED
	m_destroyed = true;
#endif
    }
        
private:
#ifndef PSYNTH_ATEXIT_FIXED
    static bool m_destroyed;
#endif
};

/**
 * Lifetime policy for the singleton class that never destroys the singleton.
 */
template <class T>
struct lifetime_no_destroy
{
    /** Scheduler function */
    static void schedule (T*, destroy_func_t)
    {}

    /** Error handler funcion */
    static void on_dead_reference ()
    {
	throw singleton_error ();
    }
};

} /* namespace base */
} /* namespace psynth */

#include <psynth/base/singleton.tpp>

#endif /* PSYNTH_SINGLETON_H */
