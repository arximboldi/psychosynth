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

#include <iostream>

#include "gui3d/element.hpp"
#include "gui3d/query_flags.hpp"

#include <psynth/common/misc.hpp>

#define ROTATION_FACTOR     100.0f

#define ELEMENT_GHOST_COLOUR    ColourValue(0.8, 0.8, 0.1, 0.4)
#define ELEMENT_NORMAL_COLOUR   ColourValue(1, 1, 1, 0.5)
#define ELEMENT_SELECTED_COLOUR ColourValue(1, 0, 0, 0.7)

using namespace std;
using namespace Ogre;
using namespace psynth;

element::element (world_node& obj, Ogre::SceneManager* scene) :
    m_obj(obj),
    m_col_ghost(ELEMENT_GHOST_COLOUR),
    m_col_selected(ELEMENT_SELECTED_COLOUR),
    m_col_normal(ELEMENT_NORMAL_COLOUR),
    m_scene(scene),
    m_aimpoint(0, Z_POS, 0),
    m_click_diff(0,0),
    m_pos(0,0),
    m_owned(false),
    m_ghost(true),
    m_selected(false),
    m_moving(false),
    m_modifier_1(0),
    m_modifier_2(0),
    m_gui_prop(obj)
{
    m_base = new flat_ring (string("EB") + itoa(m_obj.get_id (),10),
			    Degree(0), Degree(360),
			    0, 1,
			    m_col_ghost);

    m_gui_prop.set_active (false);
    
    m_node = scene->getRootSceneNode()->createChildSceneNode();
    
    m_node->attachObject(m_base);

    vector_2f v;
    obj.get_param (psynth::node::PARAM_POSITION, v);
    m_pos.x = v.x;
    m_pos.y = v.y;
    m_node->setPosition(Vector3(m_pos.x, Z_POS, m_pos.y));

    m_selected = false;
    set_ghost (true);
}

element::~element()
{
    for (elem_component_iter it = m_comp.begin(); it != m_comp.end(); ++it)
	delete *it;
    
    m_scene->destroySceneNode(m_node->getName());
    delete m_base;
}

void element::add_component (elem_component* comp)
{
    SceneNode* node;
    m_comp.push_back (comp);
    node = m_node->createChildSceneNode();
    node->yaw (Radian(Math::PI / 2), Node::TS_LOCAL);
    comp->set_scene_node (node);
    comp->set_parent (this);
    comp->init ();
    /* TODO */
    node->setVisible (!m_ghost);
}

void element::set_target (const world_node& obj)
{
    if (!m_target.is_null()) {
	m_target.delete_listener (this);
	m_aimpoint = Vector3(0, Z_POS, 0);
    }
    
    m_target = obj;

    if (!m_target.is_null()) {
	m_target.add_listener (this);

	vector_2f v;
	m_target.get_param (psynth::node::PARAM_POSITION, v);
	m_aimpoint.x = v.x;
	m_aimpoint.y = v.y;
	
	m_aimpoint = Vector3(v.x, Z_POS, v.y);
    }

    m_node->lookAt(m_aimpoint, Node::TS_PARENT);
}

void element::clear_target (const world_node& obj)
{
    if (m_target == obj) {
	m_target.delete_listener (this);
	m_target = world_node ();
	m_aimpoint = Vector3(0, Z_POS, 0);
    }
}

void element::set_position (const Ogre::Vector2& pos)
{
    vector_2f dest;
    dest.x = pos.x;
    dest.y = pos.y;
    m_obj.set_param (psynth::node::PARAM_POSITION, dest);
}

void element::set_ghost (bool ghost)
{
    if (m_ghost != ghost) {
	m_ghost = ghost;
	m_base->set_colour (m_ghost ? m_col_ghost : m_col_normal);
	
	for (elem_component_iter it = m_comp.begin(); it != m_comp.end(); ++it)
	    (*it)->get_scene_node()->setVisible (!m_ghost);
    }
}

void element::set_selected (bool selected)
{
    if (m_selected != selected) {
	m_selected = selected;
	m_base->set_colour (m_selected ? m_col_selected : m_col_normal);
    }
}

bool element::pointer_clicked (const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    bool ret = false;
    
    for (elem_component_iter it = m_comp.begin(); it != m_comp.end(); ++it)
	if ((*it)->handle_pointer_click (pos, id))
	    ret = true;
    
    switch (id) {
    case OIS::MB_Left:
	if (m_ghost && m_owned) {
	    m_obj.activate();
	    set_selected (true);
	    ret = true;
	} else if ((pos-m_pos).squaredLength() < RADIOUS) {
	    if (m_modifier_1)
		set_selected (!m_selected);
	    else
		set_selected (true);
	    ret = true;
	} else if (!m_modifier_1 && !m_modifier_2)
	    set_selected (false);

	if (m_selected) {
	    m_moving = true;
	    m_click_diff = pos - m_pos;
	}
	
	break;
    default:
	break;
    }

    return ret;
}

bool element::pointer_moved (const Ogre::Vector2& pos)
{
    bool ret = false;
    
    for (elem_component_iter it = m_comp.begin(); it != m_comp.end();)
	if ((*it++)->handle_pointer_move (pos))
	    ret = true;
    
    if (m_ghost && m_owned)
	set_position (pos);
    else if (m_moving)
	set_position (pos - m_click_diff);

    return ret;
}

bool element::pointer_released (const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    for (elem_component_iter it = m_comp.begin(); it != m_comp.end();)
	if ((*it++)->handle_pointer_release (pos, id))
	    return true;

    m_moving = false;

    return false;
}

bool element::key_pressed (const OIS::KeyEvent& e)
{
    switch(e.key) {
    case OIS::KC_DELETE:
	if (m_selected)
	    m_obj.delete_me ();
	break;

    case OIS::KC_E:
	if (m_selected)
	    m_gui_prop.toggle();
	break;

    case OIS::KC_LCONTROL:
    case OIS::KC_RCONTROL:
	//m_modifier_1++;
	m_modifier_1 = 1;
	break;

    case OIS::KC_LSHIFT:
    case OIS::KC_RSHIFT:
	//m_modifier_2++;
	m_modifier_2 = 1;
	break;
	
    default:
	break;
    }
       
    return false;
}

bool element::key_released (const OIS::KeyEvent& e)
{
    switch(e.key) {
    case OIS::KC_LCONTROL:
    case OIS::KC_RCONTROL:
	//m_modifier_1--;
	m_modifier_1 = 0;
	break;

    case OIS::KC_LSHIFT:
    case OIS::KC_RSHIFT:
	//m_modifier_2--;
	m_modifier_2 = 0;
	break;
	
    default:
	break;
    }
    
    return false;
}

void element::node_moved (world_node& obj, vector_2f& dest)
{
    if (obj == m_obj) {
	m_pos.x = dest.x;
	m_pos.y = dest.y;
	m_node->setPosition (m_pos.x, Z_POS, m_pos.y);
    } else if (obj == m_target) {
	m_aimpoint.x = dest.x;
	m_aimpoint.z = dest.y;
    }
    
    m_node->lookAt (m_aimpoint, Node::TS_PARENT);
    
    /*
    for (std::list<Connection*>::iterator it = m_src_con.begin();
	 it != m_src_con.end(); ++it)
	(*it)->updateSource(m_pos);

    for (std::list<Connection*>::iterator it = m_dest_con.begin();
	 it != m_dest_con.end(); ++it)
	(*it)->updateDestiny(m_pos);
    */
}

void element::handle_activate_node (world_node& obj)
{
    if (obj == m_obj)
	set_ghost (false);
}

void element::handle_deactivate_node (world_node& obj)
{
    if (obj == m_obj)
	set_ghost (true);
}

void element::handle_set_param_node (world_node& obj, int param_id)
{
    if (obj == m_obj) {
	if (param_id == node::PARAM_POSITION) {
	    vector_2f dest;
	    obj.get_param (param_id, dest);
	    node_moved (obj, dest);
	}
    
	for (elem_component_iter it = m_comp.begin(); it != m_comp.end(); ++it)
	    (*it)->handle_param_change (obj, param_id);
    } else if (obj == m_target) {
	if (param_id == node::PARAM_POSITION) {
	    vector_2f dest;
	    obj.get_param (param_id, dest);
	    node_moved (obj, dest);
	}
    }
}
