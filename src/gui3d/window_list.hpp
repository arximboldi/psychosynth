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
#include <cstddef> // CEGUI bugfix.
#include <CEGUI/CEGUI.h>
#include <OIS/OIS.h>

#include "gui3d/toggable_window.hpp"

const int WL_BUTTON_SIZE = 32;
const int WL_GAP = 4;

class window_list : public OIS::KeyListener
{
    class button
    {
	friend class window_list;

	toggable_window *m_toggable;
	CEGUI::Window *m_window;
	CEGUI::Window *m_tooltip;
	OIS::KeyCode m_key;
    public:
	button (const std::string& imageset,
		const std::string &layout,
		const std::string& tooltip,
		toggable_window* window,
		OIS::KeyCode key, int i);

	~button () {
	    delete m_toggable;
	}

	bool on_click (const CEGUI::EventArgs &e);
	bool on_leave (const CEGUI::EventArgs &e);
	bool on_enter (const CEGUI::EventArgs &e);
    };

    std::list<button*> m_lwind;
    int m_nwind;

public:
    window_list ()
	: m_nwind(0) {}

    ~window_list ();

    void add_window (std::string but_imageset, std::string but_layout,
		     std::string tooltip,
		     toggable_window* window, OIS::KeyCode key);

    bool keyPressed (const OIS::KeyEvent &arg);
    bool keyReleased (const OIS::KeyEvent &arg) {
	return false;
    };
};

#endif /* WINDOWLIST_H */
