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

#include "gui3d/ToggableWindow.h"

using namespace CEGUI;

ToggableWindow::ToggableWindow() :
    m_window(NULL),
    m_active(false)
{
}

ToggableWindow::~ToggableWindow()
{
    if (m_active) {
	Window* root_win = System::getSingleton().getGUISheet();
	root_win->removeChildWindow(m_window);
    }
    delete m_window;
}

void ToggableWindow::buildWindow()
{
    m_window = createWindow();
    m_window->subscribeEvent(FrameWindow::EventCloseClicked, 
			     Event::Subscriber(&ToggableWindow::onClose, this));
}

void ToggableWindow::setActive(bool active)
{
    if (m_window == NULL)
	buildWindow();
		
    if (active != m_active) {
	m_active = active;
	Window* root_win = System::getSingleton().getGUISheet();
	if (m_active)  root_win->addChildWindow(m_window);
	if (!m_active) root_win->removeChildWindow(m_window);
    }
}



