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

#ifndef MAPITERATOR_H
#define MAPITERATOR_H

#include <map>

/**
 * @class MapIterator
 *
 * This class acts as interface to a map<a,b>::iterator. It just changes the
 * opearator* behaviour, which now returns a pointer to type Data
 */
template <typename Key, typename Data>
class MapIterator : public std::map<Key, Data>::iterator
{
public:
    MapIterator()
	{}
    
    MapIterator (const typename std::map<Key, Data>::iterator& i) :
	std::map<Key, Data>::iterator(i) {}

    MapIterator (const MapIterator<Key, Data>& i) :
	std::map<Key, Data>::iterator(static_cast<const typename std::map<Key, Data>::iterator>(i))
	{}

    Data& operator*() {
	return std::map<Key, Data>::iterator::operator*().second;
    }

    const Data& operator*() const {
	return std::map<Key, Data>::iterator::operator*().second;
    }
};

/**
 * @class MapIterator
 *
 * This class acts as interface to a map<a,b>::iterator. It just changes the
 * opearator* behaviour, which now returns a pointer to type Data
 */
template <typename Key, typename Data>
class MapConstIterator : public std::map<Key, Data>::const_iterator
{
public:
    MapConstIterator (const typename std::map<Key, Data>::const_iterator& i) :
	std::map<Key, Data>::const_iterator(i) {}

    MapConstIterator (const MapConstIterator<Key, Data>& i) :
	std::map<Key, Data>::const_iterator(static_cast<const typename std::map<Key, Data>::const_iterator>(i))
	{}

    const Data& operator*() {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }

    const Data& operator*() const {
	return std::map<Key, Data>::const_iterator::operator*().second;
    }
};


#endif /* MAPITERATOR_H */
