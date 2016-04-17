/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007, 2016 Juan Pedro Bolivar Puente                    *
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

#include "gui3d/window_list.hpp"

using namespace CEGUI;

window_list::button::button (const std::string& imageset,
			     const std::string& layout,
			     const std::string& tooltip,
			     toggable_window* window, OIS::KeyCode key, int i) :
    m_toggable(window),
    m_key(key)
{
    ImageManager::getSingleton().loadImageset(imageset);

    m_window = WindowManager::getSingleton().loadLayoutFromFile(layout);

    m_window->setPosition(UVector2(UDim(0.0, WL_GAP*(i+1) + WL_BUTTON_SIZE*i),
    				   UDim(1.0, -WL_BUTTON_SIZE-WL_GAP)));
    m_window->setWantsMultiClickEvents(false);
    m_window->subscribeEvent(
	PushButton::EventClicked,
	Event::Subscriber(&window_list::button::on_click, this));
    m_window->subscribeEvent(
	PushButton::EventMouseEntersArea,
	Event::Subscriber(&window_list::button::on_enter, this));
    m_window->subscribeEvent(
	PushButton::EventMouseLeavesArea,
	Event::Subscriber(&window_list::button::on_leave, this));

    m_tooltip = WindowManager::getSingleton().createWindow("TaharezLook/StaticText");
    m_tooltip->setPosition(UVector2(UDim(0.0, 6.0), UDim(1.0, -WL_BUTTON_SIZE - WL_GAP - 20)));
    m_tooltip->setSize(USize(UDim(1.0, 0), UDim(0.0, 16)));
    m_tooltip->setText(tooltip);
    m_tooltip->setVisible(false);

    Window* root_win = System::getSingleton()
        .getDefaultGUIContext()
        .getRootWindow();
    root_win->addChild(m_window);
    root_win->addChild(m_tooltip);

    m_toggable->set_active (false);
}

bool window_list::button::on_click (const CEGUI::EventArgs &e)
{
    m_toggable->toggle();
    return true;
}

bool window_list::button::on_enter (const CEGUI::EventArgs &e)
{
    m_tooltip->setVisible(true);
    return true;
}

bool window_list::button::on_leave (const CEGUI::EventArgs &e)
{
    m_tooltip->setVisible(false);
    return true;
}

void window_list::add_window (std::string but_imageset, std::string but_layout,
			      std::string tooltip,
			      toggable_window* window, OIS::KeyCode key)
{
    m_lwind.push_back(new button(but_imageset, but_layout, tooltip,
				 window, key, m_nwind++));
}

bool window_list::keyPressed( const OIS::KeyEvent &arg )
{
    std::list<button*>::iterator i;
    for (i = m_lwind.begin(); i != m_lwind.end(); i++) {
	if ((*i)->m_key == arg.key) {
	    (*i)->m_toggable->toggle();
	    return true;
	}
    }
    return false;
};

window_list::~window_list()
{
    std::list<button*>::iterator i;
    for (i = m_lwind.begin(); i != m_lwind.end(); i++)
	delete *i;
}
