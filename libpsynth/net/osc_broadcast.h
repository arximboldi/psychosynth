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

#ifndef PSYNTH_OSCBROADCAST_H
#define PSYNTH_OSCBROADCAST_H

#include <list>
#include <lo/lo.h>

#include <libpsynth/net/osc_misc.h>

namespace psynth
{

/*
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

class osc_broadcast
{
    std::list<lo_address> m_dest;
    lo_server m_sender;
    
public:
    osc_broadcast()
	: m_sender(NULL)
	{}
    
    ~osc_broadcast() {
	clear();
    }

    void set_sender(lo_server s) {
	m_sender = s;
    }
    
    void add_target (lo_address dest) {
	m_dest.push_back(dest);
    }
    
    void delete_target (lo_address dest);
    /*{
      m_dest.remove_if(lo_address_equals_func(dest));
    }*/

    void clear ();
    
    bool is_target (lo_address dest);
    
    void broadcast_message (const char* path, lo_message msg);

    void broadcast_message_from (const char* path, lo_message msg, lo_address from);
};

} /* namespace psynth */

#endif /* PSYNTH_OSCBROADCAST_H */
