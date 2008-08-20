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

#include <cstring>
#include <cstdlib>
#include <config.h>

#include <libpsynth/version.h>
#include <libpsynth/common/logger.h>
#include <libpsynth/common/misc.h>
#include <libpsynth/common/option_conf.h>
#ifdef PSYNTH_HAVE_XML
#include <libpsynth/common/conf_backend_xml.h>
#endif

#include "gui3d/Psychosynth3D.h"

#include "gui3d/QuitWindow.h"
#include "gui3d/ConfWindow.h"
#include "gui3d/InfoWindow.h"
#include "gui3d/SelectorWindow.h"
#include "gui3d/DefaultSelectorPopulator.h"

#ifdef PSYNTH_HAVE_OSC
#include "gui3d/NetworkWindow.h"
#endif
#ifdef PSYNTH_HAVE_PCM
#include "gui3d/RecordWindow.h"
#endif

#include "gui3d/CameraControllerRasko.h"
#include "gui3d/Element.h"
#include "gui3d/ElementTypes.h"
#include "gui3d/QueryFlags.h"
#include "gui3d/FlatRing.h"

#define DEFAULT_SCREEN_WIDTH  800
#define DEFAULT_SCREEN_HEIGHT 600
#define DEFAULT_FULLSCREEN    0
#define DEFAULT_FPS           120

using namespace Ogre;
using namespace std;
using namespace psynth;


Psychosynth3D::Psychosynth3D()
{
}

Psychosynth3D::~Psychosynth3D()
{
}

void Psychosynth3D::print_help ()
{
    cout <<
	"Psychosynth3D (c) 2007-2008 Juan Pedro Bolívar Puente\n"
	"\n"
	"Usage:\n"
	"  psynth3d [options]\n"
	"\n"
	"Base options:\n";
    print_base_options (cout);
    cout <<
	"\n"
	"GUI options:\n"
	"  -W, --width <value>   Set the screen width.\n"
	"  -H, --height <value>  Set the screen height.\n"
	"  -f, --fullscreen      Run program in fullscreen.\n"
	"  -w, --window          Run program as a window.\n"
	"  -F, --fps <value>     Set the frames per second to render.\n";
}

void Psychosynth3D::print_version ()
{
    cout << "Psychosynth3D " << VERSION << endl;	
}

void Psychosynth3D::prepare (psynth::arg_parser& arg_parser)
{
    conf_node& conf = config::instance().get_child ("psynth3d");
    
    arg_parser.add('W', "width", new option_conf<int>(conf.get_child ("screen_width")));
    arg_parser.add('H', "height", new option_conf<int>(conf.get_child ("screen_height")));
    arg_parser.add('f', "fullscreen", new option_conf_value<int>(1, conf.get_child ("fullscreen")));
    arg_parser.add('w', "window", new option_conf_value<int>(0, conf.get_child ("fullscreen")));
    arg_parser.add('F', "fps", new option_conf<int>(conf.get_child ("fps")));
}

int Psychosynth3D::execute()
{
    logger::instance() ("gui", psynth::log::INFO, "Loading settings.");
     
    conf_node& conf = config::instance().get_child ("psynth3d");
    setupSettings(conf);
    
    logger::instance() ("gui", psynth::log::INFO, "Initializing Ogre.");
    setupOgre(conf);
    logger::instance() ("gui", psynth::log::INFO, "Initializing OIS.");
    setupInput();
    logger::instance() ("gui", psynth::log::INFO, "Initializing synthesizer.");
    setup_synth();
#ifdef PSYNTH_HAVE_OSC
    logger::instance() ("gui", psynth::log::INFO, "Initializing networking.");
    setupNet();
#endif
    logger::instance() ("gui", psynth::log::INFO, "Initializing scene.");
    setupTable();
    logger::instance() ("gui", psynth::log::INFO, "Initializing CEGUI.");
    setupGui();
    logger::instance() ("gui", psynth::log::INFO, "Initializing GUI elements.");
    setupMenus();
		
    m_ogre->startRendering();

    logger::instance() ("gui", psynth::log::INFO, "Closing GUI elements.");
    closeMenus();
    logger::instance() ("gui", psynth::log::INFO, "Closing CEGUI.");
    closeGui();
#ifdef PSYNTH_HAVE_OSC
    logger::instance() ("gui", psynth::log::INFO, "Closing networking.");
    closeNet();
#endif
    logger::instance() ("gui", psynth::log::INFO, "Closing scene.");
    closeTable();
    logger::instance() ("gui", psynth::log::INFO, "Closing synthesizer.");
    close_synth();
    logger::instance() ("gui", psynth::log::INFO, "Closing OIS.");
    closeInput();
    logger::instance() ("gui", psynth::log::INFO, "Closing Ogre.");
    closeOgre();

    logger::instance() ("gui", psynth::log::INFO, "Storing settings.");
    conf.save();
    
    return 0;
}

bool Psychosynth3D::frameStarted(const Ogre::FrameEvent& evt)
{
    m_timer.update ();
    m_inputmgr->capture ();
    m_taskmgr->update (m_timer.delta_ticks ());
    get_table ()->update ();
    m_elemmgr->update ();

#ifdef PSYNTH_HAVE_OSC
    while(m_oscclient->receive ());
    m_oscclient->update (m_timer.delta_ticks ());
    while(m_oscserver->receive());
    m_oscserver->update (m_timer.delta_ticks());
#endif
    
    return !must_quit;
}

void Psychosynth3D::setupSettings(conf_node& conf)
{
#ifdef PSYNTH_HAVE_XML
    conf.attach_backend (new conf_backend_xml (get_config_path() + "psynth3d.xml"));
#endif
    conf.def_load();

    conf.get_child ("screen_width").def(DEFAULT_SCREEN_WIDTH);
    conf.get_child ("screen_height").def(DEFAULT_SCREEN_HEIGHT);
    conf.get_child ("fullscreen").def(DEFAULT_FULLSCREEN);
    conf.get_child ("fps").def(DEFAULT_FPS);

    /* Is it dangerous to have this set before the gui is initialized? */
    conf.add_nudge_event (MakeDelegate(this, &Psychosynth3D::onConfigChange));
    conf.get_child ("fps").add_change_event (MakeDelegate(this, &Psychosynth3D::onFpsChange));
}

void Psychosynth3D::setupOgre(psynth::conf_node& conf)
{
    int screen_width;
    int screen_height;
    int fullscreen;
    int fps;

    conf.get_child ("screen_width").get(screen_width);
    conf.get_child ("screen_height").get(screen_height);
    conf.get_child ("fullscreen").get(fullscreen);
    conf.get_child ("fps").get(fps);
    
    (new LogManager)->createLog(get_config_path() + "/psynth3d_Ogre.log",
				false, false, false);  
    m_ogre = new Root(get_data_path() + "/plugins.cfg",
		      get_data_path() + "/ogre.cfg");
        
    ResourceGroupManager& res_mgr = ResourceGroupManager::getSingleton();
    res_mgr.addResourceLocation(get_data_path (), "FileSystem", "General");
    res_mgr.addResourceLocation(get_data_path () + "/mesh", "FileSystem", "General");
    res_mgr.addResourceLocation(get_data_path () + "/texture", "FileSystem", "General");
    res_mgr.addResourceLocation(get_data_path () + "/material", "FileSystem", "General");
    res_mgr.addResourceLocation(get_data_path () + "/gui", "FileSystem", "GUI");

    if (!m_ogre->restoreConfig() && !m_ogre->showConfigDialog())
	m_ogre->setRenderSystem( *(m_ogre->getAvailableRenderers()->begin()) );

    m_ogre->initialise(false);

    res_mgr.initialiseAllResourceGroups();


    m_window = m_ogre->createRenderWindow("Psychosynth3D",
					  screen_width,
					  screen_height,
					  fullscreen);
	
    m_ogre->addFrameListener(this);
	
    m_scene = m_ogre->createSceneManager(Ogre::ST_GENERIC, "main");

    m_timer.force_fps(fps);
}

void Psychosynth3D::setupInput()
{
    int fullscreen; /* I prefer not using the singleton here. */
    config::instance().get_path("psynth3d/fullscreen").get(fullscreen);
    
    OIS::ParamList pl;
    size_t window_hnd = 0;

    m_window->getCustomAttribute("WINDOW", &window_hnd);
    
    pl.insert(std::make_pair(std::string("WINDOW"), std::string(itoa(window_hnd, 10))));

#if defined OIS_WIN32_PLATFORM 
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_FOREGROUND" ))); 
    pl.insert(std::make_pair(std::string("w32_mouse"), std::string("DISCL_NONEXCLUSIVE"))); 
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_FOREGROUND"))); 
    pl.insert(std::make_pair(std::string("w32_keyboard"), std::string("DISCL_NONEXCLUSIVE"))); 
#elif defined OIS_LINUX_PLATFORM
    pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false"))); 
    pl.insert(std::make_pair(std::string("x11_mouse_hide"), std::string("true"))); 
    pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string(fullscreen ? "true" : "false"))); 
    pl.insert(std::make_pair(std::string("XAutoRepeatOn"), std::string("true"))); 
#endif

    m_inputmgr = new InputManager(pl);
}

void Psychosynth3D::setupGui()
{  
    m_ceguirender = new CEGUI::OgreCEGUIRenderer(m_window,
						 Ogre::RENDER_QUEUE_OVERLAY,
						 false, 3000, m_scene);

    m_gui = new CEGUI::System(m_ceguirender);

    CEGUI::Logger::getSingleton().setLogFilename(get_config_path () + "/psynth3d_CEGUI.log");
    CEGUI::SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
    m_gui->setDefaultMouseCursor("TaharezLook", "MouseArrow");
    m_gui->setDefaultFont(CEGUI::FontManager::getSingleton().
			  createFont("DejaVuSans-9.font"));
	
    CEGUI::WindowManager *win = CEGUI::WindowManager::getSingletonPtr();
    CEGUI::Window *sheet = win->createWindow("DefaultGUISheet", "root"); // TODO: root?
    m_gui->setGUISheet(sheet);
    m_gui->setDefaultTooltip("TaharezLook/Tooltip");
    
    m_guiinput = new CeguiInjecter(); 
    m_inputmgr->addMouseListener(m_guiinput);
    m_inputmgr->addKeyListener(m_guiinput);

    m_gui->setMultiClickTimeout(0.0f);
    //m_gui->setMultiClickToleranceAreaSize(CEGUI::Size(0.0f, 0.0f));
}

#ifdef PSYNTH_HAVE_OSC
void Psychosynth3D::setupNet()
{
    m_oscclient = new OSCClient();
    m_oscserver = new OSCServer();

    m_oscclient->setTable (get_table());
    m_oscserver->setTable (get_table());
}
#endif

void Psychosynth3D::setupTable()
{
    m_scene->setAmbientLight(ColourValue(0, 0, 0));
    //m_scene->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

    m_camera = m_scene->createCamera("camera");
    m_camera->setNearClipDistance(0.1);
    m_camera->setCastShadows(false);
    m_camera->setVisible(false);
    m_viewport = m_window->addViewport(m_camera);
    m_viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
    m_camera->setAspectRatio(Ogre::Real(m_window->getWidth())/m_window->getHeight());
    
    Light* light;
    light = m_scene->createLight("light1");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(-30, 20, -25));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 1.1, 0, 0);

    light = m_scene->createLight("light2");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(20, 20, 15));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 1.5, 0, 0);

    light = m_scene->createLight("light3");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(20, 20, -15));
    light->setDiffuseColour(1.0, 0.5, 0.5);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 1.3, 0, 0);

    light = m_scene->createLight("light4");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(-20, 20, 15));
    light->setDiffuseColour(1.0, 0.3, 0.3);
    light->setSpecularColour(1.0, 1.0, 0.3);
    light->setAttenuation(100, 1.5, 0, 0);
    
    Entity *ent1 = m_scene->createEntity( "object1", "table.mesh" );
    ent1->setQueryFlags(QFLAG_TABLE);
    SceneNode *node1 = m_scene->getRootSceneNode()->createChildSceneNode();
    //node1->setScale(Vector3(1.5, 1.5, 1.5));
    node1->attachObject(ent1);
    node1->setScale(2, 2, 2);
    
    SceneNode *node = m_scene->getRootSceneNode()->createChildSceneNode();
    /* FIXME: Memory leak. */
    FlatRing* ring = new FlatRing("the_point", Degree(0), Degree(360), 0, 0.4,
				  ColourValue(0, 0, 0, 0.8));
    node->attachObject(ring);
    node->setPosition(Vector3(0,0.001,0));

    m_taskmgr = &TaskManager::instance();
    m_elemmgr = new ElementManager (get_table(), m_scene, m_camera);
    m_camctrl = new CameraControllerRasko(m_camera, m_taskmgr);

    m_inputmgr->addMouseListener(m_camctrl);
    m_inputmgr->addMouseListener(m_elemmgr);
    m_inputmgr->addKeyListener(m_camctrl);
    m_inputmgr->addKeyListener(m_elemmgr);

    get_table()->addTableListener(m_elemmgr);
    get_table()->addTablePatcherListener(m_elemmgr);
}

void Psychosynth3D::setupMenus()
{
    SelectorWindow* selector = new SelectorWindow(m_elemmgr);
    
    m_populator.populate(selector);
      
    m_windowlist = new WindowList();
    
    m_windowlist->addWindow("SelectorWindowButton.imageset",
			    "SelectorWindowButton.layout",
			    "Add objects to the table.",
			    selector,
			    OIS::KC_UNASSIGNED);
#ifdef PSYNTH_HAVE_PCM
    m_windowlist->addWindow("RecordWindowButton.imageset",
			    "RecordWindowButton.layout",
			    "Record the playing sound.",
			    new RecordWindow (get_table ()),
			    OIS::KC_UNASSIGNED);
#endif
#ifdef PSYNTH_HAVE_OSC
    m_windowlist->addWindow("NetworkWindowButton.imageset",
			    "NetworkWindowButton.layout",
			    "Create or join network sessions.",
			    new NetworkWindow(m_oscclient,
					      m_oscserver),
			    OIS::KC_UNASSIGNED);
#endif
    m_windowlist->addWindow("ConfWindowButton.imageset",
			    "ConfWindowButton.layout",
			    "Configure the program settings.",
			    new ConfWindow(config::instance().get_child ("psynth3d"),
					   config::instance().get_child ("psychosynth")),
			    OIS::KC_UNASSIGNED);
    m_windowlist->addWindow("InfoWindowButton.imageset",
			    "InfoWindowButton.layout",
			    "General information about the program.",
			    new InfoWindow(),
			    OIS::KC_UNASSIGNED);
    m_windowlist->addWindow("QuitWindowButton.imageset",
			    "QuitWindowButton.layout",
			    "Leave this fancy program.",
			    new QuitWindow(),
			    OIS::KC_ESCAPE);

    m_inputmgr->addKeyListener(m_windowlist);
}

void Psychosynth3D::closeTable()
{
    delete m_camctrl;
    delete m_elemmgr;
}

void Psychosynth3D::closeMenus()
{
    delete m_windowlist;
}

#ifdef PSYNTH_HAVE_OSC
void Psychosynth3D::closeNet()
{
    delete m_oscclient;
    delete m_oscserver;
}
#endif

void Psychosynth3D::closeGui()
{
    delete m_guiinput;
// TODO
//	delete m_ceguirender;
//	delete m_gui; /
}

void Psychosynth3D::closeInput()
{
    delete m_inputmgr;
}

void Psychosynth3D::closeOgre()
{
    delete m_ogre;
}

bool Psychosynth3D::onConfigChange(psynth::conf_node& conf)
{
    int sc_width;
    int sc_height;
    int sc_fullscreen;
    int fps;

    conf.get_child ("screen_width").get(sc_width);
    conf.get_child ("screen_height").get(sc_height);
    conf.get_child ("fullscreen").get(sc_fullscreen);
    conf.get_child ("fps").get(fps);

    m_timer.force_fps(fps);
    m_window->setFullscreen(sc_fullscreen,
			    sc_width,
			    sc_height);
    //m_ogre->reinitialise();
    
    return true;
}

bool Psychosynth3D::onFpsChange(psynth::conf_node& conf)
{
    int fps;
    
    conf.get(fps);
    m_timer.force_fps(fps);

    return true;
}

