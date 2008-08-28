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

#ifndef TOGGABLEWINDOW_H
#define TOGGABLEWINDOW_H

#include <CEGUI/CEGUI.h>

class toggable_window
{
    CEGUI::FrameWindow* m_window;
    bool m_active;
	
    virtual CEGUI::FrameWindow* create_window () = 0;
    void build_window ();

protected:
    CEGUI::FrameWindow* get_window () {
	return m_window;
    }
    
public:
    toggable_window ();
    virtual ~toggable_window ();
	
    void set_active (bool active);

    bool is_active() {
	return m_active;
    }
    
    bool toggle () {
	set_active (!m_active);
	return m_active;
    };
	
    bool on_close (const CEGUI::EventArgs &e) {
	set_active (false);
	return true;
    };
};

#endif /* TOGGABLEWINDOW_H */
