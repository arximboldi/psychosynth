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

class ToggableWindow
{
    CEGUI::FrameWindow* m_window;
    bool m_active;
	
    virtual CEGUI::FrameWindow* createWindow() = 0;
    void buildWindow();

protected:
    CEGUI::FrameWindow* getWindow() {
	return m_window;
    }
    
public:
    ToggableWindow();
    virtual ~ToggableWindow();
	
    void setActive(bool active);

    bool isActive() {
	return m_active;
    }
    
    bool toggle() {
	setActive(!m_active);
	return m_active;
    };
	
    bool onClose(const CEGUI::EventArgs &e) {
	setActive(false);
	return true;
    };
};

#endif /* TOGGABLEWINDOW_H */
