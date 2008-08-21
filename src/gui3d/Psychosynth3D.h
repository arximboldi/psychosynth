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

#include "gui3d/DefaultSelectorPopulator.h"
#include "gui3d/InputManager.h"
#include "gui3d/CeguiInjecter.h"
#include "gui3d/TaskManager.h"
#include "gui3d/ElementManager.h"
#include "gui3d/WindowList.h"
#include "gui3d/CameraControllerRasko.h"

class Psychosynth3D : public psynth::psynth_app,
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

    DefaultSelectorPopulator m_populator;
    InputManager*     m_inputmgr;
    CeguiInjecter*    m_guiinput;
    TaskManager*      m_taskmgr;
    ElementManager*   m_elemmgr;
    WindowList*       m_windowlist;
    CameraController* m_camctrl;
    psynth::timer m_timer;

#ifdef PSYNTH_HAVE_OSC
    psynth::osc_client* m_oscclient;
    psynth::osc_server* m_oscserver;
#endif
    
    bool must_quit;

    void setupSettings(psynth::conf_node& conf);
    void setupOgre(psynth::conf_node& conf);
    void setupInput();
    void setupGui();
#ifdef PSYNTH_HAVE_OSC
    void setupNet();
#endif
    void setupMenus();
    void setupTable();

    void closeTable();
    void closeMenus();
#ifdef PSYNTH_HAVE_OSC
    void closeNet();
#endif
    void closeGui();
    void closeInput();
    void closeOgre();

    void print_help();
    void print_version();

    bool onConfigChange(psynth::conf_node& conf);
    bool onFpsChange(psynth::conf_node& conf);

public:
    Psychosynth3D();
    ~Psychosynth3D();

    void prepare (psynth::arg_parser& args);
    int execute ();
       	
    bool frameStarted(const Ogre::FrameEvent& evt);
};

#endif /* PSYCHOSYNTH3D_H */

