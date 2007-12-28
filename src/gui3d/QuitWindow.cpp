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

#include "gui3d/QuitWindow.h"

using namespace CEGUI;
using namespace std;

FrameWindow* QuitWindow::createWindow()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    FrameWindow* window = dynamic_cast<FrameWindow*>(
	wmgr.createWindow("TaharezLook/FrameWindow", "window_menu"));
	
    window->setPosition( UVector2(UDim(0.5, -100), UDim(0.5, -50)) );
    window->setSize    ( UVector2(UDim(0, 200),     UDim(0, 80)) );
    window->setText("Quit Window");
	
    Window *button_quit = wmgr.createWindow("TaharezLook/Button", "button_quit");
    button_quit->setText("Quit");
    button_quit->setPosition( UVector2(UDim(0.10, 0), UDim(0, 40)) );
    button_quit->setSize    ( UVector2(UDim(0.80, 0), UDim(0, 20)) );
    button_quit->setWantsMultiClickEvents(false);
    
    window->addChildWindow(button_quit);
	
    button_quit->subscribeEvent(PushButton::EventClicked, 
				Event::Subscriber(&QuitWindow::onQuit, this));
           
    return window;
}

QuitWindow::QuitWindow()
{
}

QuitWindow::~QuitWindow()
{
}



