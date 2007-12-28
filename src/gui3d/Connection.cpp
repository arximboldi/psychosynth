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

#include "gui3d/Connection.h"
#include "gui3d/OgreMisc.h"

using namespace Ogre;
using namespace std;

#define CON_WIDTH 0.05f
#define CON_Y     0.001f

void ConnectionObject::build(const Ogre::Vector2& src,
			     const Ogre::Vector2& dst)
{
    Vector2 side =  (src - dst).perpendicular().normalisedCopy() * CON_WIDTH;
    Vector2 top_left  = src + side;
    Vector2 top_right = src - side;
    Vector2 bot_left  = dst + side;
    Vector2 bot_right = dst - side;

    position(top_left.x, CON_Y, top_left.y);
    position(top_right.x, CON_Y, top_right.y);
    position(bot_left.x, CON_Y, bot_left.y);
    position(bot_right.x, CON_Y, bot_right.y);
}

ConnectionObject::ConnectionObject(const std::string& id,
				   const Ogre::ColourValue& colour,
				   const Vector2& src,
				   const Vector2& dest) :
    ManualObject(id)
{
    setDynamic(true);
    createColourMaterial(id, colour);
    begin(getName(), RenderOperation::OT_TRIANGLE_STRIP);
    build(src, dest);
    end();
}

void ConnectionObject::update(const Ogre::Vector2& src,
			      const Ogre::Vector2& dest)
{
    beginUpdate(0);
    build(src, dest);
    end();
}

ConnectionObject::~ConnectionObject()
{
    Ogre::MaterialManager::getSingleton().remove(getName());
}

Connection::Connection(Ogre::SceneManager* scene,
		       const TableObject& src,
		       const TableObject& dest) :
    m_scene(scene),
    m_s_obj(src),
    m_d_obj(dest)
{
    Vector2f v;

    src.getParam(Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION), v);
    m_src.x = v.x;
    m_src.y = v.y;

    dest.getParam(Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION), v);
    m_dest.x = v.x;
    m_dest.y = v.y;
    
    m_node = m_scene->getRootSceneNode()->createChildSceneNode();
    m_line = new ConnectionObject(m_node->getName(), ColourValue(0.7, 0.7, 0, 0.5),
				  m_src, m_dest);
    m_node->attachObject(m_line);

    m_s_obj.addListener(this);
    m_d_obj.addListener(this);
}

Connection::~Connection()
{
    m_scene->destroySceneNode(m_node->getName());

    m_s_obj.deleteListener(this);
    m_d_obj.deleteListener(this);

    delete m_line;
}

void Connection::handleSetParamObject(TableObject& obj, Object::ParamID id)
{
    if (id == Object::ParamID(Object::PARAM_COMMON, Object::PARAM_POSITION)) {
	Vector2f pos;
	obj.getParam(id, pos);
    
	if (obj.getID() == m_s_obj.getID()) {
	    m_src.x = pos.x;
	    m_src.y = pos.y;
	} else if (obj.getID() == m_d_obj.getID()) {
	    m_dest.x = pos.x;
	    m_dest.y = pos.y;
	}

	m_line->update(m_src, m_dest);
    }
}

/*
  void Connection::updateSource(const Vector2& pos)
  {
  m_src = pos;
  m_line->update(m_src, m_dest);
  }

  void Connection::updateDestiny(const Vector2& pos)
  {
  m_dest = pos;
  m_line->update(m_src, m_dest);
  }

  void Connection::update(const Vector2& src, const Vector2& dest)
  {
  m_src = src;
  m_dest = dest;
  m_line->update(m_src, m_dest);
  }
*/

