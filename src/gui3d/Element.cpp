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

#define ROTATION_FACTOR     100.0f

using namespace std;
using namespace Ogre;

Element::Element(TableObject& obj, Ogre::SceneManager* scene) :
    m_obj(obj),
    m_col_ghost(0.8, 0.8, 0.1, 0.4),
    m_col_selected(0.8, 0.1, 0.1, 0.7),
    m_col_normal(0.5, 0.8, 0.5, 0.5),
    m_scene(scene),
    m_aimpoint(0, Z_POS, 0),
    m_click_diff(0,0),
    m_pos(0,0),
    m_owned(false),
    m_ghost(true),
    m_selected(false),
    m_moving(false),
    m_gui_prop(obj)
{
    m_base = new FlatRing(string("EB") + itoa(m_obj.getID(),10),
			  Degree(0), Degree(360),
			  0, 1,
			  m_col_ghost);

    m_gui_prop.setActive(false);
    
    m_node = scene->getRootSceneNode()->createChildSceneNode();
    
    m_node->attachObject(m_base);

    Vector2f v;
    obj.getParam(Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION), v);
    m_pos.x = v.x;
    m_pos.y = v.y;
    m_node->setPosition(Vector3(m_pos.x, Z_POS, m_pos.y));

    m_selected = false;
    setGhost(true);
}

Element::~Element()
{
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	delete *it;
    
    m_scene->destroySceneNode(m_node->getName());
    delete m_base;
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

void Element::setTarget(const TableObject& obj)
{
    if (!m_target.isNull()) {
	m_target.deleteListener(this);
	m_aimpoint = Vector3(0, Z_POS, 0);
    }
    
    m_target = obj;

    if (!m_target.isNull()) {
	m_target.addListener(this);

	Vector2f v;
	m_target.getParam(Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION), v);
	m_aimpoint.x = v.x;
	m_aimpoint.y = v.y;
	
	m_aimpoint = Vector3(v.x, Z_POS, v.y);
    }

    m_node->lookAt(m_aimpoint, Node::TS_PARENT);
}

void Element::clearTarget(const TableObject& obj)
{
    if (m_target == obj) {
	m_target.deleteListener(this);
	m_target = TableObject();
	m_aimpoint = Vector3(0, Z_POS, 0);
    }
}

void Element::setPosition(const Ogre::Vector2& pos)
{
    Vector2f dest;
    dest.x = pos.x;
    dest.y = pos.y;
    m_obj.setParam(Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION), dest);
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
    bool ret = false;
    
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	if ((*it)->handlePointerClick(pos, id))
	    ret = true;
    
    switch (id) {
    case OIS::MB_Left:
	if (m_ghost && m_owned) {
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

    return ret;
}

bool Element::pointerMoved(const Ogre::Vector2& pos)
{
    bool ret = false;
    
    for (ElemComponentIter it = m_comp.begin(); it != m_comp.end();)
	if ((*it++)->handlePointerMove(pos))
	    ret = true;
    
    if (m_ghost && m_owned)
	setPosition(pos);
    else if (m_moving)
	setPosition(pos - m_click_diff);

    return ret;
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
    switch(e.key) {
    case OIS::KC_DELETE:
	if (m_selected)
	    m_obj.deleteMe();
	break;

    case OIS::KC_E:
	if (m_selected)
	    m_gui_prop.toggle();
	break;
	
    default:
	break;
    }
       
    return false;
}

bool Element::keyReleased(const OIS::KeyEvent& e)
{
    return false;
}

void Element::objectMoved(TableObject& obj, Vector2f& dest)
{
    if (obj == m_obj) {
	m_pos.x = dest.x;
	m_pos.y = dest.y;
	m_node->setPosition(m_pos.x, Z_POS, m_pos.y);
    } else if (obj == m_target) {
	m_aimpoint.x = dest.x;
	m_aimpoint.z = dest.y;
    }
    
    m_node->lookAt(m_aimpoint, Node::TS_PARENT);
    
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
    if (obj == m_obj)
	setGhost(false);
}

void Element::handleDeactivateObject(TableObject& obj)
{
    if (obj == m_obj)
	setGhost(true);
}

void Element::handleSetParamObject(TableObject& obj, Object::ParamID param_id)
{
    if (obj == m_obj) {
	if (param_id == Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION)) {
	    Vector2f dest;
	    obj.getParam(param_id, dest);
	    objectMoved(obj, dest);
	}
    
	for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	    (*it)->handleParamChange(obj, param_id);
    }
}
