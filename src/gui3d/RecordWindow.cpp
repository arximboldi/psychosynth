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

#include <string>

#include "gui3d/RecordWindow.h"

using namespace std;
using namespace CEGUI;

#define RW_WIDTH  220
#define RW_HEIGHT 90

#define DEFAULT_FILE "record.au"

CEGUI::FrameWindow* RecordWindow::createWindow()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow"));
    
    window->setPosition(UVector2(UDim(0, 40), UDim(1, -RW_HEIGHT-100)));
    window->setSize    (UVector2(UDim(0, RW_WIDTH),UDim(0, RW_HEIGHT)));
    window->setText("Recorder");

    Window* file_label = wmgr.createWindow("TaharezLook/StaticText");
    file_label->setText("File");
    file_label->setPosition(UVector2(UDim(0, 20), UDim(0, 30)));
    file_label->setSize    (UVector2(UDim(0.2, 0), UDim(0, 20)));

    m_file = dynamic_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    m_file->setPosition(UVector2(UDim(0.2, 20), UDim(0, 30)));
    m_file->setSize    (UVector2(UDim(0.8, -40), UDim(0, 20)));
    m_file->setText(DEFAULT_FILE);
    
    m_button = wmgr.createWindow("TaharezLook/Button");
    m_button->setText("Start");
    m_button->setPosition(UVector2(UDim(0, 20), UDim(0, 60)));
    m_button->setSize    (UVector2(UDim(1, -40), UDim(0, 20)));
    m_button->setWantsMultiClickEvents(false);
    
    window->addChildWindow(m_button);
    window->addChildWindow(m_file);
    window->addChildWindow(file_label);

    m_button->subscribeEvent(PushButton::EventClicked, 
			     Event::Subscriber(&RecordWindow::onClick, this));
    
    return window;
}

bool RecordWindow::onClick(const CEGUI::EventArgs &e)
{
    if (!m_recording) {
	std::string fname = m_file->getText().c_str();
	m_output.setFileName(fname);
	if (m_output.open()) {
	    m_table->attachPassiveOutput(&m_output);
	    m_button->setText("Stop");
	    m_file->disable();
	    m_recording = true;
	}
    } else {
	m_table->detachPassiveOutput(&m_output);
	m_output.close();
	m_button->setText("Start");
	m_file->enable();
	m_recording = false;
    }
    
    return true;
}



