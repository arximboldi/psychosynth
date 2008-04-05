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

#include <libpsynth/version.h>
#include <libpsynth/common/Config.h>
#include "gui3d/ToggableWindow.h"

class OutputConfWindow
{
public:
    virtual CEGUI::Window* createWindow(psynth::ConfNode& node) = 0;
    virtual void apply() = 0;
};

class OutputConfWindowSimple : public OutputConfWindow
{
    std::string m_conf_path;
    std::string m_name;
    CEGUI::Editbox* m_value;
    psynth::ConfNode* m_node;
    
public:
    OutputConfWindowSimple(const std::string& cp,
			   const std::string& name) :
	m_conf_path(cp), m_name(name) {}

    CEGUI::Window* createWindow(psynth::ConfNode& node);

    void apply() {
	m_node->getPath(m_conf_path).set(m_value->getText().c_str());
    };
};

class OutputConfWindowFactory {
public:
    virtual const char* getName() = 0;
    virtual OutputConfWindow* createOutputConfWindow() = 0;
};

#ifdef PSYNTH_HAVE_ALSA
class OutputConfWindowFactoryAlsa : public OutputConfWindowFactory
{
public:
    const char* getName() {
	return "alsa";
    }
    
    OutputConfWindow* createOutputConfWindow() {
	return new OutputConfWindowSimple("out_device", "Device:");
    }
};
#endif /* PSYNTH_HAVE_ALSA */

#ifdef PSYNTH_HAVE_OSS
class OutputConfWindowFactoryOss : public OutputConfWindowFactory
{
public:
    const char* getName() {
	return "oss";
    }
    
    OutputConfWindow* createOutputConfWindow() {
	return new OutputConfWindowSimple("out_device", "Device:");
    }
};
#endif /* PSYNTH_HAVE_OSS */

#ifdef PSYNTH_HAVE_JACK
class OutputConfWindowFactoryJack : public OutputConfWindowFactory
{
public:
    const char* getName() {
	return "jack";
    }
    
    OutputConfWindow* createOutputConfWindow() {
	return new OutputConfWindowSimple("server", "Name:");
    }
};
#endif /* PSYNTH_HAVE_JACK */

class PathConfWindow : public ToggableWindow
{
    CEGUI::ItemListbox* m_listbox;
    CEGUI::Editbox* m_editbox;
    
    psynth::ConfNode& m_conf;
    
    bool onGeneric(const CEGUI::EventArgs &e);
    bool onDelete(const CEGUI::EventArgs &e);
    bool onAdd(const CEGUI::EventArgs &e);
    bool onChange(const CEGUI::EventArgs &e);
    bool onApply(const CEGUI::EventArgs &e);
    bool onSelection(const CEGUI::EventArgs &e);
    void populate();
    CEGUI::FrameWindow* createWindow();
    
public:
    PathConfWindow(psynth::ConfNode& conf_node) :
	m_conf(conf_node)
	{}  
};

class ConfWindow : public ToggableWindow
{
    /* File manager config. */
    PathConfWindow m_samples_win;
    
    /* The output thing is boring.*/
    typedef std::map<std::string, OutputConfWindowFactory*> OCWFMap;
    OCWFMap m_map;
    OutputConfWindow* m_out_win;
    CEGUI::Window* m_out_cegui_win;
    
    psynth::ConfNode& m_gui_conf;
    psynth::ConfNode& m_psynth_conf;
    
    
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

    void startOutputConfWindow(const std::string& out);
    void stopOutputConfWindow();
    CEGUI::Window* createAudioSettingsWindow();
    CEGUI::Window* createVideoSettingsWindow();
    CEGUI::Window* createPathsSettingsWindow();

    bool onGeneric(const CEGUI::EventArgs &e);
    bool onAudioApplyPress(const CEGUI::EventArgs &e);
    bool onOutputChange(const CEGUI::EventArgs &e);
    bool onFpsChange(const CEGUI::EventArgs &e);
    bool onWindowApplyPress(const CEGUI::EventArgs &e);
    bool onSamplesPathsPress(const CEGUI::EventArgs &e);
    
    void attachOutputConfWindowFactory(OutputConfWindowFactory* f) {
	m_map.insert(std::make_pair(std::string(f->getName()), f));
    }
    
public:
    ConfWindow(psynth::ConfNode& gui_conf,
	       psynth::ConfNode& psynth_conf);

    ~ConfWindow();
    
    CEGUI::FrameWindow* createWindow();
};

#endif /* CONFWINDOW_H */

