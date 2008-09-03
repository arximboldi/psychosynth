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

#include "gui3d/psychosynth_3d.h"

#include "gui3d/quit_window.h"
#include "gui3d/conf_window.h"
#include "gui3d/info_window.h"
#include "gui3d/selector_window.h"
#include "gui3d/default_selector_populator.h"

#ifdef PSYNTH_HAVE_OSC
#include "gui3d/network_window.h"
#endif
#ifdef PSYNTH_HAVE_PCM
#include "gui3d/record_window.h"
#endif

#include "gui3d/camera_controller_default.h"
#include "gui3d/element.h"
#include "gui3d/element_types.h"
#include "gui3d/query_flags.h"
#include "gui3d/flat_ring.h"

#define DEFAULT_SCREEN_WIDTH  800
#define DEFAULT_SCREEN_HEIGHT 600
#define DEFAULT_FULLSCREEN    0
#define DEFAULT_FPS           120

using namespace Ogre;
using namespace std;
using namespace psynth;


psychosynth_3d::psychosynth_3d()
{
}

psychosynth_3d::~psychosynth_3d()
{
}

void psychosynth_3d::print_help ()
{
    cout <<
	"psychosynth_3d (c) 2007-2008 Juan Pedro Bolívar Puente\n"
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

void psychosynth_3d::print_version ()
{
    cout << "psychosynth_3d " << VERSION << endl;	
}

void psychosynth_3d::prepare (psynth::arg_parser& arg_parser)
{
    conf_node& conf = config::instance().get_child ("psynth3d");
    
    arg_parser.add('W', "width", new option_conf<int>(conf.get_child ("screen_width")));
    arg_parser.add('H', "height", new option_conf<int>(conf.get_child ("screen_height")));
    arg_parser.add('f', "fullscreen", new option_conf_value<int>(1, conf.get_child ("fullscreen")));
    arg_parser.add('w', "window", new option_conf_value<int>(0, conf.get_child ("fullscreen")));
    arg_parser.add('F', "fps", new option_conf<int>(conf.get_child ("fps")));
}

int psychosynth_3d::execute()
{
    logger::instance() ("gui", psynth::log::INFO, "Loading settings.");
     
    conf_node& conf = config::instance().get_child ("psynth3d");
    setup_settings (conf);
    
    logger::instance() ("gui", psynth::log::INFO, "Initializing Ogre.");
    setup_ogre (conf);
    logger::instance() ("gui", psynth::log::INFO, "Initializing OIS.");
    setup_input ();
    logger::instance() ("gui", psynth::log::INFO, "Initializing synthesizer.");
    setup_synth ();
#ifdef PSYNTH_HAVE_OSC
    logger::instance() ("gui", psynth::log::INFO, "Initializing networking.");
    setup_net ();
#endif
    logger::instance() ("gui", psynth::log::INFO, "Initializing scene.");
    setup_world ();
    logger::instance() ("gui", psynth::log::INFO, "Initializing CEGUI.");
    setup_gui ();
    logger::instance() ("gui", psynth::log::INFO, "Initializing GUI elements.");
    setup_menus ();
		
    m_ogre->startRendering();

    logger::instance() ("gui", psynth::log::INFO, "Closing GUI elements.");
    close_menus ();
    logger::instance() ("gui", psynth::log::INFO, "Closing CEGUI.");
    close_gui ();
#ifdef PSYNTH_HAVE_OSC
    logger::instance() ("gui", psynth::log::INFO, "Closing networking.");
    close_net ();
#endif
    logger::instance() ("gui", psynth::log::INFO, "Closing scene.");
    close_world ();
    logger::instance() ("gui", psynth::log::INFO, "Closing synthesizer.");
    close_synth ();
    logger::instance() ("gui", psynth::log::INFO, "Closing OIS.");
    close_input ();
    logger::instance() ("gui", psynth::log::INFO, "Closing Ogre.");
    close_ogre ();

    logger::instance() ("gui", psynth::log::INFO, "Storing settings.");
    conf.save();
    
    return 0;
}

bool psychosynth_3d::frameStarted (const Ogre::FrameEvent& evt)
{
    m_timer.update ();
    m_inputmgr->capture ();
    m_taskmgr->update (m_timer.delta_ticks ());
    get_world ()->update ();
    m_elemmgr->update ();

#ifdef PSYNTH_HAVE_OSC
    while(m_oscclient->receive ());
    m_oscclient->update (m_timer.delta_ticks ());
    while(m_oscserver->receive());
    m_oscserver->update (m_timer.delta_ticks());
#endif
    
    return !must_quit;
}

void psychosynth_3d::setup_settings (conf_node& conf)
{
#ifdef PSYNTH_HAVE_XML
    conf.attach_backend (new conf_backend_xml
			 ((get_config_path() / "psynth3d.xml").file_string ()));
#endif
    conf.def_load();

    conf.get_child ("screen_width").def(DEFAULT_SCREEN_WIDTH);
    conf.get_child ("screen_height").def(DEFAULT_SCREEN_HEIGHT);
    conf.get_child ("fullscreen").def(DEFAULT_FULLSCREEN);
    conf.get_child ("fps").def(DEFAULT_FPS);

    /* Is it dangerous to have this set before the gui is initialized? */
    conf.add_nudge_event (MakeDelegate(this, &psychosynth_3d::on_config_change));
    conf.get_child ("fps").add_change_event (MakeDelegate(this, &psychosynth_3d::on_fps_change));
}

void psychosynth_3d::setup_ogre (psynth::conf_node& conf)
{
    int screen_width;
    int screen_height;
    int fullscreen;
    int fps;

    conf.get_child ("screen_width").get(screen_width);
    conf.get_child ("screen_height").get(screen_height);
    conf.get_child ("fullscreen").get(fullscreen);
    conf.get_child ("fps").get(fps);
    
    (new LogManager)->createLog ((get_config_path() / "psynth3d_Ogre.log").file_string (),
				 false, false, false);  
    m_ogre = new Root ((get_data_path() / "plugins.cfg").file_string (),
		       (get_data_path() / "ogre.cfg").file_string ());
        
    ResourceGroupManager& res_mgr = ResourceGroupManager::getSingleton();
    res_mgr.addResourceLocation (get_data_path ().file_string (),
				 "FileSystem", "General");
    res_mgr.addResourceLocation ((get_data_path () / "mesh").file_string (),
				 "FileSystem", "General");
    res_mgr.addResourceLocation ((get_data_path () / "texture").file_string (),
				 "FileSystem", "General");
    res_mgr.addResourceLocation ((get_data_path () / "material").file_string (),
				 "FileSystem", "General");
    res_mgr.addResourceLocation ((get_data_path () / "gui").file_string (),
				 "FileSystem", "GUI");

    if (!m_ogre->restoreConfig() && !m_ogre->showConfigDialog())
	m_ogre->setRenderSystem( *(m_ogre->getAvailableRenderers()->begin()) );

    m_ogre->initialise(false);

    res_mgr.initialiseAllResourceGroups();


    m_window = m_ogre->createRenderWindow("psychosynth_3d",
					  screen_width,
					  screen_height,
					  fullscreen);
	
    m_ogre->addFrameListener(this);
	
    m_scene = m_ogre->createSceneManager(Ogre::ST_GENERIC, "main");

    m_timer.force_fps(fps);
}

void psychosynth_3d::setup_input ()
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

    m_inputmgr = new input_manager (pl);
}

void psychosynth_3d::setup_gui ()
{  
    m_ceguirender = new CEGUI::OgreCEGUIRenderer(m_window,
						 Ogre::RENDER_QUEUE_OVERLAY,
						 false, 3000, m_scene);

    m_gui = new CEGUI::System(m_ceguirender);

    CEGUI::Logger::getSingleton().setLogFilename (
	(get_config_path () / "psynth3d_CEGUI.log").file_string ());
    CEGUI::SchemeManager::getSingleton().loadScheme("TaharezLook.scheme");
    m_gui->setDefaultMouseCursor("TaharezLook", "MouseArrow");
    m_gui->setDefaultFont(CEGUI::FontManager::getSingleton().
			  createFont("DejaVuSans-9.font"));
	
    CEGUI::WindowManager *win = CEGUI::WindowManager::getSingletonPtr();
    CEGUI::Window *sheet = win->createWindow("DefaultGUISheet", "root"); // TODO: root?
    m_gui->setGUISheet(sheet);
    m_gui->setDefaultTooltip("TaharezLook/Tooltip");
    
    m_guiinput = new cegui_injecter (); 
    m_inputmgr->add_mouse_listener (m_guiinput);
    m_inputmgr->add_key_listener (m_guiinput);

    m_gui->setMultiClickTimeout(0.0f);
    //m_gui->setMultiClickToleranceAreaSize(CEGUI::Size(0.0f, 0.0f));
}

#ifdef PSYNTH_HAVE_OSC
void psychosynth_3d::setup_net ()
{
    m_oscclient = new osc_client();
    m_oscserver = new osc_server();

    m_oscclient->set_world (get_world());
    m_oscserver->set_world (get_world());
}
#endif

void psychosynth_3d::setup_world ()
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
    
    Entity *ent1 = m_scene->createEntity( "object1", "world.mesh" );
    ent1->setQueryFlags(QFLAG_WORLD);
    SceneNode *node1 = m_scene->getRootSceneNode()->createChildSceneNode();
    //node1->setScale(Vector3(1.5, 1.5, 1.5));
    node1->attachObject(ent1);
    node1->setScale(2, 2, 2);
    
    SceneNode *node = m_scene->getRootSceneNode()->createChildSceneNode();
    /* FIXME: Memory leak. */
    flat_ring* ring0 = new flat_ring ("the_point_0", Degree(0), Degree(360),
				      0, 0.2,
				      ColourValue(0, 0, 0, 0.8));
    flat_ring* ring1 = new flat_ring ("the_point_1", Degree(0), Degree(360),
				      0.4, 0.5,
				      ColourValue(0, 0, 0, 0.8));
    
    node->attachObject(ring0);
    node->attachObject(ring1);
    node->setPosition(Vector3(0,0.001,0));

    m_taskmgr = &task_manager::instance ();
    m_elemmgr = new element_manager (get_world(), m_scene, m_camera);
    m_camctrl = new camera_controller_default (m_camera);

    m_inputmgr->add_mouse_listener (m_camctrl);
    m_inputmgr->add_mouse_listener (m_elemmgr);
    m_inputmgr->add_key_listener (m_camctrl);
    m_inputmgr->add_key_listener (m_elemmgr);

    get_world()->add_world_listener (m_elemmgr);
    get_world()->add_world_patcher_listener (m_elemmgr);
}

void psychosynth_3d::setup_menus ()
{
    selector_window* selector = new selector_window (m_elemmgr);
    
    m_populator.populate(selector);
      
    m_windowlist = new window_list ();
    
    m_windowlist->add_window ("SelectorWindowButton.imageset",
			      "SelectorWindowButton.layout",
			      "Add objects to the world.",
			      selector,
			      OIS::KC_UNASSIGNED);
#ifdef PSYNTH_HAVE_PCM
    m_windowlist->add_window ("RecordWindowButton.imageset",
			      "RecordWindowButton.layout",
			      "Record the playing sound.",
			      new record_window (get_world ()),
			      OIS::KC_UNASSIGNED);
#endif
#ifdef PSYNTH_HAVE_OSC
    m_windowlist->add_window ("NetworkWindowButton.imageset",
			      "NetworkWindowButton.layout",
			      "Create or join network sessions.",
			      new network_window (m_oscclient,
						  m_oscserver),
			      OIS::KC_UNASSIGNED);
#endif
    m_windowlist->add_window ("ConfWindowButton.imageset",
			      "ConfWindowButton.layout",
			      "Configure the program settings.",
			      new conf_window (config::instance().get_child ("psynth3d"),
					       config::instance().get_child ("psychosynth")),
			      OIS::KC_UNASSIGNED);
    m_windowlist->add_window ("InfoWindowButton.imageset",
			      "InfoWindowButton.layout",
			      "General information about the program.",
			      new info_window (),
			      OIS::KC_UNASSIGNED);
    m_windowlist->add_window ("QuitWindowButton.imageset",
			      "QuitWindowButton.layout",
			      "Leave this fancy program.",
			      new quit_window (),
			      OIS::KC_ESCAPE);

    m_inputmgr->add_key_listener (m_windowlist);
}

void psychosynth_3d::close_world ()
{
    delete m_camctrl;
    delete m_elemmgr;
}

void psychosynth_3d::close_menus ()
{
    delete m_windowlist;
}

#ifdef PSYNTH_HAVE_OSC
void psychosynth_3d::close_net ()
{
    delete m_oscclient;
    delete m_oscserver;
}
#endif

void psychosynth_3d::close_gui ()
{
    delete m_guiinput;
// TODO
//	delete m_ceguirender;
//	delete m_gui; /
}

void psychosynth_3d::close_input ()
{
    delete m_inputmgr;
}

void psychosynth_3d::close_ogre ()
{
    delete m_ogre;
}

bool psychosynth_3d::on_config_change (psynth::conf_node& conf)
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

bool psychosynth_3d::on_fps_change (psynth::conf_node& conf)
{
    int fps;
    
    conf.get(fps);
    m_timer.force_fps(fps);

    return true;
}

