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

#include "gui3d/task_manager.hpp"
#include "gui3d/camera_controller.hpp"

class camera_controller_default;

class camera_move : public task
{
    camera_controller_default* m_ctrl;
    Ogre::Vector3 m_dest;
    Ogre::Vector3 m_speed;
    int m_duration;

public:
    camera_move (camera_controller_default* ctrl, Ogre::Vector3 dest, int duration);

    void update (int ms);
};

class camera_controller_default : public camera_controller
{
    friend class camera_move;

    camera_move* m_move;
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

    void recalculate ();
    bool get_table_intersection (Ogre::Vector3& dest);

public:
    camera_controller_default (Ogre::Camera* camera);

    virtual bool mouseMoved(const OIS::MouseEvent& e);
    virtual bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    virtual bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);

    virtual bool keyPressed(const OIS::KeyEvent &e);
    virtual bool keyReleased(const OIS::KeyEvent &e);
};

#endif /* CAMERACONTROLLERRASKO_H */
