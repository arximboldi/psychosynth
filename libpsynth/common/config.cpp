/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#include "common/config.h"

using namespace std;

namespace psynth
{

void conf_subject::notify_conf_change (conf_node& source)
{
    for (list<conf_listener*>::iterator i = m_list.begin();
	 i != m_list.end();
	 ++i)
	(*i)->handle_conf_change (source);

    for (list<conf_event>::iterator i = m_change_del.begin();
	 i != m_change_del.end();
	 ++i)
	(*i) (source);    
}

void conf_subject::notify_conf_nudge (conf_node& source)
{
    for (list<conf_listener*>::iterator i = m_list.begin();
	 i != m_list.end();
	 ++i)
	(*i)->handle_conf_nudge (source);

    for (list<conf_event>::iterator i = m_nudge_del.begin ();
	 i != m_nudge_del.end ();
	 ++i)
	(*i) (source);
}

void conf_subject::notify_conf_new_child (conf_node& child)
{
    for (list<conf_listener*>::iterator i = m_list.begin();
	 i != m_list.end ();
	 ++i)
	(*i)->handle_conf_new_child (child);
}

void conf_subject::notify_conf_remove_child (conf_node& child)
{
    for (list<conf_listener*>::iterator i = m_list.begin();
	 i != m_list.end();
	 ++i)
	(*i)->handle_conf_remove_child (child);
}

} /* namespace psynth */
