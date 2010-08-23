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

#ifndef PSYNTH_OSCCONTROLLER_H
#define PSYNTH_OSCCONTROLLER_H

#include <map>

#include <psynth/world/world.hpp>
#include <psynth/net/osc_misc.hpp>
#include <psynth/net/osc_broadcast.hpp>

namespace psynth
{

class osc_controller : public world_listener,
		       public world_node_listener,
		       public osc_broadcast
{
    std::map<std::pair<int,int>, int> m_local_id;
    std::map<int, std::pair<int,int> > m_net_id;
    world* m_world;
    int m_skip;
    int m_id;
    bool m_activated;
    bool m_broadcast;
    
    LO_HANDLER (osc_controller, add);
    LO_HANDLER (osc_controller, delete);
    LO_HANDLER (osc_controller, param);
    LO_HANDLER (osc_controller, activate);
    LO_HANDLER (osc_controller, deactivate);

    void add_to_world (world* world) {
	world->add_world_listener (this);
	world->add_world_node_listener (this);
    }

    void delete_from_world (world* world) {
	world->delete_world_listener (this);
	world->delete_world_node_listener (this);
    }
    
public:
    osc_controller (bool broadcast = false);
    ~osc_controller ();

    void set_id (int id) {
	m_id = id;
    }

    void set_world (world* world) {
	deactivate ();
	m_world = world;
    }

    world* get_world () {
	return m_world;
    }

    void activate () {
	if (!m_activated && m_world) {
	    m_activated = true;
	    add_to_world (m_world);
	}
    }

    void deactivate () {
	if (m_activated && m_world) {
	    m_activated = false;
	    delete_from_world (m_world);
	}
    }

    void clear () {
	deactivate();
	m_local_id.clear();
	m_net_id.clear();
	m_skip = 0;
	osc_broadcast::clear();
    }

    void add_methods (lo_server s);
    
    void handle_add_node (world_node& obj);
    void handle_delete_node (world_node& obj);
    void handle_activate_node (world_node& obj);
    void handle_deactivate_node (world_node& obj);
    void handle_set_param_node (world_node& ob, int param_id);
};

} /* namespace psynth */

#endif /* PSYNTH_OSCCONTROLLER_H */
