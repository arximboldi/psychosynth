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

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include <OIS/OIS.h>
#include <OGRE/Ogre.h>


class camera_controller : public OIS::MouseListener,
			  public OIS::KeyListener
{
protected:	
    Ogre::Camera* m_camera;

public:
    camera_controller (Ogre::Camera* camera = NULL)
	: m_camera(camera) {}
	
    virtual ~camera_controller() {};

    const Ogre::Camera* get_camera () const {
	return m_camera;
    }
    
    Ogre::Camera* get_camera () {
	return m_camera;
    }

    void set_camera (Ogre::Camera* camera) {
	m_camera = camera;
    }

    /* OIS */
    virtual bool mouseMoved(const OIS::MouseEvent& e) {return false;};
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id) {return false;};
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id) {return false;};
    virtual bool keyPressed(const OIS::KeyEvent &e) {return false;};
    virtual bool keyReleased(const OIS::KeyEvent &e) {return false;};
};

#endif
