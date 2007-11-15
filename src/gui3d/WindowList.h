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

#ifndef WINDOWLIST_H
#define WINDOWLIST_H

#include <list>
#include <iostream>
#include <CEGUI/CEGUI.h>
#include <OIS/OIS.h>

#include "gui3d/ToggableWindow.h"

const int WL_BUTTON_SIZE = 32;
const int WL_GAP = 4;

class WindowList : public OIS::KeyListener
{
    class Button {
	friend class WindowList;
		
	ToggableWindow *m_toggable;
	CEGUI::Window *m_window;
	OIS::KeyCode m_key;
    public:
	Button(std::string imageset, std::string layout, ToggableWindow* window,
	       OIS::KeyCode key, int i);
		
	~Button() {
	    delete m_toggable;
	}
		
	bool onClick(const CEGUI::EventArgs &e) {
	    m_toggable->toggle();
	    return true;
	};
    };
	
    std::list<Button*> m_lwind;
    int m_nwind;

public:
    WindowList() : m_nwind(0) {}
	
    ~WindowList() {
	std::list<Button*>::iterator i;
	for (i = m_lwind.begin(); i != m_lwind.end(); i++)
	    delete *i;
    }
	
    void addWindow(std::string but_imageset, std::string but_layout,
		   ToggableWindow* window, OIS::KeyCode key) {
	m_lwind.push_back( new Button(but_imageset, but_layout, window, key, m_nwind++) );
    }
	
    bool keyPressed( const OIS::KeyEvent &arg ) {
	std::list<Button*>::iterator i;
	for (i = m_lwind.begin(); i != m_lwind.end(); i++) {
	    if ((*i)->m_key == arg.key) {
		(*i)->m_toggable->toggle();
		return true;
	    }
	}
	return false;
    };
	
    bool keyReleased( const OIS::KeyEvent &arg ) {
	return false;
    };	
};

#endif /* WINDOWLIST_H */
