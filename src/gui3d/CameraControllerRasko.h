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

#ifndef CAMERACONTROLLERRASKO_H
#define CAMERACONTROLLERRASKO_H

#include "gui3d/TaskManager.h"
#include "gui3d/CameraController.h"

class CameraControllerRasko;

class CameraMove : public Task {
    CameraControllerRasko* m_ctrl;
    Ogre::Vector3 m_dest;
    Ogre::Vector3 m_speed;
    int m_duration;

public:
    CameraMove(CameraControllerRasko* ctrl, Ogre::Vector3 dest, int duration);

    void update(int ms);
};

class CameraControllerRasko : public CameraController {
    friend class CameraMove;
	
    TaskManager* m_taskmgr;
    CameraMove* m_move;
    bool m_mouseleft;
    bool m_mouseright;
    bool m_mousecenter;
    bool m_moving;
    int m_modifier;
    Ogre::Vector3 m_aimpoint;
    Ogre::Vector3 m_last_tpos;
    Ogre::Radian m_xangle;
    Ogre::Radian m_yangle;
    Ogre::Real m_dist;
    Ogre::Real m_zxdist;
	
    void recalculate();
    bool getTableIntersection(Ogre::Vector3& dest);
public:
    CameraControllerRasko(Ogre::Camera* camera, TaskManager* taskmgr);
	
    virtual bool mouseMoved(const OIS::MouseEvent& e);
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    
    virtual bool keyPressed(const OIS::KeyEvent &e);
    virtual bool keyReleased(const OIS::KeyEvent &e);
};

#endif /* CAMERACONTROLLERRASKO_H */
