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

#include "gui3d/ElemSecondComponent.h"
#include "gui3d/OgreMisc.h"

#define SECOND_RAD 1.2
#define SECOND_MIN_ANGLE 85
#define SECOND_POINT_MIN_ANGLE -85
#define SECOND_RANGE_ANGLE 170
#define SECOND_POINT_RAD 0.2

using namespace std;
using namespace Ogre;
using namespace psynth;

ElemSecondComponent::ElemSecondComponent(int param,
					 float min_val, float max_val) :
    m_indicator(NULL),
    m_indicator_point(NULL),
    m_param(param),
    m_min_val(min_val),
    m_max_val(max_val),
    m_changing(false)
{
}

ElemSecondComponent::~ElemSecondComponent()
{
    delete m_indicator;
    delete m_indicator_point;
}

void ElemSecondComponent::init()
{
    getParent()->getObject().getParam(m_param, m_old_value);
    m_angle = Degree(m_old_value / (m_max_val - m_min_val) *
		     SECOND_RANGE_ANGLE + SECOND_POINT_MIN_ANGLE);

    m_indicator = new FlatRing(string("SEC1")+getSceneNode()->getName(),
			       Degree(SECOND_MIN_ANGLE), Degree(SECOND_MIN_ANGLE +
								SECOND_RANGE_ANGLE),
			       SECOND_RAD - 0.05, SECOND_RAD + 0.05,
			       ColourValue(1,1,1,0.5));

    m_indicator_point = new FlatRing(string("SECP")+getSceneNode()->getName(),
				     Degree(0), Degree(360),
				     0, 0.15,
				     ColourValue(1,1,1,0.7));

    getSceneNode()->attachObject(m_indicator);

    m_point_yaw = getSceneNode()->createChildSceneNode();
    m_point_node = m_point_yaw->createChildSceneNode();
    m_point_node->attachObject(m_indicator_point);
    m_point_node->setPosition(SECOND_RAD, 0, 0);
    m_point_yaw->yaw(m_angle);
}

bool ElemSecondComponent::handlePointerMove(Ogre::Vector2 pos)
{
    if (m_changing) {
	Quaternion orig_angle = getSceneNode()->getWorldOrientation();
	Vector3 abs_position = getSceneNode()->getWorldPosition();
	Vector2 abs_pos = Vector2(abs_position.x, abs_position.z);

	pos -= abs_pos;
	pos = yawVector2(pos, orig_angle.getYaw().valueRadians());
	float new_angle = Math::ATan2(pos.x, pos.y).valueRadians();

	if (new_angle < -Math::PI/2)
	    new_angle = Math::PI;

	float new_val = new_angle/Math::PI * (m_max_val - m_min_val) + m_min_val;
	
	if (new_val < m_min_val)
	    new_val = m_min_val;
	if (new_val > m_max_val)
	    new_val = m_max_val;

	getParent()->getObject().setParam(m_param, new_val);
    }
    return false;
}

bool ElemSecondComponent::handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    Vector3 real_position = m_point_node->getWorldPosition();
    Vector2 real_pos = Vector2(real_position.x, real_position.z);

    if ((pos - real_pos).length() < SECOND_POINT_RAD) {
	m_changing = true;
	return true;
    }
	
    return false;
}

bool ElemSecondComponent::handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    m_changing = false;
    return false;
}

void ElemSecondComponent::handleParamChange(TableObject& obj, int param_id)
{
    if (param_id == m_param) {
	Degree new_angle;
	obj.getParam(m_param, m_old_value);
	
	new_angle = Degree(m_old_value / (m_max_val - m_min_val) *
			   SECOND_RANGE_ANGLE + SECOND_POINT_MIN_ANGLE);
	m_point_yaw->yaw(new_angle - m_angle);
	m_angle = new_angle;
    }
}
