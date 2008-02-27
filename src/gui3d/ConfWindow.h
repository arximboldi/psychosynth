/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef CONFWINDOW_H
#define CONFWINDOW_H

#include <libpsynth/common/Config.h>
#include "gui3d/ToggableWindow.h"

class ConfWindow : public ToggableWindow
{
    psynth::ConfNode& m_gui_conf;
    psynth::ConfNode& m_psynth_conf;

    CEGUI::Window* createAudioSettingsWindow();
    CEGUI::Window* createVideoSettingsWindow();
public:
    ConfWindow(psynth::ConfNode& gui_conf,
	       psynth::ConfNode& psynth_conf) :
	m_gui_conf(gui_conf),
	m_psynth_conf(psynth_conf) {}

    CEGUI::FrameWindow* createWindow();
};

#endif /* CONFWINDOW_H */
