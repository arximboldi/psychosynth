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

#include <psynth/version.hpp>
#include <psynth/base/config.hpp>
#include "gui3d/toggable_window.hpp"

class output_conf_window
{
public:
    virtual CEGUI::Window* create_window (psynth::conf_node& node) = 0;
    virtual void apply() = 0;
};

class output_conf_window_simple : public output_conf_window
{
    std::string m_conf_path;
    std::string m_name;
    CEGUI::Editbox* m_value;
    psynth::conf_node* m_node;
    
public:
    output_conf_window_simple (const std::string& cp,
			      const std::string& name) :
	m_conf_path(cp), m_name(name) {}

    CEGUI::Window* create_window(psynth::conf_node& node);

    void apply () {
	m_node->get_path(m_conf_path).set<std::string>(m_value->getText().c_str ());
    };
};

class output_conf_window_factory {
public:
    virtual const char* get_name () = 0;
    virtual output_conf_window* create_output_conf_window() = 0;
};

#ifdef PSYNTH_HAVE_ALSA
class output_conf_window_factory_alsa : public output_conf_window_factory
{
public:
    const char* get_name () {
	return "alsa";
    }
    
    output_conf_window* create_output_conf_window () {
	return new output_conf_window_simple ("out_device", "Device:");
    }
};
#endif /* PSYNTH_HAVE_ALSA */

#ifdef PSYNTH_HAVE_OSS
class output_conf_window_factory_oss : public output_conf_window_factory
{
public:
    const char* get_name () {
	return "oss";
    }
    
    output_conf_window* create_output_conf_window () {
	return new output_conf_window_simple ("out_device", "Device:");
    }
};
#endif /* PSYNTH_HAVE_OSS */

#ifdef PSYNTH_HAVE_JACK
class output_conf_window_factory_jack : public output_conf_window_factory
{
public:
    const char* get_name () {
	return "jack";
    }
    
    output_conf_window* create_output_conf_window () {
	return new output_conf_window_simple ("server", "Name:");
    }
};
#endif /* PSYNTH_HAVE_JACK */

class path_conf_window : public toggable_window
{
    CEGUI::ItemListbox* m_listbox;
    CEGUI::Editbox* m_editbox;
    
    psynth::conf_node& m_conf;
    
    bool on_generic (const CEGUI::EventArgs &e);
    bool on_delete (const CEGUI::EventArgs &e);
    bool on_add (const CEGUI::EventArgs &e);
    bool on_change (const CEGUI::EventArgs &e);
    bool on_apply (const CEGUI::EventArgs &e);
    bool on_selection (const CEGUI::EventArgs &e);
    void populate ();
    CEGUI::FrameWindow* create_window ();
    
public:
    path_conf_window (psynth::conf_node& conf_node) :
	m_conf (conf_node)
	{}  
};

class conf_window : public toggable_window
{
    /* File manager config. */
    path_conf_window m_samples_win;
    
    /* The output thing is boring.*/
    typedef std::map<std::string, output_conf_window_factory*> ocwf_map;
    ocwf_map m_map;
    output_conf_window* m_out_win;
    CEGUI::Window* m_out_cegui_win;
    
    psynth::conf_node& m_gui_conf;
    psynth::conf_node& m_psynth_conf;
    
    
    /* Video options */
    CEGUI::Editbox* m_sc_width;
    CEGUI::Editbox* m_sc_height;
    CEGUI::Checkbox* m_fullscreen;
    CEGUI::Scrollbar* m_fps;
    CEGUI::Window* m_fpslabel;
    
    /* Audio options */
    CEGUI::Editbox* m_srate;
    CEGUI::Editbox* m_bufsize;
    CEGUI::Editbox* m_channels;
    CEGUI::Combobox* m_output;

    void start_output_conf_window (const std::string& out);
    void stop_output_conf_window ();
    CEGUI::Window* create_audio_settings_window ();
    CEGUI::Window* create_video_settings_window ();
    CEGUI::Window* create_paths_settings_window ();

    bool on_generic (const CEGUI::EventArgs &e);
    bool on_audio_apply_press (const CEGUI::EventArgs &e);
    bool on_output_change (const CEGUI::EventArgs &e);
    bool on_fps_change (const CEGUI::EventArgs &e);
    bool on_window_apply_press (const CEGUI::EventArgs &e);
    bool on_samples_paths_press (const CEGUI::EventArgs &e);
    
    void attach_output_conf_window_factory (output_conf_window_factory* f) {
	m_map.insert(std::make_pair(std::string(f->get_name()), f));
    }
    
public:
    conf_window (psynth::conf_node& gui_conf,
		 psynth::conf_node& psynth_conf);

    ~conf_window ();
    
    CEGUI::FrameWindow* create_window ();
};

#endif /* CONFWINDOW_H */

