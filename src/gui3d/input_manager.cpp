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

#include "gui3d/input_manager.hpp"

input_manager::input_manager(OIS::ParamList& pl)
{
    m_inputmgr = OIS::InputManager::createInputSystem(pl);
	
    m_keyboard = static_cast<OIS::Keyboard*>(
	m_inputmgr->createInputObject(OIS::OISKeyboard, true));
    m_mouse = static_cast<OIS::Mouse*>(
	m_inputmgr->createInputObject(OIS::OISMouse, true));
	
    m_keyboard->setEventCallback(this);
    m_mouse->setEventCallback(this);   
}

input_manager::input_manager(size_t window_hnd)
{
    m_inputmgr = OIS::InputManager::createInputSystem(window_hnd);
	
    m_keyboard = static_cast<OIS::Keyboard*>(
	m_inputmgr->createInputObject(OIS::OISKeyboard, true));
    m_mouse = static_cast<OIS::Mouse*>(
	m_inputmgr->createInputObject(OIS::OISMouse, true));
	
    m_keyboard->setEventCallback (this);
    m_mouse->setEventCallback (this);
}

input_manager::~input_manager()
{
    delete m_keyboard;
    delete m_mouse;
    //delete m_inputmgr; /* TODO */
}

bool input_manager::mouseMoved (const OIS::MouseEvent& e)
{
    bool finished = false;
    for (mouse_reverse_iterator i = m_mlist.rbegin(); i != m_mlist.rend() && !finished; i++)
	finished = (*i)->mouseMoved(e);
    return true;
}

bool input_manager::mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    bool finished = false;
    for (mouse_reverse_iterator i = m_mlist.rbegin(); i != m_mlist.rend() && !finished; i++)
	finished = (*i)->mousePressed(e, id);
    return true;
}

bool input_manager::mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
    bool finished = false;
    for (mouse_reverse_iterator i = m_mlist.rbegin(); i != m_mlist.rend() && !finished; i++)
	finished = (*i)->mouseReleased(e, id);
    return true;
}

bool input_manager::keyPressed (const OIS::KeyEvent &e)
{
    bool finished = false;
    for (key_reverse_iterator i = m_klist.rbegin(); i != m_klist.rend() && !finished; i++)
	finished = (*i)->keyPressed(e);
    return true;
}

bool input_manager::keyReleased (const OIS::KeyEvent &e)
{
    bool finished = false;
    for (key_reverse_iterator i = m_klist.rbegin(); i != m_klist.rend() && !finished; i++)
	finished = (*i)->keyReleased(e);
    return true;
}
