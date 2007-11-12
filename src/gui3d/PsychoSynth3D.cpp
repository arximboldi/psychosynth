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

#include "gui3d/PsychoSynth3D.h"
#include "gui3d/QuitWindow.h"
#include "gui3d/ElementSelector.h"
#include "gui3d/CameraControllerRasko.h"
#include "gui3d/Element.h"
#include "gui3d/QueryFlags.h"
#include "gui3d/FlatRing.h"


using namespace Ogre;
using namespace std;

void PsychoSynth3D::setupOgre()
{
    m_ogre = new Root("data/plugins.cfg", "data/ogre.cfg");

    ResourceGroupManager& resource_manager = ResourceGroupManager::getSingleton();
    resource_manager.addResourceLocation("data", "FileSystem", "General");
    resource_manager.addResourceLocation("data/gui/schemes", "FileSystem", "GUI");
    resource_manager.addResourceLocation("data/gui/fonts", "FileSystem", "GUI");
    resource_manager.addResourceLocation("data/gui/configs", "FileSystem", "GUI");
    resource_manager.addResourceLocation("data/gui/imagesets", "FileSystem", "GUI");
    resource_manager.addResourceLocation("data/gui/looknfeel", "FileSystem", "GUI");
    resource_manager.addResourceLocation("data/gui/layouts", "FileSystem", "GUI");

    if (!m_ogre->restoreConfig() && !m_ogre->showConfigDialog())
	m_ogre->setRenderSystem( *(m_ogre->getAvailableRenderers()->begin()) );

    m_ogre->initialise(false);

    resource_manager.initialiseAllResourceGroups();

    m_window = m_ogre->createRenderWindow("PsychoSynth3D", 800, 600, false);
	
    m_ogre->addFrameListener(this);
	
    /*** TODO: This scene creation code must be separated *********************/
    m_scene = m_ogre->createSceneManager(Ogre::ST_GENERIC, "main");
	
    m_camera = m_scene->createCamera("camera");
    m_camera->setNearClipDistance(5);
    m_viewport = m_window->addViewport(m_camera);

    m_scene->setAmbientLight(ColourValue(0, 0, 0));
    m_scene->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    Light* light1 = m_scene->createLight("light1");
    light1->setType(Light::LT_POINT);
    light1->setPosition(Vector3(-10, 20, -5));
    light1->setDiffuseColour(1.0, 1.0, 1.0);
    light1->setSpecularColour(1.0, 1.0, 1.0);

    Light* light2 = m_scene->createLight("light2");
    light2->setType(Light::LT_POINT);
    light2->setPosition(Vector3(30, 30, 30));
    light2->setDiffuseColour(0.7, 0.4, 0.3);
    light2->setSpecularColour(0.7, 0.4, 0.3);

    m_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
    m_camera->setAspectRatio(Ogre::Real(m_window->getWidth())/m_window->getHeight());
	
    Entity *ent1 = m_scene->createEntity( "object1", "table.mesh" );
    ent1->setQueryFlags(QFLAG_TABLE);
    SceneNode *node1 = m_scene->getRootSceneNode()->createChildSceneNode("nodo1");
    node1->attachObject( ent1 );

    SceneNode *node = m_scene->getRootSceneNode()->createChildSceneNode("myring");
    FlatRing* ring = new FlatRing("ringy", Degree(0), Degree(360), 0, 0.4,
				  ColourValue(0, 0, 0, 0.7));
    node->attachObject(ring);
    node->setPosition(Vector3(0,0.001,0));

}

void PsychoSynth3D::setupInput()
{
    size_t window_hnd = 0;
    m_window->getCustomAttribute("WINDOW", &window_hnd);
    m_inputmgr = new InputManager(window_hnd);
}

void PsychoSynth3D::setupGui()
{
    m_ceguirender = new CEGUI::OgreCEGUIRenderer(m_window,
						 Ogre::RENDER_QUEUE_OVERLAY, false,
						 3000, m_scene);
    m_gui = new CEGUI::System(m_ceguirender);
	
    CEGUI::SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
    m_gui->setDefaultMouseCursor("TaharezLook", "MouseArrow");
    m_gui->setDefaultFont(CEGUI::FontManager::getSingleton().
			  createFont("DejaVuSans-10.font"));
	
    /* TODO */
    CEGUI::WindowManager *win = CEGUI::WindowManager::getSingletonPtr();
    CEGUI::Window *sheet = win->createWindow("DefaultGUISheet", "root");
    m_gui->setGUISheet(sheet);
}

void PsychoSynth3D::closeGui()
{
//  TODO
//	delete m_ceguirender;
//	delete m_gui; /
}

void PsychoSynth3D::closeInput()
{
    delete m_inputmgr;
}

void PsychoSynth3D::closeOgre()
{
    delete m_ogre;
}

int PsychoSynth3D::run(int argc, const char* argv[])
{
    setupOgre();
    setupInput();
    setupGui();

    m_guiinput   = new CeguiInjecter();
    m_taskmgr    = new TaskManager();
    m_elemmgr    = new ElementManager(NULL, m_scene, m_camera);
    m_camctrl    = new CameraControllerRasko(m_camera, m_taskmgr);
    m_windowlist = new WindowList();
	
    ElementSelector* selector = new ElementSelector(m_elemmgr);
    ElementSelector::Category* cat = NULL;
    cat = selector->addCategory("Wave");
    cat->addButton("Sine", 0);
    cat->addButton("Square", 0);
    cat->addButton("Sawtooth", 0);
    cat->addButton("Triangle", 0);
    cat = selector->addCategory("Sample");
    cat->addButton("Bass.wav", 0);
    cat = selector->addCategory("Control");
	
    m_windowlist->addWindow("ElementSelectorButton.imageset",
			    "ElementSelectorButton.layout",
			    selector,
			    OIS::KC_UNASSIGNED);
    m_windowlist->addWindow("QuitWindowButton.imageset",
			    "QuitWindowButton.layout",
			    new QuitWindow(),
			    OIS::KC_ESCAPE);
	
    m_inputmgr->addMouseListener(m_guiinput);
    m_inputmgr->addMouseListener(m_elemmgr);
    m_inputmgr->addMouseListener(m_camctrl);
	
    m_inputmgr->addKeyListener(m_guiinput);
    m_inputmgr->addKeyListener(m_elemmgr);
    m_inputmgr->addKeyListener(m_camctrl);
    m_inputmgr->addKeyListener(m_windowlist);
		
    m_timer.forceFps(120);
		
    m_ogre->startRendering();
	
    delete m_taskmgr;
    delete m_camctrl;
    delete m_windowlist;
    delete m_elemmgr;
	
    closeGui();
    closeInput();
    closeOgre();
       
    return 0;
}

bool PsychoSynth3D::frameStarted(const Ogre::FrameEvent& evt)
{
    m_timer.update();
    m_inputmgr->capture();

    m_taskmgr->update(m_timer.deltaticks());
    return !must_quit;
}
