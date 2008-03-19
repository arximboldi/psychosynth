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

#include <config.h>

#include <libpsynth/version.h>
#include <libpsynth/common/Logger.h>
#include <libpsynth/common/Misc.h>
#include <libpsynth/common/OptionConf.h>
#ifdef PSYNTH_HAVE_XML
#include <libpsynth/common/ConfBackendXML.h>
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

void Psychosynth3D::printHelp()
{
    cout <<
	"Psychosynth3D (c) 2007-2008 Juan Pedro Bolívar Puente\n"
	"\n"
	"Usage:\n"
	"  psynth3d [options]\n"
	"\n"
	"Base options:\n";
    printBaseOptions(cout);
    cout <<
	"\n"
	"GUI options:\n"
	"  -W, --width <value>   Set the screen width.\n"
	"  -H, --height <value>  Set the screen height.\n"
	"  -f, --fullscreen      Run program in fullscreen.\n"
	"  -w, --window          Run program as a window.\n"
	"  -F, --fps <value>     Set the frames per second to render.\n";
}

void Psychosynth3D::printVersion()
{
    cout << "Psychosynth3D " << VERSION << endl;	
}

void Psychosynth3D::prepare(psynth::ArgParser& arg_parser)
{
    ConfNode& conf = Config::instance().getChild("psynth3d");
    
    arg_parser.add('W', "width", new OptionConf<int>(conf.getChild("screen_width")));
    arg_parser.add('H', "height", new OptionConf<int>(conf.getChild("screen_height")));
    arg_parser.add('f', "fullscreen", new OptionConfValue<int>(1, conf.getChild("fullscreen")));
    arg_parser.add('w', "window", new OptionConfValue<int>(0, conf.getChild("fullscreen")));
    arg_parser.add('F', "fps", new OptionConf<int>(conf.getChild("fps")));
}

int Psychosynth3D::execute()
{
    Logger::instance().log("gui", psynth::Log::INFO, "Loading settings.");
     
    ConfNode& conf = Config::instance().getChild("psynth3d");
    setupSettings(conf);
    
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing Ogre.");
    setupOgre(conf);
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing OIS.");
    setupInput();
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing synthesizer.");
    setupSynth();
#ifdef PSYNTH_HAVE_OSC
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing networking.");
    setupNet();
#endif
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing scene.");
    setupTable();
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing CEGUI.");
    setupGui();
    Logger::instance().log("gui", psynth::Log::INFO, "Initializing GUI elements.");
    setupMenus();
		
    m_ogre->startRendering();

    Logger::instance().log("gui", psynth::Log::INFO, "Closing GUI elements.");
    closeMenus();
    Logger::instance().log("gui", psynth::Log::INFO, "Closing CEGUI.");
    closeGui();
#ifdef PSYNTH_HAVE_OSC
    Logger::instance().log("gui", psynth::Log::INFO, "Closing networking.");
    closeNet();
#endif
    Logger::instance().log("gui", psynth::Log::INFO, "Closing scene.");
    closeTable();
    Logger::instance().log("gui", psynth::Log::INFO, "Closing synthesizer.");
    closeSynth();
    Logger::instance().log("gui", psynth::Log::INFO, "Closing OIS.");
    closeInput();
    Logger::instance().log("gui", psynth::Log::INFO, "Closing Ogre.");
    closeOgre();

    Logger::instance().log("gui", psynth::Log::INFO, "Storing settings.");
    conf.save();
    
    return 0;
}

bool Psychosynth3D::frameStarted(const Ogre::FrameEvent& evt)
{
    m_timer.update();
    m_inputmgr->capture();
    m_taskmgr->update(m_timer.deltaticks());
    getTable()->update();
    m_elemmgr->update();

#ifdef PSYNTH_HAVE_OSC
    while(m_oscclient->receive());
    m_oscclient->update(m_timer.deltaticks());
    while(m_oscserver->receive());
    m_oscserver->update(m_timer.deltaticks());
#endif
    
    return !must_quit;
}

void Psychosynth3D::setupSettings(ConfNode& conf)
{
#ifdef PSYNTH_HAVE_XML
    conf.attachBackend(new ConfBackendXML(getConfigPath() + "psynth3d.xml"));
#endif
    conf.defLoad();

    conf.getChild("screen_width").def(DEFAULT_SCREEN_WIDTH);
    conf.getChild("screen_height").def(DEFAULT_SCREEN_HEIGHT);
    conf.getChild("fullscreen").def(DEFAULT_FULLSCREEN);
    conf.getChild("fps").def(DEFAULT_FPS);

    /* Is it dangerous to have this set before the gui is initialized? */
    conf.addNudgeEvent(MakeDelegate(this, &Psychosynth3D::onConfigChange));
    conf.getChild("fps").addChangeEvent(MakeDelegate(this, &Psychosynth3D::onFpsChange));
}

void Psychosynth3D::setupOgre(psynth::ConfNode& conf)
{
    int screen_width;
    int screen_height;
    int fullscreen;
    int fps;

    conf.getChild("screen_width").get(screen_width);
    conf.getChild("screen_height").get(screen_height);
    conf.getChild("fullscreen").get(fullscreen);
    conf.getChild("fps").get(fps);
    
    (new LogManager)->createLog(getConfigPath() + "/psynth3d_Ogre.log",
				false, false, false);  
    m_ogre = new Root(getDataPath() + "/plugins.cfg",
		      getDataPath() + "/ogre.cfg");

    ResourceGroupManager& res_mgr = ResourceGroupManager::getSingleton();
    res_mgr.addResourceLocation(getDataPath(), "FileSystem", "General");
    res_mgr.addResourceLocation(getDataPath() + "/mesh", "FileSystem", "General");
    res_mgr.addResourceLocation(getDataPath() + "/texture", "FileSystem", "General");
    res_mgr.addResourceLocation(getDataPath() + "/material", "FileSystem", "General");
    res_mgr.addResourceLocation(getDataPath() + "/gui", "FileSystem", "GUI");

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

    m_timer.forceFps(fps);
}

void Psychosynth3D::setupInput()
{
    int fullscreen; /* I prefer not using the singleton here. */
    Config::instance().getPath("psynth3d/fullscreen").get(fullscreen);
    
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
    CEGUI::Logger::getSingleton().setLogFilename(getConfigPath() + "/psynth3d_CEGUI.log");
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
}

#ifdef PSYNTH_HAVE_OSC
void Psychosynth3D::setupNet()
{
    m_oscclient = new OSCClient();
    m_oscserver = new OSCServer();

    m_oscclient->setTable(getTable());
    m_oscserver->setTable(getTable());
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

    light->setPosition(Vector3(-20, 50, -20));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 1.6, 0, 0);

    light = m_scene->createLight("light2");
    light->setType(Light::LT_POINT);
    light->setPosition(Vector3(20, 50, 20));
    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setSpecularColour(1.0, 1.0, 1.0);
    light->setAttenuation(100, 1.6, 0, 0);
	
    Entity *ent1 = m_scene->createEntity( "object1", "table.mesh" );
    ent1->setQueryFlags(QFLAG_TABLE);
    SceneNode *node1 = m_scene->getRootSceneNode()->createChildSceneNode();
    //node1->setScale(Vector3(1.5, 1.5, 1.5));
    node1->attachObject(ent1);
    node1->setScale(1.5, 1.5, 1.5);
    
    SceneNode *node = m_scene->getRootSceneNode()->createChildSceneNode();
    /* FIXME: Memory leak. */
    FlatRing* ring = new FlatRing("the_point", Degree(0), Degree(360), 0, 0.4,
				  ColourValue(0, 0, 0, 0.8));
    node->attachObject(ring);
    node->setPosition(Vector3(0,0.001,0));

    m_taskmgr = new TaskManager();
    m_elemmgr = new ElementManager(getTable(), m_scene, m_camera);
    m_camctrl = new CameraControllerRasko(m_camera, m_taskmgr);

    m_inputmgr->addMouseListener(m_camctrl);
    m_inputmgr->addMouseListener(m_elemmgr);
    m_inputmgr->addKeyListener(m_camctrl);
    m_inputmgr->addKeyListener(m_elemmgr);

    getTable()->addTableListener(m_elemmgr);
    getTable()->addTablePatcherListener(m_elemmgr);
}

void Psychosynth3D::setupMenus()
{
    SelectorWindow* selector = new SelectorWindow(m_elemmgr);
    
    DefaultSelectorPopulator populator;

    populator.populate(getDataPath(), selector);
    
    /*
    cat = selector->addCategory("Wave");
    cat->addButton("Sine", ELEM_OSC_SINE);
    cat->addButton("Square", ELEM_OSC_SQUARE);
    cat->addButton("Sawtooth", ELEM_OSC_SAWTOOTH);
    cat->addButton("Triangle", ELEM_OSC_TRIANGLE);
    cat->addButton("Moogsaw", ELEM_OSC_MOOGSAW);
    cat->addButton("Exp", ELEM_OSC_EXP);
    
    cat = selector->addCategory("LFO");
    cat->addButton("Sine", ELEM_LFO_SINE);
    cat->addButton("Square", ELEM_LFO_SQUARE);
    cat->addButton("Sawtooth", ELEM_LFO_SAWTOOTH);
    cat->addButton("Triangle", ELEM_LFO_TRIANGLE);
    cat->addButton("Moogsaw", ELEM_LFO_MOOGSAW);
    cat->addButton("Exp", ELEM_LFO_EXP);
    
    cat = selector->addCategory("Filter");
    cat->addButton("Lowpass", ELEM_FILTER_LOWPASS);
    cat->addButton("Highpass", ELEM_FILTER_HIGHPASS);
    cat->addButton("Bandpass CSG", ELEM_FILTER_BANDPASS_CSG);
    cat->addButton("Bandpass CZPG", ELEM_FILTER_BANDPASS_CZPG);
    cat->addButton("Notch", ELEM_FILTER_NOTCH);
    cat->addButton("Moog", ELEM_FILTER_MOOG);
    
    cat = selector->addCategory("Mix");
    cat->addButton("Mixer", ELEM_MIXER);
    cat->addButton("Ring Mod.", ELEM_RINGMOD);
    cat->addButton("Ctrl. Mixer", ELEM_CTRLMIXER);
    cat->addButton("Ctrl. Ring Mod.", ELEM_CTRLRINGMOD);
    */
    
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
			    new RecordWindow(getTable()),
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
			    new ConfWindow(Config::instance().getChild("psynth3d"),
					   Config::instance().getChild("psychosynth")),
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
    delete m_taskmgr;
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

bool Psychosynth3D::onConfigChange(psynth::ConfNode& conf)
{
    int sc_width;
    int sc_height;
    int sc_fullscreen;
    int fps;

    conf.getChild("screen_width").get(sc_width);
    conf.getChild("screen_height").get(sc_height);
    conf.getChild("fullscreen").get(sc_fullscreen);
    conf.getChild("fps").get(fps);

    m_timer.forceFps(fps);
    m_window->setFullscreen(sc_fullscreen,
			    sc_width,
			    sc_height);
    return true;
}

bool Psychosynth3D::onFpsChange(psynth::ConfNode& conf)
{
    int fps;
    
    conf.get(fps);
    m_timer.forceFps(fps);

    return true;
}

