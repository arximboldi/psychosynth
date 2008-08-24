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

#include <libpsynth/world/world.h>

#include "gui3d/OgreMisc.h"

class ConnectionLine : public Ogre::ManualObject
{
    Ogre::ColourValue m_colour;
    
    void build(const Ogre::Vector2& src,
	       const Ogre::Vector2& dest);
public:
    ConnectionLine(const std::string &id,
		     const Ogre::ColourValue& colour,
		     const Ogre::Vector2& src,
		     const Ogre::Vector2& dest);
    
    void update(const Ogre::Vector2& src,
		const Ogre::Vector2& dest);

    void setColour(Ogre::ColourValue colour) {
	m_colour = colour;
	createColourMaterial(getName(), m_colour);
    };
    
    ~ConnectionLine();
};

class ConnectionWave : public Ogre::ManualObject
{
    Ogre::ColourValue m_colour;
    
    void build(const Ogre::Vector2& src,
	       const Ogre::Vector2& dest,
	       const psynth::sample* samples, int n_samples);
public:
    ConnectionWave(const std::string &id,
		     const Ogre::ColourValue& colour,
		     const Ogre::Vector2& src,
		     const Ogre::Vector2& dest);
    
    void update(const Ogre::Vector2& src,
		const Ogre::Vector2& dest,
		const psynth::sample* samples, int n_samples);

    void setColour(Ogre::ColourValue colour) {
	m_colour = colour;
	createColourMaterial(getName(), m_colour);
    };
    
    ~ConnectionWave();
};

class Connection : public psynth::world_node_listener
{
    Ogre::SceneManager* m_scene;
    Ogre::SceneNode* m_node;

    psynth::watch* m_watch;
    ConnectionLine* m_line;
    ConnectionWave* m_wave;
    
    psynth::world_patcher_event m_link;

    Ogre::Vector2 m_src;
    Ogre::Vector2 m_dest;

    bool m_is_muted;
    
public:

    Connection (Ogre::SceneManager* scene, const psynth::world_patcher_event& ev);

    ~Connection();

    bool pointerClicked(const Ogre::Vector2& pos, OIS::MouseButtonID id);

/*
    bool pointerReleased(const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointerMoved(const Ogre::Vector2& pos);
*/
    
    void handle_activate_node (psynth::world_node& obj) {};
    void handle_deactivate_node (psynth::world_node& obj) {};
    void handle_set_param_node (psynth::world_node& ob,
			      int param_id);

    void update();
    
    const psynth::world_node& getSource() {
	return m_link.src;
    }

    const psynth::world_node& getDestiny() {
	return m_link.dest;
    }
};

#endif /* CONNECTION_H */
