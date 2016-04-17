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

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <list>
#include <OIS/OIS.h>

class input_manager: public OIS::MouseListener, public OIS::KeyListener
{
    typedef std::list<OIS::MouseListener*>::reverse_iterator mouse_reverse_iterator;
    typedef std::list<OIS::KeyListener*>::reverse_iterator key_reverse_iterator;

    std::list<OIS::MouseListener*> m_mlist;
    std::list<OIS::KeyListener*> m_klist;

    OIS::InputManager* m_inputmgr;
    OIS::Keyboard* m_keyboard;
    OIS::Mouse* m_mouse;

    int m_width;
    int m_height;

public:
    input_manager (OIS::ParamList& pl);
    input_manager (size_t window_handle);
    ~input_manager ();

    bool add_mouse_listener (OIS::MouseListener* listener) {
	m_mlist.push_back (listener);
	return true; /* TODO */
    }

    bool add_key_listener (OIS::KeyListener* listener) {
	m_klist.push_back (listener);
	return true;
    }

    bool remove_mouse_listener (OIS::MouseListener* listener) {
	m_mlist.remove (listener);
	return true;
    }

    bool remove_keyboard_listener (OIS::KeyListener* listener) {
	m_klist.remove(listener);
	return true;
    }

    void capture () {
	m_mouse->capture();
	m_keyboard->capture();
    }

    /* Reimplemented from OIS */
    bool mouseMoved (const OIS::MouseEvent& e);
    bool mousePressed (const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased (const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool keyPressed (const OIS::KeyEvent &e);
    bool keyReleased (const OIS::KeyEvent &e);
};

#endif /* INPUTMANAGER_H */
