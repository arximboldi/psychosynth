
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

#ifndef ELEMMAINCOMPONENT_H
#define ELEMMAINCOMPONENT_H

#include "gui3d/element.hpp"

class elem_main_component : public elem_component
{
    std::string m_mesh;
    Ogre::SceneNode* m_ent_node;
    Ogre::Entity*   m_mesh_ent;

    flat_ring* m_indicator;
    flat_ring* m_indicator_fill;

    Ogre::Vector2 m_last_mouse_pos;

    int m_param;

    float m_min_val;
    float m_max_val;
    float m_old_value;
    bool m_rotating;

public:
    elem_main_component (const std::string& mesh,
			 int param,
			 float min_val, float max_val);
    ~elem_main_component ();

    void set_mesh (const std::string& mesh);

    void init ();
    bool handle_pointer_move (Ogre::Vector2 pos);
    bool handle_pointer_click(Ogre::Vector2 pos, OIS::MouseButtonID id);
    bool handle_pointer_release (Ogre::Vector2 pos, OIS::MouseButtonID id);
    virtual void handle_param_change (psynth::world_node& obj,
				      int id);
};

class elem_multi_main_component : public elem_main_component
{
    int m_param;
    const char** m_names;

public:
    elem_multi_main_component (int param_1,
			       float min_val, float max_val,
			       int param_2,
			       const char** names);

    void handle_param_change (psynth::world_node& obj,
			      int id);
};

#endif /* ELEMMAIMCOMPONENT */
