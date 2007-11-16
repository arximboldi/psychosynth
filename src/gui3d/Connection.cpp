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

#define CON_WIDTH 0.1f
#define CON_Y     0.001f

void ConnectionObject::build(const Ogre::Vector2& src,
			     const Ogre::Vector2& dst)
{
    Vector2 top_left  = src + src.perpendicular().normalisedCopy() * CON_WIDTH;
    Vector2 top_right = src - src.perpendicular().normalisedCopy() * CON_WIDTH;
    Vector2 bot_left  = dst + dst.perpendicular().normalisedCopy() * CON_WIDTH;
    Vector2 bot_right = dst - dst.perpendicular().normalisedCopy() * CON_WIDTH;

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
    m_d_obj(dest),
    m_src(src.getX(), src.getY()),
    m_dest(dest.getX(), dest.getY())
{
    m_node = m_scene->getRootSceneNode()->createChildSceneNode();
    m_line = new ConnectionObject(m_node->getName(), ColourValue(1,1,1,0.5),
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
}

void Connection::handleMoveObject(TableObject& obj)
{
    if (obj.getID() == m_s_obj.getID()) {
	m_src.x = obj.getX();
	m_src.y = obj.getY();
    } else if (obj.getID() == m_d_obj.getID()) {
	m_dest.x = obj.getX();
	m_dest.y = obj.getY();
    }

    m_line->update(m_src, m_dest);
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
