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

#ifndef PSYCHOSYNTH3D_H
#define PSYCHOSYNTH3D_H

#include <OGRE/Ogre.h>
#include <CEGUI/CEGUI.h>
#include <OgreCEGUIRenderer.h>

#include <libpsynth/version.h>
#include <libpsynth/common/timer.h>
#include <libpsynth/psynth/psynth_app.h>
#ifdef PSYNTH_HAVE_OSC
#include <libpsynth/net/osc_client.h>
#include <libpsynth/net/osc_server.h>
#endif

#include "gui3d/default_selector_populator.h"
#include "gui3d/input_manager.h"
#include "gui3d/cegui_injecter.h"
#include "gui3d/task_manager.h"
#include "gui3d/element_manager.h"
#include "gui3d/window_list.h"
#include "gui3d/camera_controller_default.h"

class psychosynth_3d : public psynth::psynth_app,
		       public Ogre::FrameListener
{
    Ogre::Root*          m_ogre;
    Ogre::RenderSystem*   m_render;
    Ogre::RenderWindow*  m_window;
    Ogre::SceneManager*  m_scene;
    Ogre::Camera*        m_camera;
    Ogre::Viewport*      m_viewport;
	
    CEGUI::System*            m_gui;
    CEGUI::OgreCEGUIRenderer* m_ceguirender;

    default_selector_populator m_populator;
    input_manager*      m_inputmgr;
    cegui_injecter*     m_guiinput;
    task_manager*       m_taskmgr;
    element_manager*    m_elemmgr;
    window_list*        m_windowlist;
    camera_controller*  m_camctrl;
    psynth::timer m_timer;

#ifdef PSYNTH_HAVE_OSC
    psynth::osc_client* m_oscclient;
    psynth::osc_server* m_oscserver;
#endif
    
    bool must_quit;

    void setup_settings (psynth::conf_node& conf);
    void setup_ogre (psynth::conf_node& conf);
    void setup_input ();
    void setup_gui ();
#ifdef PSYNTH_HAVE_OSC
    void setup_net ();
#endif
    void setup_menus ();
    void setup_world ();

    void close_world ();
    void close_menus ();
#ifdef PSYNTH_HAVE_OSC
    void close_net ();
#endif
    void close_gui ();
    void close_input ();
    void close_ogre ();

    void print_help ();
    void print_version ();

    bool on_config_change (psynth::conf_node& conf);
    bool on_fps_change (psynth::conf_node& conf);

public:
    psychosynth_3d ();
    ~psychosynth_3d ();

    void prepare (psynth::arg_parser& args);
    int execute ();
       	
    bool frameStarted(const Ogre::FrameEvent& evt);
};

#endif /* PSYCHOSYNTH3D_H */

