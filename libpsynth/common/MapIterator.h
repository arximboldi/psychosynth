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
class MapIterator : public std::map<Key, Data>::iterator
{
public:
    /** Constructor. */
    MapIterator()
	{}

    /** Constructor from a map::iterator. */
    MapIterator (const typename std::map<Key, Data>::iterator& i) :
	std::map<Key, Data>::iterator(i) {}

    /** Copy constructor. */
    MapIterator (const MapIterator<Key, Data>& i) :
	std::map<Key, Data>::iterator(static_cast<const typename std::map<Key, Data>::iterator>(i))
	{}

    /**
     * Indirection operator, returns a reference to the referred value.
     */
    Data& operator*() {
	return std::map<Key, Data>::iterator::operator*().second;
    }

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Data& operator*() const {
	return std::map<Key, Data>::iterator::operator*().second;
    }
};

/**
 * This class acts as wrapper to a @c map::const_iterator. It just changes the
 * opearator* behaviour, which now returns a pointer to the contained value
 * hidding the user the value key. Just a @c const version of @c MapIterator
 * to hold constant data.
 */
template <typename Key, typename Data>
class MapConstIterator : public std::map<Key, Data>::const_iterator
{
public:
    /** Constructor from a @c map::const_iterator */
    MapConstIterator (const typename std::map<Key, Data>::const_iterator& i) :
	std::map<Key, Data>::const_iterator(i) {}

    /** Copy constructor. */
    MapConstIterator (const MapConstIterator<Key, Data>& i) :
	std::map<Key, Data>::const_iterator(static_cast<const typename std::map<Key, Data>::const_iterator>(i))
	{}

    /**
     * Indirection operator, returns a reference to the referred value.
     */
    const Data& operator*() {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }

    /**
     * Const indirection operator, returns a constant reference to the
     * referred value.
     */
    const Data& operator*() const {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_MAPITERATOR_H */
