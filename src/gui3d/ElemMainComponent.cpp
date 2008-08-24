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

#include "gui3d/ElemMainComponent.h"
#include <libpsynth/common/misc.h>

#define INDICATOR_MIN_ANGLE 175
#define INDICATOR_RANGE_ANGLE 170

#define COLOUR_ACTIVE  ColourValue(1, 1, 1, 0.7)
#define COLOUR_PASSIVE ColourValue(1, 1, 1, 0.4)

using namespace std;
using namespace Ogre;
using namespace psynth;

ElemMainComponent::ElemMainComponent(const std::string& mesh,
				     int param,
				     float min_val, float max_val) :
    m_mesh(mesh),
    m_mesh_ent(NULL),
    m_indicator(NULL),
    m_indicator_fill(NULL),
    m_param(param),
    m_min_val(min_val),
    m_max_val(max_val),
    m_rotating(false)
{
}

ElemMainComponent::~ElemMainComponent()
{
    delete m_indicator;
    delete m_indicator_fill;
}

void ElemMainComponent::setMesh(const std::string& mesh)
{
    if (m_mesh_ent) {
	m_ent_node->detachObject(m_mesh_ent);
	getParent()->getScene()->destroyEntity(m_mesh_ent);
    }

    m_mesh_ent = getParent()->getScene()->createEntity(string("EE") +
						       itoa(getParent()->getObject().get_id (),10),
						       mesh);
    m_ent_node->attachObject(m_mesh_ent);
    updateVisibility();
}

bool ElemMainComponent::handlePointerMove(Ogre::Vector2 pos)
{
    if (m_rotating) {
	Vector2 rad   = m_last_mouse_pos - getParent()->getPosition();
	Vector2 delta = pos - m_last_mouse_pos;
	float value = m_old_value  + (rad.perpendicular().normalisedCopy() * delta).length()
	    * (rad.crossProduct(delta) < 0.0 ? 1 : -1) * (m_max_val - m_min_val) / (4 * Math::PI);

	if (value > m_max_val)
	    value = m_max_val;
	if (value < m_min_val)
	    value = m_min_val;

	getParent()->getObject().set_param (m_param, value);
    }

    m_last_mouse_pos = pos;

    return false;
}

bool ElemMainComponent::handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    if (getParent()->isSelected() && id == OIS::MB_Right) {
	m_rotating = true;
	return true;
    }

    return false;
}

bool ElemMainComponent::handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    if (id == OIS::MB_Right)
	m_rotating = false;

    return false;
}

void ElemMainComponent::init()
{
    getParent()->getObject().get_param (m_param, m_old_value);

    m_ent_node = getSceneNode()->createChildSceneNode();

    setMesh(m_mesh);

    m_indicator = new FlatRing(string("IND1")+getSceneNode()->getName(),
			       Degree(INDICATOR_MIN_ANGLE), Degree(INDICATOR_MIN_ANGLE +
				   INDICATOR_RANGE_ANGLE),
			       Element::RADIOUS + 0.1, Element::RADIOUS + 0.3,
			       COLOUR_PASSIVE);

    m_indicator_fill = new FlatRing(string("IND2")+getSceneNode()->getName(),
				    Degree(INDICATOR_MIN_ANGLE),
				    Degree(INDICATOR_MIN_ANGLE +
					   (m_old_value-m_min_val) / (m_max_val-m_min_val) *
					   INDICATOR_RANGE_ANGLE),
				    Element::RADIOUS + 0.1, Element::RADIOUS + 0.3,
				    COLOUR_ACTIVE);

    getSceneNode()->attachObject(m_indicator);
    getSceneNode()->attachObject(m_indicator_fill);
}

void ElemMainComponent::handleParamChange(world_node& obj, int param_id)
{
    if (param_id == m_param) {
	float new_val;
	obj.get_param (m_param, new_val);

	m_ent_node->yaw(Radian((new_val - m_old_value)/(m_max_val-m_min_val) * 2 * Math::PI));
	m_indicator_fill->setEndAngle(Degree(INDICATOR_MIN_ANGLE +
					     ((new_val-m_min_val)/(m_max_val-m_min_val))
					     * INDICATOR_RANGE_ANGLE));
	m_indicator_fill->update();
	m_old_value = new_val;
    }
}

ElemMultiMainComponent::ElemMultiMainComponent(int param_1,
					       float min_val, float max_val,
					       int param_2,
					       const char** names) :
    ElemMainComponent(names[0], param_1, min_val, max_val),
    m_param(param_2),
    m_names(names)
{
}

void ElemMultiMainComponent::handleParamChange(world_node& obj, int param_id)
{
    if (param_id == m_param) {
	int val;
	obj.get_param (param_id, val);

	setMesh(m_names[val]);
    } else
	ElemMainComponent::handleParamChange(obj, param_id);
}
