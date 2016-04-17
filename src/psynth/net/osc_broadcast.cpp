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

#include <cstring>
#include <iostream>

#include "net/osc_broadcast.hpp"
#include "net/osc_misc.hpp"

using namespace std;

namespace psynth
{

void osc_broadcast::clear()
{
    for (list<lo_address>::iterator it = m_dest.begin();
	 it != m_dest.end(); ++it)
	lo_address_free(*it);

    m_dest.clear();
    m_sender = NULL;
}

void osc_broadcast::delete_target (lo_address add)
{
   for (list<lo_address>::iterator it = m_dest.begin();
	 it != m_dest.end();)
       if (lo_address_equals(*it, add)) {
	   lo_address_free(*it);
	   m_dest.erase(it++);
       }
       else
	   ++it;
}

bool osc_broadcast::is_target (lo_address adr)
{
    for (list<lo_address>::iterator it = m_dest.begin();
	 it != m_dest.end(); ++it)
	if (lo_address_equals(*it, adr))
	    return true;

    return false;
}

void osc_broadcast::broadcast_message (const char* path, lo_message msg)
{
    if (m_sender)
	for (list<lo_address>::iterator it = m_dest.begin();
	     it != m_dest.end(); ++it)
	    lo_send_message_from (*it, m_sender, path, msg);
    else
	for (list<lo_address>::iterator it = m_dest.begin();
	     it != m_dest.end(); ++it)
	    lo_send_message (*it, path, msg);
}

void osc_broadcast::broadcast_message_from (const char* path, lo_message msg, lo_address source)
{
    if (m_sender) {
	for (list<lo_address>::iterator it = m_dest.begin();
	     it != m_dest.end(); ++it)
	    if (!lo_address_equals(*it, source))
		lo_send_message_from(*it, m_sender, path, msg);
    } else {
	for (list<lo_address>::iterator it = m_dest.begin();
	     it != m_dest.end(); ++it)
	    if (!lo_address_equals(*it, source))
		lo_send_message(*it, path, msg);
    }
}

} /* namespace psynth */
