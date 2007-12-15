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

#ifndef QUITWINDOW_H
#define QUITWINDOW_H

#include <OGRE/Ogre.h>
#include "gui3d/ToggableWindow.h"

class QuitWindow : public ToggableWindow
{	
    CEGUI::FrameWindow* createWindow();

public:
    QuitWindow();
    ~QuitWindow();
	
    bool onQuit(const CEGUI::EventArgs &e) {
	/* TODO */
	Ogre::Root::getSingleton().queueEndRendering();
	return true;
    };
};

#endif /* QUITWINDOW_H */
