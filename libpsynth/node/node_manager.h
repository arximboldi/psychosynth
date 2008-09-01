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

#ifndef PSYNTH_NODE_MANAGER_H
#define PSYNTH_NODE_MANAGER_H

#include <map>

#include <boost/thread/mutex.hpp>

#include <libpsynth/common/map_iterator.h>
#include <libpsynth/node/node_output.h>

namespace psynth
{

class node_manager
{
    std::map<int, node*> m_node_map;
    std::list<node_output*> m_outputs;
    std::list<node*> m_delete_list;
    
    boost::mutex m_update_lock;
    
public:
    typedef map_iterator<int, node*> iterator;
    typedef map_const_iterator<int, node*> const_iterator;

    node_manager ();
    ~node_manager ();
	
    bool attach_node (node* obj, int id);
    bool detach_node (int id);
    void detach_node (iterator it);
    bool delete_node (int id);
    void delete_node (iterator it);

    const_iterator begin () const {
	return m_node_map.begin();
    }
    
    iterator begin () {
	return m_node_map.begin();
    }

    const_iterator end () const {
	return m_node_map.end();
    }
    
    iterator end () {
	return m_node_map.end();
    }
    
    iterator find (int id) {
	return m_node_map.find(id);
    }

    const_iterator find (int id) const {
	return m_node_map.find(id);
    }

    void set_info (const audio_info& info);

    /**
     * Makes a full new update of the objects. This means that it first resets
     * the is-updated property of the objects and then calls update() on all
     * the attached node_outputs. The update is propagated via DFS. Some
     * objects may not be updated if not conected to a subgraph containing an
     * OutputObject.
     *
     * This function may be called by an OutputObject if a registered Output
     * system calls for new data and not enought data is availible in its
     * buffer.
     */
    void update ();
};

} /* namespace psynth */

#endif /* PSYNTH_NODE_MANAGER_H */
