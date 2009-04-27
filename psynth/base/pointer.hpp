/**
 *  Time-stamp:  <2009-04-27 16:40:13 raskolnikov>
 *
 *  @file        pointer.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  Pointer manipulation helper functions.
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_POINTER_HPP
#define PSYNTH_POINTER_HPP

#include <memory>
#include <algorithm>

namespace psynth
{

/**
 * Functor for the delete operator, usefull to clear pointer containers with
 * for_each.
 */
template <class PtrT>
class deleter
{
public:
    void operator () (PtrT ptr) {
	delete ptr;
    }
};

template <class T>
class null_ptr
{
public:
    null_ptr ()
	: m_ptr (0)
    {}

    null_ptr (null_ptr& p)
	: m_ptr (p.m_ptr)
    {}
    
    template <class Y>
    null_ptr (null_ptr<Y>& p)
	: m_ptr (p.m_ptr)
    {}
    
    null_ptr (T* ptr)
	: m_ptr (ptr)
    {}
    
    template <class Y>
    null_ptr (Y* ptr)
	: m_ptr (ptr)
    {}

    T* operator-> () const
    {
	return m_ptr;
    }
    
    T& operator* () const
    {
	return *m_ptr;
    }

    operator T* () const
    {
	return m_ptr;
    }

    operator bool () const
    {
	return m_ptr != 0;
    }

private:
    T* m_ptr;
};

template <class T>
class mgr_ptr;

template <class T>
class mgr_auto_ptr;

template <class T>
class manage_me_ptr
{
    template <class Y> friend class mgr_ptr;
    template <class Y> friend class mgr_auto_ptr;
    
    T* m_ptr;
public:

    manage_me_ptr (T* p)
	: m_ptr (p)
    {}
};

template <class T>
class mgr_ptr
{
    T* m_ptr;
    bool m_managed;
    
public:
    ~mgr_ptr () { }
    
    mgr_ptr ()
	: m_ptr (0)
	, m_managed (false) {}

    mgr_ptr (const mgr_ptr& p)
	: m_ptr (p.m_ptr)
	, m_managed (p.m_managed)
    {}
    
    template <class Y>
    mgr_ptr (const mgr_ptr<Y>& p)
	: m_ptr (p.m_ptr)
	, m_managed (p.m_managed)
    {}
    
    mgr_ptr (T* ptr)
	: m_ptr (ptr)
	, m_managed (false)
    {}
    
    template <class Y>
    mgr_ptr (Y* ptr)
	: m_ptr (ptr)
	, m_managed (false)
    {}
    
    template <class Y>
    mgr_ptr (const manage_me_ptr<Y>& p)
	: m_ptr (p.m_ptr)
	, m_managed (true)
    {}

    mgr_ptr& operator= (const mgr_ptr& p)
    {
	m_ptr = p.m_ptr;
	m_managed = p.m_managed;
	return *this;
    }
    
    template <class Y>
    mgr_ptr& operator= (const mgr_ptr<Y>& p)
    {
	m_ptr = p.m_ptr;
	m_managed = p.m_managed;
	return *this;
    }

    mgr_ptr& operator= (T* p)
    {
	m_ptr = p;
	m_managed = false;
	return *this;
    }
    
    template <class Y>
    mgr_ptr& operator= (Y* p)
    {
	m_ptr = p;
	m_managed = false;
	return *this;
    }

    template <class Y>
    mgr_ptr& operator= (const manage_me_ptr<Y>& p)
    {
	m_ptr = p.m_ptr;
	m_managed = true;
	return *this;
    }

    T* operator-> () const
    {
	return m_ptr;
    }
    
    T& operator* () const
    {
	return *m_ptr;
    }

    operator T* () const
    {
	return m_ptr;
    }

    operator bool () const
    {
	return m_ptr != 0;
    }
    
    T* get () const
    {
	return m_ptr;
    }

    bool is_managed () const
    {
	return m_managed;
    }
    
    void manage ()
    {
	m_managed = true;
    }
    
    void unmanage ()
    {
	m_managed = false;
    }
    
    void clean ()
    {
	if (m_managed) {
	    delete m_ptr;
	    m_ptr = 0;
	}
    }
};

template <class T>
class mgr_auto_ptr
{
    T* m_ptr;
    bool m_managed;
    
public:
    ~mgr_auto_ptr ()
    {
	clean ();
    }
    
    mgr_auto_ptr ()
	: m_ptr (0)
	, m_managed (false)
    {}

    mgr_auto_ptr (mgr_auto_ptr& p)
	: m_ptr (p.m_ptr)
	, m_managed (p.m_managed)
    {
	p.unmanage ();
    }
    
    template <class Y>
    mgr_auto_ptr (mgr_auto_ptr<Y>& p)
	: m_ptr (p.m_ptr)
	, m_managed (p.m_managed)
    {
	p.unmanage ();
    }
    
    mgr_auto_ptr (T* ptr)
	: m_ptr (ptr)
	, m_managed (false)
    {}
    
    template <class Y>
    mgr_auto_ptr (Y* ptr)
	: m_ptr (ptr)
	, m_managed (false)
    {}
    
    template <class Y>
    mgr_auto_ptr (const manage_me_ptr<Y>& p)
	: m_ptr (p.m_ptr)
	, m_managed (true)
    {}

    template <class Y>
    mgr_auto_ptr (std::auto_ptr<Y>& p)
	: m_ptr (p.release ())
	, m_managed (true)
    {}
    
    mgr_auto_ptr& operator= (mgr_auto_ptr& p)
    {
	if (&p != this) {
	    clean ();
	    m_ptr = p.m_ptr;
	    m_managed = p.m_managed;
	    p.unmanage ();
	}
	
	return *this;
    }
    
    template <class Y>
    mgr_auto_ptr& operator= (mgr_auto_ptr<Y>& p)
    {
	if (&p != this) {
	    clean ();
	    m_ptr = p.m_ptr;
	    m_managed = p.m_managed;
	    p.unmanage ();
	}
	return *this;
    }
    
    mgr_auto_ptr& operator= (T* p)
    {
	if (p != m_ptr) {
	    clean ();
	    m_ptr = p;
	    m_managed = false;
	}
	
	return *this;
    }
    
    template <class Y>
    mgr_auto_ptr& operator= (Y* p)
    {
	clean ();
	m_ptr = p;
	m_managed = false;
	return *this;
    }

    template <class Y>
    mgr_auto_ptr& operator= (const manage_me_ptr<Y>& p)
    {
	clean ();
	m_ptr = p.m_ptr;
	m_managed = true;
	return *this;
    }

    template <class Y>
    mgr_auto_ptr& operator= (std::auto_ptr<Y>& p)
    {
	clean ();
	m_ptr = p.release ();
	m_managed = true;
	return *this;
    }
    
    T* operator-> () const
    {
	return m_ptr;
    }
    
    T& operator* () const
    {
	return *m_ptr;
    }

    operator T* () const
    {
	return m_ptr;
    }

    operator bool () const
    {
	return m_ptr != 0;
    }
    
    T* get () const
    {
	return m_ptr;
    }

    bool is_managed () const
    {
	return m_managed;
    }
    
    void manage ()
    {
	m_managed = true;
    }
    
    void unmanage ()
    {
	m_managed = false;
    }
    
    void clean ()
    {
	if (m_managed) {
	    delete m_ptr;
	    m_ptr = 0;
	}
    }
};

template <class T>
manage_me_ptr<T> manage (T* ptr)
{
    return manage_me_ptr<T> (ptr);
}

template <class T>
class mgr_container : public T
{
public:
    ~mgr_container ()
    {
	std::for_each (T::begin (),
		       T::end (),
		       std::mem_fun (&T::value_type::clean ()));
    }
};

template <class T>
class mgr_assoc : public T
{
public:
    ~mgr_assoc ()
    {
	for (typename T::iterator it = T::begin ();
	     it != T::end ();
	     ++it)
	    it->second.clean ();
    }
};

} /* namespace psynth*/

#endif /* PSYNTH_POINTER_HPP */
