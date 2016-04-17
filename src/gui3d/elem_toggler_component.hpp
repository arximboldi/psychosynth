/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef ELEMTOGGLERCOMPONENT_H
#define ELEMTOGGLERCOMPONENT_H

#include <vector>
#include "gui3d/element.hpp"
#include "gui3d/task_manager.hpp"

class elem_toggler_component : public elem_component,
			       public task
{
    typedef std::pair<flat_ring*, int> Toggle;
    std::vector<Toggle> m_toggles;

    int m_param_num;
    int m_param_first;
    int m_param_step;

    int m_step;

    void create_toggles ();
    void destroy_toggles();
    void update_current_step_colour (int step);
    void update_step_colour (int step);

public:
    elem_toggler_component (int num_param,
			    int first_param,
			    int step_param);
    ~elem_toggler_component ();

    void init ();
    void update (int ms);
    bool handle_pointer_move (Ogre::Vector2 pos);
    bool handle_pointer_click (Ogre::Vector2 pos, OIS::MouseButtonID id);
    bool handle_pointer_release (Ogre::Vector2 pos, OIS::MouseButtonID id);
    void handle_param_change (psynth::world_node& obj,
			   int param_id);
};

#endif /* ELEMTOGGLERCOMPONENT_H */
