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

#include <psynth/base/misc.hpp>
#include "gui3d/elem_toggler_component.hpp"
#include "gui3d/ogre_misc.hpp"

#define TOGGLE_RAD 1.5
#define TOGGLE_GAP 4

#define COLOUR_ACTIVE  ColourValue(1, 1, 1, 0.7)
#define COLOUR_PASSIVE ColourValue(1, 1, 1, 0.3)
#define COLOUR_CURRENT_ACTIVE ColourValue(1, 0, 0, 0.7)
#define COLOUR_CURRENT_PASSIVE ColourValue(1, 0, 0, 0.3)

#define NUM_VERTEX 10

using namespace std;
using namespace Ogre;
using namespace psynth;

elem_toggler_component::elem_toggler_component(int param_num,
					   int param_first,
					   int param_step) :
    m_param_num (param_num),
    m_param_first (param_first),
    m_param_step (param_step)
{
    task_manager::self ().attach (this);
}

elem_toggler_component::~elem_toggler_component()
{
    task_manager::self ().detach (this);
}

void elem_toggler_component::create_toggles ()
{
    int num_toggles;
    int values;
    float angle_width;
    float curr_angle;
    Toggle tog;
    world_node& obj = get_parent()->get_node();

    obj.get_param(m_param_step, m_step);
    obj.get_param(m_param_num, num_toggles);
    m_toggles.resize(num_toggles);

    angle_width = (360.0 / num_toggles) - TOGGLE_GAP; 
    for (int i = 0; i < num_toggles; ++i) {
	obj.get_param (m_param_first + i, tog.second);
	
	curr_angle = (360.0 / num_toggles) * i;
	tog.first = new flat_ring (string("TOG") +
				   itoa(i, 10) +
				   get_scene_node()->getName(),
				   Degree(curr_angle),
				   Degree(curr_angle - angle_width),
				   TOGGLE_RAD - 0.1,
				   TOGGLE_RAD + 0.1,
				   tog.second ? COLOUR_ACTIVE : COLOUR_PASSIVE,
				   angle_width * 360.0f/20.0);

	m_toggles[i] = tog;
	get_scene_node()->attachObject (tog.first);
    }

    update_current_step_colour (m_step % num_toggles);
}

void elem_toggler_component::destroy_toggles ()
{
    int i;
    for (i = 0; i < m_toggles.size(); ++i) {
	get_scene_node()->detachObject(m_toggles[i].first);
	delete m_toggles[i].first;
	m_toggles[i].first = 0;
    }
}

void elem_toggler_component::init()
{
    create_toggles ();
}

bool elem_toggler_component::handle_pointer_move (Ogre::Vector2 pos)
{
    return false;
}

bool elem_toggler_component::handle_pointer_click (Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    Quaternion orig_angle = get_scene_node()->_getDerivedOrientation();
    Vector3 abs_position = get_scene_node()->_getDerivedPosition();
    Vector2 abs_pos = Vector2(abs_position.x, abs_position.z);

    if ((pos - abs_pos).length() < TOGGLE_RAD + 0.1 &&
	(pos - abs_pos).length() > TOGGLE_RAD - 0.1) {
	pos -= abs_pos;
	pos = yaw_vector2 (pos, orig_angle.getYaw().valueRadians() - Math::PI/2);
	float new_angle = Math::PI - Math::ATan2(pos.x, pos.y).valueRadians();
		
	int index = new_angle / ((2.0 * Math::PI) / m_toggles.size());

	m_toggles[index].second = !m_toggles[index].second;
	get_parent()->get_node().set_param(m_param_first + index,
					   m_toggles[index].second);
    }
    return false;
}

bool elem_toggler_component::handle_pointer_release (Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    return false;
}

void elem_toggler_component::handle_param_change (world_node& obj, int param_id)
{
    if (param_id == m_param_num) {
	destroy_toggles ();
	create_toggles ();
    } else if (param_id >= m_param_first &&
	       param_id <= m_param_first + m_toggles.size()) {
	int index = param_id - m_param_first;
	
	obj.get_param(param_id, m_toggles[index].second);
	if (m_step == index)
	    update_current_step_colour (index);
	else
	    update_step_colour (index);
    }
}

void elem_toggler_component::update (int ms)
{
    if (get_parent()) {
	int new_step;
	get_parent()->get_node().get_param(m_param_step, new_step);
	if (new_step != m_step) {
	    update_step_colour (m_step % m_toggles.size());
	    m_step = new_step;
	    update_current_step_colour (m_step % m_toggles.size());
	}
    }
}

void elem_toggler_component::update_step_colour (int step)
{
    m_toggles[step].first->set_colour (m_toggles[step].second ?
				       COLOUR_ACTIVE :
				       COLOUR_PASSIVE);
    m_toggles[step].first->update();
}

void elem_toggler_component::update_current_step_colour (int step)
{
    m_toggles[step].first->set_colour (m_toggles[step].second ?
				       COLOUR_CURRENT_ACTIVE :
				       COLOUR_CURRENT_PASSIVE);
    m_toggles[step].first->update();
}

