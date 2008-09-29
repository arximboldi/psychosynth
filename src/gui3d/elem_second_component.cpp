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

#include "gui3d/elem_second_component.hpp"
#include "gui3d/ogre_misc.hpp"

#define SECOND_RAD 1.2
#define SECOND_MIN_ANGLE -5
#define SECOND_POINT_MIN_ANGLE 5
#define SECOND_RANGE_ANGLE 170
#define SECOND_POINT_RAD 0.2

#define SECOND_COLOUR       ColourValue(1,1,1,0.4)
#define SECOND_POINT_COLOUR ColourValue(1,1,1,0.7)
#define SECOND_POINT_CLICK_COLOUR ColourValue(1,0,0,0.7)

using namespace std;
using namespace Ogre;
using namespace psynth;

elem_second_component::elem_second_component(int param,
					     float min_val, float max_val) :
    m_indicator(NULL),
    m_indicator_point(NULL),
    m_param(param),
    m_min_val(min_val),
    m_max_val(max_val),
    m_changing(false)
{
}

elem_second_component::~elem_second_component()
{
    delete m_indicator;
    delete m_indicator_point;
}

void elem_second_component::init()
{
    get_parent()->get_node().get_param (m_param, m_old_value);
    m_angle = Degree(180-((m_old_value - m_min_val) / (m_max_val - m_min_val) *
			  SECOND_RANGE_ANGLE + SECOND_POINT_MIN_ANGLE));

    m_indicator = new flat_ring (string("SEC1")+get_scene_node()->getName(),
				 Degree(SECOND_MIN_ANGLE),
				 Degree(SECOND_MIN_ANGLE + SECOND_RANGE_ANGLE),
				 SECOND_RAD - 0.05,
				 SECOND_RAD + 0.05,
				 SECOND_COLOUR);

    m_indicator_point = new flat_ring (string("SECP")+get_scene_node()->getName(),
				       Degree(0), Degree(360),
				       0, 0.15,
				       SECOND_POINT_COLOUR);

    get_scene_node()->attachObject(m_indicator);

    m_point_yaw = get_scene_node()->createChildSceneNode();
    m_point_node = m_point_yaw->createChildSceneNode();
    m_point_node->attachObject(m_indicator_point);
    m_point_node->setPosition(SECOND_RAD, 0, 0);
    m_point_yaw->yaw(m_angle);
}

bool elem_second_component::handle_pointer_move (Ogre::Vector2 pos)
{
    if (m_changing) {
	Quaternion orig_angle = get_scene_node()->getWorldOrientation();
	Vector3 abs_position = get_scene_node()->getWorldPosition();
	Vector2 abs_pos = Vector2(abs_position.x, abs_position.z);

	pos -= abs_pos;
	pos = yaw_vector2 (pos, orig_angle.getYaw().valueRadians() - 3*Math::PI/2);
	float new_angle = Math::PI - Math::ATan2(pos.x, pos.y).valueRadians();

	if (new_angle < -Math::PI/2)
	    new_angle = Math::PI;
	
	float new_val = new_angle/Math::PI * (m_max_val - m_min_val) + m_min_val;
	
	if (new_val < m_min_val)
	    new_val = m_min_val;
	if (new_val > m_max_val)
	    new_val = m_max_val;

	get_parent()->get_node().set_param (m_param, new_val);
    }
    return false;
}

bool elem_second_component::handle_pointer_click (Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    Vector3 real_position = m_point_node->getWorldPosition();
    Vector2 real_pos = Vector2(real_position.x, real_position.z);

    if ((pos - real_pos).length() < SECOND_POINT_RAD) {
	m_indicator_point->set_colour (SECOND_POINT_CLICK_COLOUR);
	m_indicator_point->update ();
	m_changing = true;
	return true;
    }
	
    return false;
}

bool elem_second_component::handle_pointer_release (Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    if (m_changing) {
	m_indicator_point->set_colour (SECOND_POINT_COLOUR);
	m_indicator_point->update ();
	m_changing = false;
    }
    return false;
}

void elem_second_component::handle_param_change (world_node& obj, int param_id)
{
    if (param_id == m_param) {
	Degree new_angle;
	obj.get_param (m_param, m_old_value);

	new_angle = -Degree((m_old_value - m_min_val) / (m_max_val - m_min_val) *
			    SECOND_RANGE_ANGLE + SECOND_POINT_MIN_ANGLE + 180.0f);
	m_point_yaw->yaw((new_angle - m_angle));
	m_angle = new_angle;
    }
}
