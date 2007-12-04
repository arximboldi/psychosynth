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

#ifndef OSCBROADCAST_H
#define OSCBROADCAST_H

#include <list>
#include <lo/lo.h>

#include "net/OSCMisc.h"

/*
template<class T>
class first_equals
{
    T m_ref;
public:
    first_equals(const T& r) : m_ref(r) {};

    template<class U>
    bool operator() (const std::pair<T,U>& p) {
	return p.first == m_ref;
    }
};
*/

class lo_address_equals_func
{
    lo_address m_ref;
    
public:
    lo_address_equals_func(lo_address ref) : m_ref(ref) {}

    bool operator() (lo_address param) {
	return lo_address_equals(m_ref, param);
    }
};

class OSCBroadcast
{
    std::list<lo_address> m_dest;
    
public:
    void addDestiny(lo_address dest) {
	m_dest.push_back(dest);
    }
    
    void deleteDestiny(lo_address dest) {
	m_dest.remove_if(lo_address_equals_func(dest));
    }

    void clear() {
	m_dest.clear();
    }
    
    void broadcastMessage(const char* path, lo_message msg);

    void broadcastMessageFrom(const char* path, lo_message msg, lo_address from);
};

#endif /* OSCBROADCAST_H */
