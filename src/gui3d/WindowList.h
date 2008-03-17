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
	CEGUI::Window *m_tooltip;
	OIS::KeyCode m_key;
    public:
	Button(const std::string& imageset, const std::string &layout,
	       const std::string& tooltip,
	       ToggableWindow* window,
	       OIS::KeyCode key, int i);
		
	~Button() {
	    delete m_toggable;
	}
		
	bool onClick(const CEGUI::EventArgs &e);
	bool onLeave(const CEGUI::EventArgs &e);
	bool onEnter(const CEGUI::EventArgs &e);
    };
	
    std::list<Button*> m_lwind;
    int m_nwind;

public:
    WindowList() : m_nwind(0) {}
	
    ~WindowList();
	
    void addWindow(std::string but_imageset, std::string but_layout,
		   std::string tooltip,
		   ToggableWindow* window, OIS::KeyCode key);
	
    bool keyPressed(const OIS::KeyEvent &arg);
	
    bool keyReleased(const OIS::KeyEvent &arg) {
	return false;
    };	
};

#endif /* WINDOWLIST_H */
