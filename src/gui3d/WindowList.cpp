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

WindowList::Button::Button(const std::string& imageset,
			   const std::string& layout,
			   const std::string& tooltip,
			   ToggableWindow* window, OIS::KeyCode key, int i) :
    m_toggable(window),
    m_key(key)
{
    ImagesetManager::getSingleton().createImageset(imageset);
	 
    m_window = WindowManager::getSingleton().loadWindowLayout(layout);

    m_window->setPosition(UVector2(UDim(0.0, WL_GAP*(i+1) + WL_BUTTON_SIZE*i), 
    				   UDim(1.0, -WL_BUTTON_SIZE-WL_GAP)));
    m_window->setWantsMultiClickEvents(false);
    m_window->subscribeEvent(PushButton::EventClicked, 
			     Event::Subscriber(&WindowList::Button::onClick, this));
    m_window->subscribeEvent(PushButton::EventMouseEnters, 
			     Event::Subscriber(&WindowList::Button::onEnter, this));
    m_window->subscribeEvent(PushButton::EventMouseLeaves, 
			     Event::Subscriber(&WindowList::Button::onLeave, this));
	
    m_tooltip = WindowManager::getSingleton().createWindow("TaharezLook/StaticText");
    m_tooltip->setPosition(UVector2(UDim(0.0, 6.0), UDim(1.0, -WL_BUTTON_SIZE - WL_GAP - 20)));
    m_tooltip->setSize(UVector2(UDim(1.0, 0), UDim(0.0, 16)));
    m_tooltip->setText(tooltip);
    m_tooltip->setVisible(false);
    
    Window* root_win = System::getSingleton().getGUISheet();
    root_win->addChildWindow(m_window);
    root_win->addChildWindow(m_tooltip);

    m_toggable->setActive(false);
}

bool WindowList::Button::onClick(const CEGUI::EventArgs &e)
{
    m_toggable->toggle();
    return true;
}

bool WindowList::Button::onEnter(const CEGUI::EventArgs &e)
{
    m_tooltip->setVisible(true);
    return true;
}

bool WindowList::Button::onLeave(const CEGUI::EventArgs &e)
{
    m_tooltip->setVisible(false);
    return true;
}

void WindowList::addWindow(std::string but_imageset, std::string but_layout,
			   std::string tooltip,
			   ToggableWindow* window, OIS::KeyCode key)
{
    m_lwind.push_back(new Button(but_imageset, but_layout, tooltip,
				 window, key, m_nwind++));
}
	
bool WindowList::keyPressed( const OIS::KeyEvent &arg )
{
    std::list<Button*>::iterator i;
    for (i = m_lwind.begin(); i != m_lwind.end(); i++) {
	if ((*i)->m_key == arg.key) {
	    (*i)->m_toggable->toggle();
	    return true;
	}
    }
    return false;
};

WindowList::~WindowList()
{
    std::list<Button*>::iterator i;
    for (i = m_lwind.begin(); i != m_lwind.end(); i++)
	delete *i;
}
