/**
 *  Time-stamp:  <2010-10-19 18:16:46 raskolnikov>
 *
 *  @file        iterator.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Iterator wrapper tools.
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

#ifndef PSYNTH_ITERATOR_H
#define PSYNTH_ITERATOR_H

#include <map>
#include <iterator>
#include <type_traits>

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
    typedef typename std::remove_pointer<ptr_value_type>::type value_type;

    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::difference_type difference_type;
    typedef value_type* pointer;
    typedef value_type& reference;

    ptr_iterator () {}

    ptr_iterator (const Iterator& it)
	: Iterator (it)
    {}

    template<class OtherIterator>
    ptr_iterator (const OtherIterator& it)
	: Iterator (it)
    {}
    
    ptr_iterator (const ptr_iterator& it)
	: Iterator (it)
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
    typename Iterator::value_type operator->() const
    {
	return Iterator::operator* ();
    }
};

template <typename Iterator>
class ptr_const_iterator : public Iterator
{
public:
    typedef typename std::iterator_traits<Iterator>::value_type ptr_value_type;
    typedef typename std::remove_pointer<ptr_value_type>::type value_type;
    
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
	return Iterator::operator* ();
    }
};
  
/**
 * This class acts as wrapper to a map::iterator. It just changes the
 * opearator* behaviour, which now returns a pointer to the contained value
 * hidding the user the value key.
 */
template <typename Key, typename Data>
class map_iterator : public std::map<Key, Data>::iterator
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
    map_iterator()
    {}

    /** Constructor from a map::iterator. */
    map_iterator (const typename std::map<Key, Data>::iterator& i)
	: std::map<Key, Data>::iterator(i) {}

    /** Copy constructor. */
    map_iterator (const map_iterator<Key, Data>& i)
	: std::map<Key, Data>::iterator (
	    static_cast<const typename std::map<Key, Data>::iterator>(i)
	    )
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
 * This class acts as wrapper to a @c map::const_iterator. It just changes the
 * opearator* behaviour, which now returns a pointer to the contained value
 * hidding the user the value key. Just a @c const version of @c map_iterator
 * to hold constant data.
 *
 * @todo Implement -> operator.
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
     * Indirection operator, returns a reference to the referred value.
     */
    const Data& operator* ()
    {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Data& operator* () const
    {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_ITERATOR_H */
