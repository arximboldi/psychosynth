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

#include <libpsynth/common/Thread.h>
#include <libpsynth/common/Timer.h>
#include <libpsynth/table/Table.h>
#include <libpsynth/output/Output.h>
#include <libpsynth/net/OSCClient.h>
#include <libpsynth/net/OSCServer.h>

#include "gui3d/InputManager.h"
#include "gui3d/CeguiInjecter.h"
#include "gui3d/TaskManager.h"
#include "gui3d/ElementManager.h"
#include "gui3d/WindowList.h"
#include "gui3d/CameraControllerRasko.h"

class PsychoSynth3D : public Ogre::FrameListener
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
    Timer m_timer;

    AudioInfo  m_audio_info;
    Table*     m_table;
    Output*    m_output;
    OSCClient* m_oscclient;
    OSCServer* m_oscserver;
    
    bool must_quit;
  
    void setupOgre();
    void setupInput();
    void setupGui();
    void setupSynth();
    void setupNet();
    void setupMenus();
    void setupTable();

    void closeTable();
    void closeMenus();
    void closeNet();
    void closeSynth();
    void closeGui();
    void closeInput();
    void closeOgre();

public:
    PsychoSynth3D();
    ~PsychoSynth3D();
		
    int run(int argc, const char* argv[]);
       	
    bool frameStarted(const Ogre::FrameEvent& evt);
};

#endif /* PSYCHOSYNTH3D_H */

