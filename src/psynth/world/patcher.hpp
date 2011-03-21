/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef PSYNTH_PATCHER_H
#define PSYNTH_PATCHER_H

#include <map>
#include <set>

#include <psynth/graph/node.hpp>

namespace psynth
{

struct patcher_event {
    graph::node* src;
    graph::node* dest;
    int src_socket;
    int dest_socket;
    int socket_type;

    patcher_event (graph::node* s, graph::node* d, int ss, int ds, int st):
	src(s), dest(d), src_socket(ss), dest_socket(ds), socket_type(st) {};
};

class patcher_listener {
public:
    virtual ~patcher_listener () {};
    virtual void handle_link_added (const patcher_event& ev) = 0;
    virtual void handle_link_deleted (const patcher_event& ev) = 0;
};

class patcher_subject {
    std::list<patcher_listener*> m_list;

protected:
    void notify_link_added (const patcher_event& ev) {
	for (std::list<patcher_listener*>::iterator it = m_list.begin ();
	     it != m_list.end(); )
	    (*it++)->handle_link_added (ev);
    };
    
    void notify_link_deleted (const patcher_event& ev) {
	for (std::list<patcher_listener*>::iterator it = m_list.begin();
	     it != m_list.end(); )
	    (*it++)->handle_link_deleted (ev);
    };
    
public:
    void add_listener (patcher_listener* l) {
	m_list.push_back (l);
    };
    
    void delete_listener (patcher_listener* l) {
	m_list.remove (l);
    };
};

class patcher : public patcher_subject
{
public:
    virtual ~patcher () {};
    
    virtual bool add_node (graph::node* obj) = 0;
    virtual bool delete_node (graph::node* obj) = 0;
    virtual void set_param_node (graph::node* obj, int param) = 0;
    virtual void update () = 0;
    virtual void clear () = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_PATCHER_H */
