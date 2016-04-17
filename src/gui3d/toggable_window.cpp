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

#include "gui3d/toggable_window.hpp"

using namespace CEGUI;

toggable_window::toggable_window ()
    : m_window(NULL)
    , m_active(false)
{
}

toggable_window::~toggable_window ()
{
    if (m_active) {
	Window* root_win = System::getSingleton()
            .getDefaultGUIContext()
            .getRootWindow();
	root_win->removeChild(m_window);
    }
    delete m_window;
}

void toggable_window::build_window ()
{
    m_window = create_window ();
    m_window->subscribeEvent(
	FrameWindow::EventCloseClicked,
	Event::Subscriber(&toggable_window::on_close, this));
}

void toggable_window::set_active (bool active)
{
    if (m_window == NULL)
	build_window ();

    if (active != m_active) {
	m_active = active;
	Window* root_win = System::getSingleton()
            .getDefaultGUIContext()
            .getRootWindow();
        if (m_active)  root_win->addChild(m_window);
	if (!m_active) root_win->removeChild(m_window);
    }
}
