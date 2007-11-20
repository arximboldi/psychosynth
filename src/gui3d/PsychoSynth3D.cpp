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
#include "gui3d/ElementTypes.h"
#include "gui3d/QueryFlags.h"
#include "gui3d/FlatRing.h"

#include "output/OutputAlsa.h"
#include "table/PatcherDynamic.h"

using namespace Ogre;
using namespace std;

PsychoSynth3D::PsychoSynth3D() :
    m_audio_info(DEFAULT_SAMPLE_RATE, DEFAULT_BUFFER_SIZE, DEFAULT_NUM_CHANNEL)
{
}

PsychoSynth3D::~PsychoSynth3D()
{
}

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
	
    m_scene = m_ogre->createSceneManager(Ogre::ST_GENERIC, "main");

    /* TODO: This scene creation code must be separated? */
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
			  createFont("DejaVuSans-9.font"));
	
    CEGUI::WindowManager *win = CEGUI::WindowManager::getSingletonPtr();
    CEGUI::Window *sheet = win->createWindow("DefaultGUISheet", "root"); // TODO: root?
    m_gui->setGUISheet(sheet);

    m_guiinput   = new CeguiInjecter(); 
    m_inputmgr->addMouseListener(m_guiinput);
    m_inputmgr->addKeyListener(m_guiinput);
}

void PsychoSynth3D::setupSynth()
{
    m_table = new Table(m_audio_info);
    m_output = new OutputAlsa(m_audio_info, "default");
    m_table->attachOutput(m_output);
    m_table->attachPatcher(new PatcherDynamic());
    m_table->update();
    m_output->open();
    m_output->start();
}

void PsychoSynth3D::setupTable()
{
    m_camera = m_scene->createCamera("camera");
    m_camera->setNearClipDistance(5);
    m_viewport = m_window->addViewport(m_camera);

    m_scene->setAmbientLight(ColourValue(0, 0, 0));
    m_scene->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    Light* light;
    light = m_scene->createLight("light1");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(-30, 30, -30));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 3, 0, 0);

    light = m_scene->createLight("light2");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(30, 30, 30));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 3, 0, 0);

    light = m_scene->createLight("light3");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(30, 30, -30));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 2, 0, 0);
    
    light = m_scene->createLight("light4");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(-30, 30, 30));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 2.5, 0, 0);
    
    /*
    light = m_scene->createLight("light2");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(30, 30, 30));
    light->setDiffuseColour(0.7, 0.4, 0.3);
    light->setSpecularColour(0.7, 0.4, 0.3);
    */
    m_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
    m_camera->setAspectRatio(Ogre::Real(m_window->getWidth())/m_window->getHeight());
	
    Entity *ent1 = m_scene->createEntity( "object1", "table.mesh" );
    ent1->setQueryFlags(QFLAG_TABLE);
    SceneNode *node1 = m_scene->getRootSceneNode()->createChildSceneNode();
    //node1->setScale(Vector3(1.5, 1.5, 1.5));
    node1->attachObject(ent1);
    
    
    SceneNode *node = m_scene->getRootSceneNode()->createChildSceneNode();
    FlatRing* ring = new FlatRing("ringy", Degree(0), Degree(360), 0, 0.4,
				  ColourValue(0, 0, 0, 0.8));
    node->attachObject(ring);
    node->setPosition(Vector3(0,0.001,0));

    m_taskmgr = new TaskManager();
    m_elemmgr = new ElementManager(m_table, m_scene, m_camera);
    m_camctrl = new CameraControllerRasko(m_camera, m_taskmgr);

    m_inputmgr->addMouseListener(m_camctrl);
    m_inputmgr->addMouseListener(m_elemmgr);
    m_inputmgr->addKeyListener(m_camctrl);
    m_inputmgr->addKeyListener(m_elemmgr);

    m_table->addTableListener(m_elemmgr);
    m_table->addTablePatcherListener(m_elemmgr);
}

void PsychoSynth3D::setupMenus()
{
    m_windowlist = new WindowList();
	
    ElementSelector* selector = new ElementSelector(m_elemmgr);
    ElementSelector::Category* cat = NULL;

    cat = selector->addCategory("Wave");
    cat->addButton("Sine", ELEM_OSC_SINE);
    cat->addButton("Square", ELEM_OSC_SQUARE);
    cat->addButton("Sawtooth", ELEM_OSC_SAWTOOTH);
    cat->addButton("Triangle", ELEM_OSC_TRIANGLE);

    cat = selector->addCategory("Mix");
    cat->addButton("Mixer", ELEM_MIXER);
	
    m_windowlist->addWindow("ElementSelectorButton.imageset",
			    "ElementSelectorButton.layout",
			    selector,
			    OIS::KC_UNASSIGNED);
    
    m_windowlist->addWindow("QuitWindowButton.imageset",
			    "QuitWindowButton.layout",
			    new QuitWindow(),
			    OIS::KC_ESCAPE);

    m_inputmgr->addKeyListener(m_windowlist);
}

void PsychoSynth3D::closeTable()
{
    delete m_taskmgr;
    delete m_camctrl;
    delete m_elemmgr;
}

void PsychoSynth3D::closeMenus()
{
    delete m_windowlist;
}

void PsychoSynth3D::closeSynth()
{
    delete m_table;
}

void PsychoSynth3D::closeGui()
{
    delete m_guiinput;
// TODO
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
    setupSynth();
    setupTable();
    setupGui();
    setupMenus();
		
    m_timer.forceFps(120);
		
    m_ogre->startRendering();
    
    closeMenus();
    closeGui();
    closeTable();
    closeSynth();
    closeInput();
    closeOgre();
       
    return 0;
}

bool PsychoSynth3D::frameStarted(const Ogre::FrameEvent& evt)
{
    m_timer.update();
    m_inputmgr->capture();
    m_taskmgr->update(m_timer.deltaticks());
    m_table->update();
    m_elemmgr->update();
	
    return !must_quit;
}
