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

#include "object/node_manager.h"

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
    map<int, node*>::iterator map_iter;
    for (map_iter = m_node_map.begin(); map_iter != m_node_map.end(); ++map_iter)
	delete (*map_iter).second;
}

bool node_manager::attach_node (node* obj, int id)
{
    if (!m_node_map.insert(pair<int, node*>(id, obj)).second)
	return false;

    obj->set_id (id);
    
    ObjectOutput* out = dynamic_cast<ObjectOutput*>(obj);

    if (out != NULL) {
	if (!out->setManager (this)) {
	    m_node_map.erase(id);
	    return false; /* Already attached to an node_manager!! */
	}
		
	m_outputs.push_back(out);
    }
	
    return true;
}

bool node_manager::detach_node(int id)
{
    map<int, node*>::iterator iter;
    iter = m_node_map.find(id);
	
    if (iter == m_node_map.end())
	return false;

    detach_node(iter);
    
    return true;
}

void node_manager::detach_node (iterator it)
{
    map<int, node*>::iterator& iter = it;
    
    ObjectOutput* out = dynamic_cast<ObjectOutput*> (iter->second);
    if (out != 0) {
	m_outputs.remove(out);
	out->setManager (0);
    }

    iter->second->set_id (node::NULL_ID);
    m_node_map.erase(iter);
}

bool node_manager::delete_node (int id)
{
    map<int, node*>::iterator iter;
    iter = m_node_map.find(id);
    
    if (iter == m_node_map.end())
	return false;

    delete_node(iter);

    return true;
}

void node_manager::delete_node(iterator it)
{
    node* obj = *it;
/*    
    detach_node(it); 
*/
    obj->clear_connections ();

    if (!obj->has_connections ()) {
	detach_node (it);
	delete obj;
    }
    else
	m_delete_list.push_back(obj);
}

void node_manager::set_info (const audio_info& info)
{
    m_update_lock.lock();

    map<int, node*>::iterator map_iter;
    for (map_iter = m_node_map.begin(); map_iter != m_node_map.end(); ++map_iter) {
	map_iter->second->set_info (info);
    }
    
    m_update_lock.unlock ();
}

void node_manager::update()
{
    m_update_lock.lock();
    
    map<int, node*>::iterator map_iter;
    for (map_iter = m_node_map.begin(); map_iter != m_node_map.end(); ++map_iter) {
	(*map_iter).second->advance();
    }
	
    list<ObjectOutput*>::iterator out_iter;
    for (out_iter = m_outputs.begin(); out_iter != m_outputs.end(); ++out_iter) {
	(*out_iter)->update(NULL, -1, -1);
    }

    list<node*>::iterator del_iter;
    for (del_iter = m_delete_list.begin(); del_iter != m_delete_list.end();) {
	if (!(*del_iter)->has_connections ()) {
	    detach_node((*del_iter)->get_id ());
	    delete *del_iter;
	    m_delete_list.erase(del_iter++);
	} else
	    ++del_iter;
    }
    
    m_update_lock.unlock();
}

} /* namespace psynth */
