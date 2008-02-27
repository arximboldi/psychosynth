/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#include "gui3d/ConfWindow.h"

using namespace psynth;
using namespace CEGUI;

#define CW_WIDTH 100
#define CW_HEIGHT 100

CEGUI::Window* ConfWindow::createAudioSettingsWindow()
{
}

CEGUI::Window* ConfWindow::createVideoSettingsWindow()
{
}

CEGUI::FrameWindow* ConfWindow::createWindow()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow"));
	
    window->setPosition(UVector2(UDim(0.5, -CW_WIDTH/2), UDim(0.5, -CW_HEIGHT/2)));
    window->setSize    (UVector2(UDim(0, CW_WIDTH),UDim(0, CW_HEIGHT)));
    window->setText("Settings");
    
    Window* container = wmgr.createWindow("TaharezLook/TabControl");
    container->setPosition( UVector2(UDim(0, 10), UDim(0, 30)) );
    container->setSize    ( UVector2(UDim(1, -20),     UDim(1, -40)) );

    window->addChildWindow(container);
    //container->addChildWindow(client_win);
    //container->addChildWindow(server_win);

    return window;
}
