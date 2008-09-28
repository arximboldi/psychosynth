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

#include "node/node_manager.h"

using namespace std;

namespace psynth
{

node_manager::node_manager ()
    : m_node_map()
    , m_outputs()
{
}

node_manager::~node_manager ()
{
}

bool node_manager::add_node (mgr_ptr<node> obj, int id)
{
    if (!m_node_map.insert(make_pair (id, obj)).second)
	return false;

    obj->set_id (id);
    
    node_output* out = dynamic_cast<node_output*> (obj.get ());

    if (out) {
	if (!out->set_manager (this)) {
	    m_node_map.erase (id);
	    return false; /* Already attached to a node_manager! */
	}
	
	m_outputs.push_back (out);
    }

    if (obj.is_managed ())
	std::cout << "macagonla\n";
    
    return true;
}

void node_manager::do_delete_node (iterator it)
{
    //map<int, mgr_ptr<node> >::iterator& iter = it;
    
    node_output* out = dynamic_cast<node_output*> (it->get ());
    if (out != 0) {
	m_outputs.remove (out);
	out->set_manager (0);
    }

    (*it)->set_id (node::NULL_ID);
    m_node_map.erase (it);
}

bool node_manager::delete_node (int id)
{
    map<int, mgr_ptr<node> >::iterator iter;
    iter = m_node_map.find (id);
    
    if (iter == m_node_map.end())
	return false;

    delete_node (iter);

    return true;
}

void node_manager::delete_node (iterator it)
{
    node* obj = *it;
    
    obj->clear_connections ();
    if (!obj->has_connections ())
	do_delete_node (it);
    else
	m_delete_list.push_back (*it);
}

void node_manager::set_info (const audio_info& info)
{
    boost::mutex::scoped_lock lock (m_update_mutex);

    map<int, mgr_ptr<node> >::iterator map_iter;
    for (map_iter = m_node_map.begin();
	 map_iter != m_node_map.end();
	 ++map_iter)
    {
	map_iter->second->set_info (info);
    }
}

void node_manager::update()
{
    boost::mutex::scoped_lock lock (m_update_mutex);
    
    map<int, mgr_ptr<node> >::iterator map_iter;
    for (map_iter = m_node_map.begin();
	 map_iter != m_node_map.end();
	 ++map_iter)
    {
	(*map_iter).second->advance();
    }
	
    list<node_output*>::iterator out_iter;
    for (out_iter = m_outputs.begin();
	 out_iter != m_outputs.end();
	 ++out_iter)
    {
	(*out_iter)->update (0, -1, -1);
    }

    list<node*>::iterator del_iter;
    for (del_iter = m_delete_list.begin();
	 del_iter != m_delete_list.end();)
    {
	if (!(*del_iter)->has_connections ()) {
	    delete_node ((*del_iter)->get_id ());
	    m_delete_list.erase (del_iter++);
	} else
	    ++del_iter;
    }
}

} /* namespace psynth */
