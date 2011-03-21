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

#ifndef RECORDWINDOW_H
#define RECORDWINDOW_H

#include <psynth/world/world.hpp>
#include <psynth/io/file_output.hpp>
#include <psynth/graph/node_output.hpp>

#include "gui3d/toggable_window.hpp"

class record_window : public toggable_window
{
    psynth::world* m_table;
    psynth::graph::audio_output_ptr m_output;

    CEGUI::Window* m_button;
    CEGUI::Editbox* m_file;
    
    bool m_recording;
    
    virtual CEGUI::FrameWindow* create_window ();

public:
    record_window (psynth::world* table) :
	m_table (table),
	m_recording (false)
	{};

    ~record_window () {
	if (m_recording)
	    m_table->detach_passive_output (m_output);
    }

    bool on_click (const CEGUI::EventArgs &e);
};

#endif /* RECORDWINDOW_H */
