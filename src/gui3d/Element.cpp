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

#include "gui3d/Element.h"
#include "gui3d/QueryFlags.h"
#include "common/Misc.h"

#define ELEMENT_Z_POS       0.001f
#define ROTATION_FACTOR     100.0f
#define INDICATOR_MIN_ANGLE 260
#define INDICATOR_RANGE_ANGLE 340

using namespace std;
using namespace Ogre;

inline void ElemComponent::updateVisibility()
{
    m_node->setVisible(!m_parent->isGhost());
}

ElemMainComponent::ElemMainComponent(const std::string& mesh,
				     int param, float min_val, float max_val) :
    m_mesh(mesh),
    m_mesh_ent(NULL),
    m_param(param),
    m_min_val(min_val),
    m_max_val(max_val),
    m_rotating(false)
{
}

void ElemMainComponent::setMesh(const std::string& mesh)
{
    if (m_mesh_ent) {
	m_ent_node->detachObject(m_mesh_ent);
	getParent()->getScene()->destroyEntity(m_mesh_ent);
    }

    m_mesh_ent = getParent()->getScene()->createEntity(string("EE") +
						       itoa(getParent()->getObject().getID(),10),
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

	getParent()->getObject().setParam(m_param, value);
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
    getParent()->getObject().getParam(m_param, m_old_value);

    m_ent_node = getSceneNode()->createChildSceneNode();

    setMesh(m_mesh);

    m_indicator = new FlatRing(string("IND1")+getSceneNode()->getName(),
			       Degree(INDICATOR_MIN_ANGLE), Degree(INDICATOR_MIN_ANGLE +
				   INDICATOR_RANGE_ANGLE),
			       Element::RADIOUS + 0.1, Element::RADIOUS + 0.3,
			       ColourValue(1,1,1,0.5));

    m_indicator_fill = new FlatRing(string("IND2")+getSceneNode()->getName(),
				    Degree(INDICATOR_MIN_ANGLE),
				    Degree(INDICATOR_MIN_ANGLE +
					   m_old_value / (m_max_val-m_min_val) *
					   INDICATOR_RANGE_ANGLE),
				    Element::RADIOUS + 0.1, Element::RADIOUS + 0.3,
				    ColourValue(1,1,1,0.6));

    getSceneNode()->attachObject(m_indicator);
    getSceneNode()->attachObject(m_indicator_fill);
}

void ElemMainComponent::handleParamChange(TableObject& obj, int param_id)
{
    if (param_id == m_param) {
	float new_val;
	obj.getParam(m_param, new_val);
	m_ent_node->yaw(Radian((new_val - m_old_value)/(m_max_val-m_min_val) * 2 * Math::PI));
	m_indicator_fill->setEndAngle(Degree(INDICATOR_MIN_ANGLE +
					     new_val/(m_max_val-m_min_val)
					     * INDICATOR_RANGE_ANGLE));
	m_indicator_fill->update();
	m_old_value = new_val;
    }
}

ElemMultiMainComponent::ElemMultiMainComponent(int param_1, float min_val, float max_val,
					       int param_2, const char** names) :
    ElemMainComponent(names[0], param_1, min_val, max_val),
    m_param(param_2),
    m_names(names)
{
}
    
void ElemMultiMainComponent::handleParamChange(TableObject& obj, int param_id)
{
    if (param_id == m_param) {
	int val;
	obj.getParam(param_id, val);
	setMesh(m_names[val]);
    } else
	ElemMainComponent::handleParamChange(obj, param_id);
}

Element::Element(const TableObject& obj, Ogre::SceneManager* scene) :
    m_obj(obj),
    m_col_ghost(0.8, 0.8, 0.1, 0.4),
    m_col_selected(0.8, 0.1, 0.1, 0.7),
    m_col_normal(0.5, 0.8, 0.5, 0.5),
    m_scene(scene),
    m_selected(false),
    m_moving(false)
{
    m_base = new FlatRing(string("EB") + itoa(m_obj.getID(),10),
			  Degree(0), Degree(360),
			  0, 1,
			  m_col_ghost);

    m_node = scene->getRootSceneNode()->createChildSceneNode();
    
    m_node->attachObject(m_base);
    m_pos.x = obj.getX();
    m_pos.y = obj.getY();
    m_node->setPosition(Vector3(m_pos.x, ELEMENT_Z_POS, m_pos.y));

    m_selected = false;
    setGhost(true);
}

Element::~Element()
{
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	delete *it;
    
    m_scene->destroySceneNode(m_node->getName());
}

void Element::addComponent(ElemComponent* comp)
{
    SceneNode* node;
    m_comp.push_back(comp);
    node = m_node->createChildSceneNode();
    comp->setSceneNode(node);
    comp->setParent(this);
    comp->init();
    /* TODO */
    node->setVisible(!m_ghost);
}

void Element::setPosition(const Ogre::Vector2& pos)
{
    m_obj.move(pos.x, pos.y);
}

void Element::setGhost(bool ghost)
{
    if (m_ghost != ghost) {
	m_ghost = ghost;
	m_base->setColour(m_ghost ? m_col_ghost : m_col_normal);
	
	for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	    (*it)->getSceneNode()->setVisible(!m_ghost);
    }
}

void Element::setSelected(bool selected)
{
    if (m_selected != selected) {
	m_selected = selected;
	m_base->setColour(m_selected ? m_col_selected : m_col_normal);
    }
}

bool Element::pointerClicked(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	if ((*it)->handlePointerClick(pos, id))
	    return true;
    
    switch (id) {
    case OIS::MB_Left:
	if (m_ghost) {
	    m_obj.activate();
	    return true;
	} else if ((pos-m_pos).squaredLength() < RADIOUS) {
	    setSelected(true);
	    m_moving = true;
	    m_click_diff = pos - m_pos;
	    return true;
	} else
	    setSelected(false);
	break;
    default:
	break;
    }

    return false;
}

bool Element::pointerMoved(const Ogre::Vector2& pos)
{
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end();)
	if ((*it++)->handlePointerMove(pos))
	    return true;
    
    if (m_ghost)
	setPosition(pos);
    else if (m_moving)
	setPosition(pos - m_click_diff);

    return false;
}

bool Element::pointerReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id)
{
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end();)
	if ((*it++)->handlePointerRelease(pos, id))
	    return true;

    m_moving = false;

    return false;
}

bool Element::keyPressed(const OIS::KeyEvent& e)
{
    if (m_selected && e.key == OIS::KC_DELETE) {
	m_obj.deleteMe();
	return true;
    }
    
    return false;
}

bool Element::keyReleased(const OIS::KeyEvent& e)
{
    return false;
}

void Element::handleMoveObject(TableObject& obj)
{
    m_pos.x = obj.getX();
    m_pos.y = obj.getY();
    m_node->setPosition(m_pos.x, ELEMENT_Z_POS, m_pos.y);
    //m_node->yaw(Math::ATan2(m_pos.x, m_pos.y));
    m_node->lookAt(Vector3(0, ELEMENT_Z_POS, 0), Node::TS_PARENT);
    
    /*
    for (std::list<Connection*>::iterator it = m_src_con.begin();
	 it != m_src_con.end(); ++it)
	(*it)->updateSource(m_pos);

    for (std::list<Connection*>::iterator it = m_dest_con.begin();
	 it != m_dest_con.end(); ++it)
	(*it)->updateDestiny(m_pos);
    */
}

void Element::handleActivateObject(TableObject& obj)
{
    setGhost(false);
}

void Element::handleDeactivateObject(TableObject& obj)
{
    setGhost(true);
}

void Element::handleSetParamObject(TableObject& ob, int param_id)
{
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	(*it)->handleParamChange(ob, param_id);
}

#include "object/ObjectOscillator.h"

ElementOscillator::ElementOscillator(const TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* names[ObjectOscillator::N_OSC_TYPES] =
	{"oscsine.mesh", "oscsine.mesh", "oscsine.mesh", "oscsine.mesh"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectOscillator::PARAM_FREQUENCY, 20.0f, 5000.0f,
		     ObjectOscillator::PARAM_WAVE, names));
}

#include "object/ObjectMixer.h"

ElementMixer::ElementMixer(const TableObject& obj,
			   Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{ 
    addComponent(new ElemMainComponent("oscsine.mesh",
				       ObjectMixer::PARAM_AMPLITUDE, 0.0f, 1.0f));   
}
