/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef PSYNTH_MAPITERATOR_H
#define PSYNTH_MAPITERATOR_H

#include <map>

namespace psynth
{

/**
 * This class acts as wrapper to a map::iterator. It just changes the
 * opearator* behaviour, which now returns a pointer to the contained value
 * hidding the user the value key.
 */
template <typename Key, typename Data>
class map_iterator : public std::map<Key, Data>::iterator
{
public:
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
    Data& operator*() {
	return std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Indirection operator, returns a pointer to the referred value.
     */
    Data* operator->() {
	return &std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Data& operator*() const {
	return std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Indirection operator, returns a pointer to the referred value const value.
     */
    const Data* operator->() const {
	return &std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Preincrement operator.
     */
    map_iterator<Key, Data> operator++() {
	return std::map<Key, Data>::iterator::operator++();
    }

    /**
     * Postincrement operator.
     */
    map_iterator<Key, Data> operator++(int) {
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
    /** Constructor from a @c map::const_iterator */
    map_const_iterator (const typename std::map<Key, Data>::const_iterator& i)
	: std::map<Key, Data>::const_iterator(i) {}

    /** Copy constructor. */
    map_const_iterator (const map_const_iterator<Key, Data>& i)
	: std::map<Key, Data>::const_iterator (
	    static_cast<const typename std::map<Key, Data>::const_iterator>(i)
	    )
	{}

    /**
     * Indirection operator, returns a reference to the referred value.
     */
    const Data& operator* () {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Data& operator* () const {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_MAPITERATOR_H */
