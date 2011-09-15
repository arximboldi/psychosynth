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

#include <algorithm>
#include <psynth/base/misc.hpp>
#include "gui3d/network_window.hpp"

using namespace std;
using namespace CEGUI;
using namespace psynth;
using namespace psynth::base;

const char* DEFAULT_HOST = "localhost";

const int NW_HEIGHT = 300;
const int NW_WIDTH  = 300;

client_tab::client_tab (psynth::world* world) :
    m_client(new osc_client),
    m_connected(false),
    m_logsink (new gui_log_sink)
{
    m_client->set_world (world);
    m_client->add_listener(this);
    m_client->add_listener(&m_logger);
    
    logger::self ().child ("oscclient").add_sink (m_logsink);
}

client_tab::~client_tab ()
{
    m_client->delete_listener(this);
    m_client->delete_listener(&m_logger);
 
    logger::self ().child ("oscclient").del_sink (m_logsink);
    m_logsink->set_window (0);
    delete m_client;
}

Window* client_tab::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    Window* root = wmgr.createWindow("DefaultGUISheet");
    root->setPosition( UVector2(UDim(0, 10), UDim(0, 10)) );
    root->setSize    ( UVector2(UDim(1, -20),     UDim(1, -20)) );
    root->setText("Client");

    m_disable = wmgr.createWindow("DefaultGUISheet");
    m_disable->setPosition( UVector2(UDim(0, 0), UDim(0, 1)) );
    m_disable->setSize    ( UVector2(UDim(1, 0),     UDim(0, 75)) );

    Window* host_label = wmgr.createWindow("TaharezLook/StaticText");
    host_label->setText("Remote host");
    host_label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    host_label->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));

    m_host = dynamic_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    m_host->setPosition(UVector2(UDim(0.5, 0), UDim(0, 0)));
    m_host->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_host->setText(DEFAULT_HOST);

    Window* rport_label = wmgr.createWindow("TaharezLook/StaticText");
    rport_label->setText("Remote port");
    rport_label->setPosition(UVector2(UDim(0, 0), UDim(0, 25)));
    rport_label->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));

    m_rport = dynamic_cast<Spinner*>(wmgr.createWindow("TaharezLook/Spinner"));
    m_rport->setPosition(UVector2(UDim(0.5, 0), UDim(0, 25)));
    m_rport->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_rport->setCurrentValue(PSYNTH_DEFAULT_SERVER_PORT);
    
    Window* lport_label = wmgr.createWindow("TaharezLook/StaticText");
    lport_label->setText("Local port");
    lport_label->setPosition(UVector2(UDim(0, 0), UDim(0, 50)));
    lport_label->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));

    m_lport = dynamic_cast<Spinner*>(wmgr.createWindow("TaharezLook/Spinner"));
    m_lport->setPosition(UVector2(UDim(0.5, 0), UDim(0, 50)));
    m_lport->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_lport->setCurrentValue(PSYNTH_DEFAULT_CLIENT_PORT);

    m_button = wmgr.createWindow("TaharezLook/Button");
    m_button->setText("Connect");
    m_button->setPosition(UVector2(UDim(0, 0), UDim(0, 75)));
    m_button->setSize    (UVector2(UDim(1, 0), UDim(0, 20)));
    m_button->setWantsMultiClickEvents(false);
    m_button->subscribeEvent(PushButton::EventClicked, 
			     Event::Subscriber(&client_tab::on_button_click, this));

    MultiLineEditbox* log_viewer = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    log_viewer->setPosition(UVector2(UDim(0, 0), UDim(0, 100)));
    log_viewer->setSize    (UVector2(UDim(1, 0), UDim(1, -100)));
    log_viewer->setReadOnly(true);
    m_logsink->set_window (log_viewer);
    
    m_disable->addChildWindow(host_label);
    m_disable->addChildWindow(m_host);
    m_disable->addChildWindow(rport_label);
    m_disable->addChildWindow(m_rport);
    m_disable->addChildWindow(lport_label);
    m_disable->addChildWindow(m_lport);
    
    root->addChildWindow(m_disable);
    root->addChildWindow(m_button);
    root->addChildWindow(log_viewer);
    
    return root;
}

void client_tab::set_connected (bool con)
{
    if (m_connected != con) {
	m_connected = con;
	if (m_connected) {
	    m_disable->disable();
	    if (m_ext_disable)
		m_ext_disable->disable();
	    m_button->setText("Disconnect");
	} else {
	    m_disable->enable();
	    if (m_ext_disable)
		m_ext_disable->enable();
	    m_button->setText("Connect");
	}
    }
}

bool client_tab::on_button_click (const CEGUI::EventArgs &e)
{
    if (!m_connected) {
	lo_address add;
	add = lo_address_new(m_host->getText().c_str(),
			     itoa (m_rport->getCurrentValue(), 10));

	m_client->connect(add, itoa (m_lport->getCurrentValue(), 10));
    } else {
	m_client->disconnect();
    }
    
    return true;
}

bool client_tab::handle_client_connect (osc_client* client)
{
    set_connected (true);
    return false;
}

bool client_tab::handle_client_disconnect (osc_client* client, osc_client_error err)
{
    set_connected (false);
    return false;
}

void client_tab::update (int ticks)
{
    while (m_client->receive ());
    m_client->update (ticks);
}

server_tab::server_tab (psynth::world* world) :
    m_server(new osc_server),
    m_listening(false),
    m_logsink (new gui_log_sink)
{
    m_server->set_world (world);
    m_server->add_listener(this);
    m_server->add_listener(&m_logger);
 
    logger::self ().child ("oscserver").add_sink (m_logsink);
}

server_tab::~server_tab ()
{
    m_server->delete_listener(this);
    m_server->delete_listener(&m_logger);
    
    logger::self ().child ("oscserver").del_sink (m_logsink);
    m_logsink->set_window (0);
    delete m_server;
}

Window* server_tab::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    Window* root = wmgr.createWindow("DefaultGUISheet");
    root->setPosition( UVector2(UDim(0, 10), UDim(0, 10)) );
    root->setSize    ( UVector2(UDim(1, -20),     UDim(1, -20)) );
    root->setText("Server");

    m_disable = wmgr.createWindow("DefaultGUISheet");
    m_disable->setPosition( UVector2(UDim(0, 0), UDim(0, 1)) );
    m_disable->setSize    ( UVector2(UDim(1, 0),     UDim(0, 25)) );
    
    Window* lport_label = wmgr.createWindow("TaharezLook/StaticText");
    lport_label->setText("Port");
    lport_label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    lport_label->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));

    m_lport = dynamic_cast<Spinner*>(wmgr.createWindow("TaharezLook/Spinner"));
    m_lport->setPosition(UVector2(UDim(0.5, 0), UDim(0, 0)));
    m_lport->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_lport->setCurrentValue(PSYNTH_DEFAULT_SERVER_PORT);

    m_button = wmgr.createWindow("TaharezLook/Button");
    m_button->setText("Start");
    m_button->setPosition(UVector2(UDim(0, 0), UDim(0, 25)));
    m_button->setSize    (UVector2(UDim(1, 0), UDim(0, 20)));
    m_button->setWantsMultiClickEvents(false);
    m_button->subscribeEvent(PushButton::EventClicked, 
			     Event::Subscriber(&server_tab::on_button_click, this));

    MultiLineEditbox* log_viewer = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    log_viewer->setPosition(UVector2(UDim(0, 0), UDim(0, 50)));
    log_viewer->setSize    (UVector2(UDim(1, 0), UDim(1, -50)));
    log_viewer->setReadOnly(true);
    m_logsink->set_window (log_viewer);
    
    m_disable->addChildWindow(lport_label);
    m_disable->addChildWindow(m_lport);
    
    root->addChildWindow(m_disable);
    root->addChildWindow(m_button);
    root->addChildWindow(log_viewer);
    
    return root;
}

void server_tab::set_listening (bool lis)
{
    if (m_listening != lis) {
	m_listening = lis;
	if (m_listening) {
	    m_disable->disable();
	    if (m_ext_disable)
		m_ext_disable->disable();
	    m_button->setText("Stop");
	} else {
	    m_disable->enable();
	    if (m_ext_disable)
		m_ext_disable->enable();
	    m_button->setText("Start");
	}
    }
}

bool server_tab::handle_server_start_listening (osc_server* server)
{
    set_listening (true);
    return false;
}

bool server_tab::handle_server_stop_listening (osc_server* server, osc_server_error err)
{
    set_listening (false);
    return false;
}

bool server_tab::on_button_click (const CEGUI::EventArgs &e)
{
    if (!m_listening) {
	m_server->listen (itoa(m_lport->getCurrentValue(), 10));
    } else {
	m_server->stop ();
    }
    return true;
}

void server_tab::update (int ticks)
{
    while (m_server->receive ());
    m_server->update (ticks);
}

passive_tab::passive_tab (psynth::world* world)
    : m_world (world),
    m_passive (0),
    m_listening (false),
    m_logsink (new gui_log_sink)
{
    logger::self ().child ("oscpassive").add_sink (m_logsink);
}

passive_tab::~passive_tab ()
{
    logger::self ().child ("oscpassive").del_sink (m_logsink);
    m_logsink->set_window (0);
    delete m_passive;
}

Window* passive_tab::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    Window* root = wmgr.createWindow("DefaultGUISheet");
    root->setPosition( UVector2(UDim(0, 10), UDim(0, 10)) );
    root->setSize    ( UVector2(UDim(1, -20),     UDim(1, -20)) );
    root->setText("Passive OSC");
    
    Window* lport_label = wmgr.createWindow("TaharezLook/StaticText");
    lport_label->setText("Port");
    lport_label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    lport_label->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));

    m_lport = dynamic_cast<Spinner*>(wmgr.createWindow("TaharezLook/Spinner"));
    m_lport->setPosition(UVector2(UDim(0.5, 0), UDim(0, 0)));
    m_lport->setSize    (UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_lport->setCurrentValue(PSYNTH_DEFAULT_PASSIVE_PORT);

    m_button = wmgr.createWindow("TaharezLook/Button");
    m_button->setText("Start");
    m_button->setPosition(UVector2(UDim(0, 0), UDim(0, 25)));
    m_button->setSize    (UVector2(UDim(1, 0), UDim(0, 20)));
    m_button->setWantsMultiClickEvents(false);
    m_button->subscribeEvent(PushButton::EventClicked, 
			     Event::Subscriber(&passive_tab::on_button_click, this));

    MultiLineEditbox* log_viewer = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    log_viewer->setPosition(UVector2(UDim(0, 0), UDim(0, 50)));
    log_viewer->setSize    (UVector2(UDim(1, 0), UDim(1, -50)));
    log_viewer->setReadOnly(true);
    m_logsink->set_window (log_viewer);
    
    root->addChildWindow(lport_label);
    root->addChildWindow(m_lport);
    
    root->addChildWindow(m_button);
    root->addChildWindow(log_viewer);
    
    return root;
}

bool passive_tab::on_button_click (const CEGUI::EventArgs &e)
{
    if (!m_passive) {
	m_passive = new net::osc_passive (itoa(m_lport->getCurrentValue(), 10));
        m_passive->set_world (m_world);
    } else {
	delete m_passive;
        m_passive = 0;
    }
    return true;
}

void passive_tab::update (int ticks)
{
    if (m_passive) {
        while (m_passive->receive ());
    }
}


network_window::network_window(psynth::world* world) :
    m_client_tab (new client_tab (world)),
    m_server_tab (new server_tab (world)),
    m_passive_tab (new passive_tab (world))
{
}

network_window::~network_window()
{
}

void network_window::update (int ticks)
{
    m_client_tab->update (ticks);
    m_server_tab->update (ticks);
    m_passive_tab->update (ticks);
}

FrameWindow* network_window::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow", "network_window"));
	
    window->setPosition(UVector2(UDim(0.5, -NW_WIDTH/2), UDim(0.5, -NW_HEIGHT/2)));
    window->setSize    (UVector2(UDim(0, NW_WIDTH),UDim(0, NW_HEIGHT)));
    window->setText("Network");
    
    Window* container = wmgr.createWindow("TaharezLook/TabControl", "network_mode_tab");
    container->setPosition( UVector2(UDim(0, 10), UDim(0, 30)) );
    container->setSize    ( UVector2(UDim(1, -20),     UDim(1, -40)) );

    Window* client_win = m_client_tab->create_window ();
    Window* server_win = m_server_tab->create_window ();
    Window* passive_win = m_passive_tab->create_window ();
    
    m_server_tab->external_dependant (client_win);
    m_client_tab->external_dependant (server_win);
    
    window->addChildWindow(container);
    container->addChildWindow(client_win);
    container->addChildWindow(server_win);
    container->addChildWindow(passive_win);
    
    return window;
}
