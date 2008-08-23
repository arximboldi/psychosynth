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

#include <libpsynth/common/misc.h>

#define ROTATION_FACTOR     100.0f

#define ELEMENT_GHOST_COLOUR    ColourValue(0.8, 0.8, 0.1, 0.4)
#define ELEMENT_NORMAL_COLOUR   ColourValue(1, 1, 1, 0.5)
#define ELEMENT_SELECTED_COLOUR ColourValue(1, 0, 0, 0.7)

using namespace std;
using namespace Ogre;
using namespace psynth;

Element::Element(TableObject& obj, Ogre::SceneManager* scene) :
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
    m_base = new FlatRing(string("EB") + itoa(m_obj.getID(),10),
			  Degree(0), Degree(360),
			  0, 1,
			  m_col_ghost);

    m_gui_prop.setActive(false);
    
    m_node = scene->getRootSceneNode()->createChildSceneNode();
    
    m_node->attachObject(m_base);

    vector_2f v;
    obj.getParam(psynth::node::PARAM_POSITION, v);
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
    node->yaw(Radian(Math::PI / 2), Node::TS_LOCAL);
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

	vector_2f v;
	m_target.getParam(psynth::node::PARAM_POSITION, v);
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
    vector_2f dest;
    dest.x = pos.x;
    dest.y = pos.y;
    m_obj.setParam(psynth::node::PARAM_POSITION, dest);
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
	    setSelected(true);
	    ret = true;
	} else if ((pos-m_pos).squaredLength() < RADIOUS) {
	    if (m_modifier_1)
		setSelected(!m_selected);
	    else
		setSelected(true);
	    ret = true;
	} else if (!m_modifier_1 && !m_modifier_2)
	    setSelected(false);

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

bool Element::keyReleased(const OIS::KeyEvent& e)
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

void Element::objectMoved(TableObject& obj, vector_2f& dest)
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

void Element::handleSetParamObject(TableObject& obj, int param_id)
{
    if (obj == m_obj) {
	if (param_id == node::PARAM_POSITION) {
	    vector_2f dest;
	    obj.getParam(param_id, dest);
	    objectMoved(obj, dest);
	}
    
	for (ElemComponentIter it = m_comp.begin(); it != m_comp.end(); ++it)
	    (*it)->handleParamChange(obj, param_id);
    } else if (obj == m_target) {
	if (param_id == node::PARAM_POSITION) {
	    vector_2f dest;
	    obj.getParam(param_id, dest);
	    objectMoved(obj, dest);
	}
    }
}
