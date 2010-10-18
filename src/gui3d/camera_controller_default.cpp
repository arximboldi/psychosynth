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

#include <CEGUI/CEGUI.h>

#include "gui3d/camera_controller_default.hpp"

#define CAMERA_MOVE_DELAY 400

using namespace Ogre;
using namespace std;

camera_move::camera_move (camera_controller_default* ctrl,
			Ogre::Vector3 dest, int duration) :
    m_ctrl(ctrl),
    m_dest(dest),
    m_duration(duration)
{	
    const Vector3& src = ctrl->m_camera->getPosition();
    m_speed.x = (dest.x - src.x)/duration;
    m_speed.y = (dest.y - src.y)/duration;
    m_speed.z = (dest.z - src.z)/duration;
	
    if (m_ctrl->m_move) m_ctrl->m_move->finish();
    m_ctrl->m_move = this;
}

void camera_move::update (int ms)
{
    Ogre::Vector3 pos = m_ctrl->m_camera->getPosition();
	
    m_duration -= ms;
    if (m_duration < 0) {
	ms += m_duration;
	m_ctrl->m_move = NULL;
	finish();
    }
	
    pos.x += m_speed.x * ms;
    pos.y += m_speed.y * ms;
    pos.z += m_speed.z * ms;
    m_ctrl->m_camera->setPosition(pos);
	
    m_ctrl->m_aimpoint.x += m_speed.x * ms;
    m_ctrl->m_aimpoint.y += m_speed.y * ms;
    m_ctrl->m_aimpoint.z += m_speed.z * ms;
}

camera_controller_default::camera_controller_default (Ogre::Camera* camera)
    : camera_controller (camera),
    m_move(NULL),	
    m_mouseleft(false),
    m_mouseright(false),
    m_mousecenter(false),
    m_moving(false),
    m_modifier(0),
    m_aimpoint(0.0,0.0,0.0),
    m_xangle(0.0),
    m_yangle(Math::PI/4),
    m_dist(15.0),
    m_zxdist(m_dist * Math::Cos(m_yangle))
{
    m_camera->setPosition(Ogre::Vector3(0.0,
					m_dist * Math::Sin(m_yangle),
					m_dist * Math::Cos(m_yangle)));
    m_camera->lookAt(Ogre::Vector3(0,0,0));
}

void camera_controller_default::recalculate ()
{
    Vector3 pos = m_camera->getPosition();
	
    m_zxdist = m_dist * Math::Cos(m_yangle);
    pos.x = m_aimpoint.x + m_zxdist * Math::Sin(m_xangle);
    pos.y = m_aimpoint.y + m_dist   * Math::Sin(m_yangle);
    pos.z = m_aimpoint.z + m_zxdist * Math::Cos(m_xangle);
	
    m_camera->setPosition(pos);
    m_camera->lookAt(m_aimpoint);
}

bool camera_controller_default::get_table_intersection (Ogre::Vector3& dest)
{
    CEGUI::Point mousepos = CEGUI::MouseCursor::getSingleton().getPosition();	
    Ray ray =  Ray(m_camera->getCameraToViewportRay(
		       mousepos.d_x/m_camera->getViewport()->getActualWidth(),
		       mousepos.d_y/m_camera->getViewport()->getActualHeight()));
    pair<bool, Ogre::Real>  inter = ray.intersects(Plane(Vector3(0.0,1.0,0.0),
							 Ogre::Real(0.0)));
			
    if (inter.first) {
	dest = ray.getPoint(inter.second);
	return true;
    }

    return false;
}

bool camera_controller_default::mouseMoved (const OIS::MouseEvent& e)
{
    if (m_moving) {
	Vector3 table_pos;
	if (get_table_intersection (table_pos)) {
	    m_camera->setPosition(m_camera->getPosition() + (m_last_tpos - table_pos));
	    m_aimpoint += m_last_tpos - table_pos;
	}
    }
    
    if (m_mouseright) {		
	m_xangle += Degree(e.state.X.rel * 0.5);
	m_yangle += Degree(e.state.Y.rel * 0.5);
		
	if (m_yangle >= Degree(89.0)) m_yangle = Degree(89.0);
	else if (m_yangle < Degree(0)) m_yangle = Degree(0);
		
	m_zxdist = m_dist * cos(m_yangle.valueRadians());
		
	recalculate();
    }
	
    if (m_mousecenter) {
	// Real h = (corners[0] - corners[3]).length();
	Real min_dist = 1;//h/Math::Tan(m_yangle);
	m_dist += 25.0 * Real(e.state.Y.rel) / m_camera->getViewport()->getActualHeight();
       
	if (m_dist < min_dist)
	    m_dist = min_dist;
	
	recalculate();
    }
	
    if (e.state.Z.rel != 0) {
	Real min_dist = 1;//h/Math::Tan(m_yangle);
	m_dist += e.state.Z.rel * 0.01;
	if (m_dist < min_dist)
	    m_dist = min_dist;
	recalculate();
    }
	
    return false;
}

bool camera_controller_default::mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    Vector3 table_pos;
    const Vector3* corners;
    Real min_dist;
	
    switch(id) {
    case OIS::MB_Left:
	m_mouseleft = true;

	if (get_table_intersection (table_pos)) {
	    if (m_modifier) {
		task_manager::self ().attach (new camera_move (this, m_camera->getPosition() +
							       (table_pos - m_aimpoint),
							       CAMERA_MOVE_DELAY)); 
	    } else {
		m_moving = true;
		m_last_tpos = table_pos;
	    }
	}		
	break;
    case OIS::MB_Right:
	m_mouseright = true;
	break;
    case OIS::MB_Middle:
	m_mousecenter = true;
	break;
    case OIS::MB_Button3:
	m_dist += 1.0;
	recalculate();
	break; 
    case OIS::MB_Button4:
	corners = m_camera->getWorldSpaceCorners();
	//h = (corners[1] - corners[2]).length();
	min_dist = 1; //h/Math::Tan(m_yangle);
	m_dist -= 1.0;
	if (m_dist < min_dist)
	    m_dist = min_dist;
	recalculate();
	break;
    default:
	break;
    }
	
    return false;
}

bool camera_controller_default::mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    switch(id) {
    case OIS::MB_Left:
	m_mouseleft = false;
	m_moving = false;
	break;
    case OIS::MB_Right:
	m_mouseright = false;
	break;
    case OIS::MB_Middle:
	m_mousecenter = false;
	break;
    case OIS::MB_Button3: 
	break; 
    case OIS::MB_Button4:
	break;
    default: break;
    }
	
    return false;
}

bool camera_controller_default::keyPressed (const OIS::KeyEvent &e)
{
    if (e.key == OIS::KC_LSHIFT ||
	e.key == OIS::KC_RSHIFT)
	//m_modifier++;
	m_modifier = 1;
    
    return false;
}

bool camera_controller_default::keyReleased (const OIS::KeyEvent &e)
{
    if (e.key == OIS::KC_LSHIFT ||
	e.key == OIS::KC_RSHIFT)
	//m_modifier--;
	m_modifier = 0;
    
    return false;
}

