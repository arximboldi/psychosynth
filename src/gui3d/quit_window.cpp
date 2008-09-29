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

#include <OGRE/Ogre.h>

#include "gui3d/quit_window.hpp"

using namespace CEGUI;
using namespace std;

FrameWindow* quit_window::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    FrameWindow* window = dynamic_cast<FrameWindow*>(
	wmgr.createWindow("TaharezLook/FrameWindow", "window_menu"));
	
    window->setPosition( UVector2(UDim(0.5, -100), UDim(0.5, -50)) );
    window->setSize    ( UVector2(UDim(0, 240),     UDim(0, 100)) );
    window->setText("Quit Window");

    Window* label = wmgr.createWindow("TaharezLook/StaticText");
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0.1, 0), UDim(0, 35)));
    label->setSize(UVector2(UDim(0.8, 0), UDim(0, 20)));
    label->setText("Do you really want to quit?");
    label->setProperty("HorzFormatting", "WordWrapLeftAligned");
    
    Window *button_quit = wmgr.createWindow("TaharezLook/Button");
    button_quit->setText("Yes...");
    button_quit->setPosition( UVector2(UDim(0.10, 0), UDim(0, 65)));
    button_quit->setSize    ( UVector2(UDim(0.35, 0), UDim(0, 20)));
    button_quit->setWantsMultiClickEvents(false);

    Window *button_cancel = wmgr.createWindow("TaharezLook/Button");
    button_cancel->setText("No!");
    button_cancel->setPosition(UVector2(UDim(0.55, 0), UDim(0, 65)));
    button_cancel->setSize    (UVector2(UDim(0.35, 0), UDim(0, 20)));
    button_cancel->setWantsMultiClickEvents(false);

    window->addChildWindow(label);
    window->addChildWindow(button_quit);
    window->addChildWindow(button_cancel);
	
    button_quit->subscribeEvent(
	PushButton::EventClicked, 
	Event::Subscriber(&quit_window::on_quit, this));
    button_cancel->subscribeEvent(
	PushButton::EventClicked, 
	Event::Subscriber(&quit_window::on_cancel, this));
    return window;
}

bool quit_window::on_quit (const CEGUI::EventArgs &e)
{
    Ogre::Root::getSingleton().queueEndRendering();
    return true;
}

bool quit_window::on_cancel (const CEGUI::EventArgs &e)
{
    toggle();
    return false;
}

quit_window::quit_window ()
{
}

quit_window::~quit_window ()
{
}



