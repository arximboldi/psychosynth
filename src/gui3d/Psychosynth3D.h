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

#include <libpsynth/common/Timer.h>
#include <libpsynth/psynth/PsychosynthApp.h>
#include <libpsynth/net/OSCClient.h>
#include <libpsynth/net/OSCServer.h>

#include "gui3d/InputManager.h"
#include "gui3d/CeguiInjecter.h"
#include "gui3d/TaskManager.h"
#include "gui3d/ElementManager.h"
#include "gui3d/WindowList.h"
#include "gui3d/CameraControllerRasko.h"

class Psychosynth3D : public psynth::PsychosynthApp,
		      public Ogre::FrameListener
{
    Ogre::Root*          m_ogre;
    Ogre::RenderWindow*  m_window;
    Ogre::SceneManager*  m_scene;
    Ogre::Camera*        m_camera;
    Ogre::Viewport*      m_viewport;
	
    CEGUI::System*            m_gui;
    CEGUI::OgreCEGUIRenderer* m_ceguirender;
	
    InputManager*     m_inputmgr;
    CeguiInjecter*    m_guiinput;
    TaskManager*      m_taskmgr;
    ElementManager*   m_elemmgr;
    WindowList*       m_windowlist;
    CameraController* m_camctrl;
    psynth::Timer m_timer;

    psynth::OSCClient* m_oscclient;
    psynth::OSCServer* m_oscserver;
    
    bool must_quit;

    void setupSettings(psynth::ConfNode& conf);
    void setupOgre(psynth::ConfNode& conf);
    void setupInput();
    void setupGui();
    void setupNet();
    void setupMenus();
    void setupTable();

    void closeTable();
    void closeMenus();
    void closeNet();
    void closeGui();
    void closeInput();
    void closeOgre();

    void printHelp();
    void printVersion();
    
public:
    Psychosynth3D();
    ~Psychosynth3D();

    void prepare(psynth::ArgParser& args);
    int execute();
       	
    bool frameStarted(const Ogre::FrameEvent& evt);
};

#endif /* PSYCHOSYNTH3D_H */

