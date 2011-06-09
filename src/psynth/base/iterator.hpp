/**
 *  Time-stamp:  <2011-06-09 20:12:38 raskolnikov>
 *
 *  @file        iterator.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Iterator wrapper tools.
 *
 *  @todo Fix many implementations by using boost::iterator_adaptor
 *  and boost::iterator_facade.
 */

/*
 *  Copyright (C) 2007 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_BASE_ITERATOR_HPP_
#define PSYNTH_BASE_ITERATOR_HPP_

#include <map>
#include <iterator>

#include <psynth/base/type_traits.hpp>
#include <psynth/base/pointer.hpp>

namespace psynth
{
namespace base
{

template <typename Iterator>
class ptr_iterator : public Iterator
{
public:
    typedef std::iterator_traits<Iterator> base_traits;
    
    typedef typename std::iterator_traits<Iterator>::value_type ptr_value_type;
    typedef typename pointee<ptr_value_type>::type value_type;
    
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    ptr_iterator () = default;
    ptr_iterator (const ptr_iterator& it) = default;

    ptr_iterator (ptr_iterator&& it)
        : Iterator (std::move (it))
    {}

    ptr_iterator (const Iterator& it)
 	: Iterator (it)
    {}

    ptr_iterator (Iterator&& it)
 	: Iterator (std::move (it))
    {}
    
    template<class OtherIterator>
    ptr_iterator (const OtherIterator& it)
	: Iterator (it)
    {}

    template<class OtherIterator>
    ptr_iterator (OtherIterator&& it)
	: Iterator (std::move (it))
    {}
    
    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    value_type& operator*() const
    {
	return *Iterator::operator* ();
    }

    /**
     * Indirection operator, returns a pointer to the referred value const value.
     */
    value_type* operator->() const
    {
	return &*Iterator::operator* ();
    }
};

template <typename Iterator>
class ptr_const_iterator : public Iterator
{
public:
    typedef typename std::iterator_traits<Iterator>::value_type ptr_value_type;
    typedef typename pointee<ptr_value_type>::type value_type;
    
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    
    ptr_const_iterator () {}

    ptr_const_iterator (const Iterator& it)
	: Iterator (it)
    {}

    template<class OtherIterator>
    ptr_const_iterator (const OtherIterator& it)
	: Iterator (it)
    {}
    
    ptr_const_iterator (const ptr_const_iterator& it)
	: Iterator (it)
    {}

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const value_type& operator*() const
    {
	return *Iterator::operator* ();
    }

    /**
     * Indirection operator, returns a pointer to the referred value const value.
     */
    const value_type* operator->() const
    {
	return &*Iterator::operator* ();
    }
};
  
/**
 *  This class acts as wrapper to a map::iterator. It just changes the
 *  opearator* behaviour, which now returns a pointer to the contained
 *  value hidding the user the value key.
 */
template <typename Key, typename Data>
class map_iterator : public std::map<Key, Data>::iterator
{
public:
    typedef typename std::map<Key, Data>::iterator base_type;
    
    typedef typename base_type::iterator_category iterator_category;
    typedef Data value_type;
    typedef typename base_type::difference_type difference_type;
    typedef Data* pointer;
    typedef Data& reference;
    
    /** Constructor. */
    map_iterator ()
    {}

    /** Constructor from a map::iterator. */
    map_iterator (const base_type& i)
	: base_type (i) {}

    /** Copy constructor. */
    map_iterator (const map_iterator& i)
	: base_type (
	    static_cast<const base_type&>(i)
	    )
    {}

    /** Constructor from a map::iterator. */
    map_iterator (base_type&& i)
	: base_type (std::move (i)) {}

    /** Copy constructor. */
    map_iterator (map_iterator&& i)
	: base_type (
            std::move (
                static_cast<const base_type&>(i)
            ))
    {}

    /**
     * Indirection operator, returns a reference to the referred value.
     */
    Data& operator*()
    {
	return std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Indirection operator, returns a pointer to the referred value.
     */
    Data* operator->()
    {
	return &std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Data& operator*() const
    {
	return std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Indirection operator, returns a pointer to the referred value const value.
     */
    const Data* operator->() const
    {
	return &std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Preincrement operator.
     */
    map_iterator<Key, Data> operator++ ()
    {
	return std::map<Key, Data>::iterator::operator++();
    }

    /**
     * Postincrement operator.
     */
    map_iterator<Key, Data> operator++ (int)
    {
	return std::map<Key, Data>::iterator::operator++(0);
    }
};

/**
 *  This class acts as wrapper to a @c map::const_iterator. It just
 *  changes the opearator* behaviour, which now returns a pointer to
 *  the contained value hidding the user the value key. Just a @c
 *  const version of @c map_iterator to hold constant data.
 *
 *  @todo Implement -> operator.
 */
template <typename Key, typename Data>
class map_const_iterator : public std::map<Key, Data>::const_iterator
{
public:
    typedef typename std::map<Key, Data>::const_iterator::iterator_category iterator_category;
    typedef Data value_type;
    typedef typename std::map<Key, Data>::const_iterator::difference_type difference_type;
    typedef Data* pointer;
    typedef Data& reference;

    /** Constructor from a @c map::const_iterator */
    map_const_iterator (const typename std::map<Key, Data>::const_iterator& i)
	: std::map<Key, Data>::const_iterator(i) {}

    /** Copy constructor. */
    map_const_iterator (const map_const_iterator<Key, Data>& i)
	: std::map<Key, Data>::const_iterator (
	    static_cast<const typename std::map<Key, Data>::const_iterator>(i))
    {}

    /**
     *  Indirection operator, returns a reference to the referred
     *  value.
     */
    const Data& operator* ()
    {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }

    /**
     *  Const indirection operator, returns a constant reference to
     *  the referred value.
     */
    const Data& operator* () const
    {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }
};

/**
 *  @todo This class was copied from the "Overdose" project:
 *
 *     http://sinusoid.es/raskolnikov/overdose-0.1.2.tar.gz
 *
 *  A whole refactor of this module would provide more compact clearer
 *  code, using boost::adaptor, because this is a bit kind of a mess.
 */
template <typename Key, typename Data, class Iter>
class map_iterator_base : public std::map<Key, Data>::iterator
{
public:
    typedef typename std::map<Key, Data>::iterator::iterator_category
    iterator_category;
    typedef Data value_type;
    typedef typename std::map<Key, Data>::iterator::difference_type
    difference_type;
    typedef Data* pointer;
    typedef Data& reference;

    /** Constructor. */
    map_iterator_base ()
    {}

    /** Constructor from a map::iterator. */
    map_iterator_base (const typename std::map<Key, Data>::iterator& i)
	: std::map<Key, Data>::iterator (i)
    {}

    /** Copy constructor. */
    map_iterator_base (const map_iterator_base<Key, Data, Iter>& i)
	: std::map<Key, Data>::iterator (
	    static_cast<const typename std::map<Key, Data>::iterator> (i))
    {}
    
    /**
     * Preincrement operator.
     */
    Iter operator++ ()
    {
	return std::map<Key, Data>::iterator::operator++();
    }

    /**
     * Postincrement operator.
     */
    Iter operator++ (int)
    {
	return std::map<Key, Data>::iterator::operator++(0);
    }
};

template <typename Key, typename Data, class Iter>
class map_const_iterator_base : public std::map<Key, Data>::const_iterator
{
public:
    typedef typename std::map<Key, Data>::const_iterator::iterator_category
    iterator_category;
    typedef Data value_type;
    typedef typename std::map<Key, Data>::const_iterator::difference_type
    difference_type;
    typedef Data* pointer;
    typedef Data& reference;

    /** Constructor. */
    map_const_iterator_base ()
    {}

    /** Constructor from a map::iterator. */
    map_const_iterator_base (const typename std::map<Key, Data>::const_iterator& i)
	: std::map<Key, Data>::const_iterator (i)
    {}

    /** Copy constructor. */
    map_const_iterator_base (const map_const_iterator_base<Key, Data, Iter>& i)
	: std::map<Key, Data>::const_iterator (
	    static_cast<const typename std::map<Key, Data>::const_iterator> (i))
    {}
    
    /**
     * Preincrement operator.
     */
    Iter operator++ ()
    {
	return std::map<Key, Data>::const_iterator::operator++ ();
    }

    /**
     * Postincrement operator.
     */
    Iter operator++ (int)
    {
	return std::map<Key, Data>::const_iterator::operator++ (0);
    }
};

/**
 *  This class acts as wrapper to a map::iterator. It just changes the
 *  opearator* behaviour, allowing dereferencing only the key value.
 */
template <typename Key, typename Data>
class map_key_iterator :
    public map_iterator_base<Key, Data, map_key_iterator<Key, Data> >
{
public:
    map_key_iterator () {}

    template <class T>
    map_key_iterator (const T& val)
	: map_iterator_base<Key, Data, map_key_iterator> (val)
    {}
    
    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Key& operator*() const
    {
	return std::map<Key, Data>::iterator::operator*().first;
    }

    /**
     * Indirection operator, returns a pointer to the referred value const value.
     */
    const Key* operator->() const
    {
	return &std::map<Key, Data>::iterator::operator*().first;
    }
};


} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_ITERATOR_HPP_ */
