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

#include "gui3d/WindowList.h"

using namespace CEGUI;

WindowList::Button::Button(std::string imageset, std::string layout,
						   ToggableWindow* window, OIS::KeyCode key, int i) :
	m_toggable(window),
	m_key(key)
{
	ImagesetManager::getSingleton().createImageset(imageset);
	 
	m_window = WindowManager::getSingleton().loadWindowLayout(layout);
	
	m_window->setPosition(UVector2(UDim(0.0, WL_GAP*(i+1) + WL_BUTTON_SIZE*i), 
								   UDim(1.0, -WL_BUTTON_SIZE-WL_GAP)));
	m_window->subscribeEvent(PushButton::EventClicked, 
							 Event::Subscriber(&WindowList::Button::onClick, this));
						 
	Window* root_win = System::getSingleton().getGUISheet();
	root_win->addChildWindow(m_window);
	
	m_toggable->setActive(false);
}
