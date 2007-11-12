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

#include "gui3d/InputManager.h"

InputManager::InputManager(size_t window_hnd)
{
	m_inputmgr = OIS::InputManager::createInputSystem(window_hnd);
	
	m_keyboard = static_cast<OIS::Keyboard*>(m_inputmgr->createInputObject(OIS::OISKeyboard, true));
	m_mouse = static_cast<OIS::Mouse*>(m_inputmgr->createInputObject(OIS::OISMouse, true));
	
	m_keyboard->setEventCallback(this);
	m_mouse->setEventCallback(this);
}

InputManager::~InputManager()
{
	delete m_keyboard;
	delete m_mouse;
	//delete m_inputmgr; /* TODO */
}

bool InputManager::mouseMoved(const OIS::MouseEvent& e)
{
	bool finished = false;
	for (MouseIter i = m_mlist.begin(); i != m_mlist.end() && !finished; i++)
		finished = (*i)->mouseMoved(e);
	return true;
}

bool InputManager::mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	bool finished = false;
	for (MouseIter i = m_mlist.begin(); i != m_mlist.end() && !finished; i++)
		finished = (*i)->mousePressed(e, id);
	return true;
}

bool InputManager::mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id)
{
	bool finished = false;
	for (MouseIter i = m_mlist.begin(); i != m_mlist.end() && !finished; i++)
		finished = (*i)->mouseReleased(e, id);
	return true;
}

bool InputManager::keyPressed(const OIS::KeyEvent &e)
{
	bool finished = false;
	for (KeyIter i = m_klist.begin(); i != m_klist.end() && !finished; i++)
		finished = (*i)->keyPressed(e);
	return true;
}

bool InputManager::keyReleased(const OIS::KeyEvent &e)
{
	bool finished = false;
	for (KeyIter i = m_klist.begin(); i != m_klist.end() && !finished; i++)
		finished = (*i)->keyReleased(e);
	return true;
}
