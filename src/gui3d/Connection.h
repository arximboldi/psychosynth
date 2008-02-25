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

#ifndef CONNECTION_H
#define CONNECTION_H

#include <OGRE/Ogre.h>
#include <OIS/OIS.h>

#include <libpsynth/table/Table.h>

#include "gui3d/OgreMisc.h"

class ConnectionObject : public Ogre::ManualObject
{
    Ogre::ColourValue m_colour;
    
    void build(const Ogre::Vector2& src,
	       const Ogre::Vector2& dest);
public:
    ConnectionObject(const std::string &id,
		     const Ogre::ColourValue& colour,
		     const Ogre::Vector2& src,
		     const Ogre::Vector2& dest);
    
    void update(const Ogre::Vector2& src,
		const Ogre::Vector2& dest);

    void setColour(Ogre::ColourValue colour) {
	m_colour = colour;
	createColourMaterial(getName(), m_colour);
    };
    
    ~ConnectionObject();
};

class Connection : public psynth::TableObjectListener
{
    Ogre::SceneManager* m_scene;
    Ogre::SceneNode* m_node;
    
    ConnectionObject* m_line;

    psynth::TableObject m_s_obj;
    psynth::TableObject m_d_obj;

    Ogre::Vector2 m_src;
    Ogre::Vector2 m_dest;

    bool m_is_muted;
    
public:

    Connection(Ogre::SceneManager* scene,
	       const psynth::TableObject& src,
	       const psynth::TableObject& dest);

    ~Connection();

    bool pointerClicked(const Ogre::Vector2& pos, OIS::MouseButtonID id);
/*
    bool pointerReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointerMoved(const Ogre::Vector2& pos);
*/
    
    void handleActivateObject(psynth::TableObject& obj) {};
    void handleDeactivateObject(psynth::TableObject& obj) {};
    void handleSetParamObject(psynth::TableObject& ob,
			      psynth::Object::ParamID param_id);

    const psynth::TableObject& getSource() {
	return m_s_obj;
    }

    const psynth::TableObject& getDestiny() {
	return m_d_obj;
    }
};

#endif /* CONNECTION_H */
