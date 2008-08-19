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

#include <libpsynth/common/misc.h>
#include "gui3d/ElemTogglerComponent.h"
#include "gui3d/OgreMisc.h"

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

ElemTogglerComponent::ElemTogglerComponent(int param_num,
					   int param_first,
					   int param_step) :
    m_param_num(param_num),
    m_param_first(param_first),
    m_param_step(param_step)
{
    TaskManager::instance().attach(this);
}

ElemTogglerComponent::~ElemTogglerComponent()
{
    TaskManager::instance().detach(this);
}

void ElemTogglerComponent::createToggles()
{
    int num_toggles;
    int values;
    float angle_width;
    float curr_angle;
    Toggle tog;
    TableObject& obj = getParent()->getObject();

    obj.getParam(m_param_step, m_step);
    obj.getParam(m_param_num, num_toggles);
    m_toggles.resize(num_toggles);

    angle_width = (360.0 / num_toggles) - TOGGLE_GAP; 
    for (int i = 0; i < num_toggles; ++i) {
	obj.getParam(m_param_first + i, tog.second);
	
	curr_angle = (360.0 / num_toggles) * i;
	tog.first = new FlatRing(string("TOG") +
				 itoa(i, 10) +
				 getSceneNode()->getName(),
				 Degree(curr_angle),
				 Degree(curr_angle - angle_width),
				 TOGGLE_RAD - 0.1,
				 TOGGLE_RAD + 0.1,
				 tog.second ? COLOUR_ACTIVE : COLOUR_PASSIVE,
				 angle_width * 360.0f/20.0);

	m_toggles[i] = tog;
	getSceneNode()->attachObject(tog.first);
    }

    updateCurrentStepColour(m_step % num_toggles);
}

void ElemTogglerComponent::destroyToggles()
{
    int i;
    for (i = 0; i < m_toggles.size(); ++i) {
	getSceneNode()->detachObject(m_toggles[i].first);
	delete m_toggles[i].first;
	m_toggles[i].first = 0;
    }
}

void ElemTogglerComponent::init()
{
    createToggles();
}

bool ElemTogglerComponent::handlePointerMove(Ogre::Vector2 pos)
{
    return false;
}

bool ElemTogglerComponent::handlePointerClick(Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    Quaternion orig_angle = getSceneNode()->getWorldOrientation();
    Vector3 abs_position = getSceneNode()->getWorldPosition();
    Vector2 abs_pos = Vector2(abs_position.x, abs_position.z);

    if ((pos - abs_pos).length() < TOGGLE_RAD + 0.1 &&
	(pos - abs_pos).length() > TOGGLE_RAD - 0.1) {
	pos -= abs_pos;
	pos = yawVector2(pos, orig_angle.getYaw().valueRadians() - Math::PI/2);
	float new_angle = Math::PI - Math::ATan2(pos.x, pos.y).valueRadians();
		
	int index = new_angle / ((2.0 * Math::PI) / m_toggles.size());

	m_toggles[index].second = !m_toggles[index].second;
	getParent()->getObject().setParam(m_param_first + index,
					  m_toggles[index].second);
    }
    return false;
}

bool ElemTogglerComponent::handlePointerRelease(Ogre::Vector2 pos, OIS::MouseButtonID id)
{
    return false;
}

void ElemTogglerComponent::handleParamChange(TableObject& obj, int param_id)
{
    if (param_id == m_param_num) {
	destroyToggles();
	createToggles();
    } else if (param_id >= m_param_first &&
	       param_id <= m_param_first + m_toggles.size()) {
	int index = param_id - m_param_first;
	
	obj.getParam(param_id, m_toggles[index].second);
	if (m_step == index)
	    updateCurrentStepColour(index);
	else
	    updateStepColour(index);
    }
}

void ElemTogglerComponent::update(int ms)
{
    if (getParent()) {
	int new_step;
	getParent()->getObject().getParam(m_param_step, new_step);
	if (new_step != m_step) {
	    updateStepColour(m_step % m_toggles.size());
	    m_step = new_step;
	    updateCurrentStepColour(m_step % m_toggles.size());
	}
    }
}

void ElemTogglerComponent::updateStepColour(int step)
{
    m_toggles[step].first->setColour(m_toggles[step].second ?
				     COLOUR_ACTIVE :
				     COLOUR_PASSIVE);
    m_toggles[step].first->update();
}

void ElemTogglerComponent::updateCurrentStepColour(int step)
{
    m_toggles[step].first->setColour(m_toggles[step].second ?
				     COLOUR_CURRENT_ACTIVE :
				     COLOUR_CURRENT_PASSIVE);
    m_toggles[step].first->update();
}

