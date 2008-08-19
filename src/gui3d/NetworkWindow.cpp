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
#include "gui3d/NetworkWindow.h"

using namespace std;
using namespace CEGUI;
using namespace psynth;

const char* DEFAULT_HOST = "localhost";

const int NW_HEIGHT = 300;
const int NW_WIDTH  = 300;

ClientTab::ClientTab(OSCClient* client) :
    m_client(client),
    m_connected(false)
{
    m_client->addListener(this);
    m_client->addListener(&m_logger);
    
    psynth::logger::instance().get_child ("oscclient").attach_sink (&m_logsink);
}

ClientTab::~ClientTab()
{
    m_client->deleteListener(this);
    m_client->deleteListener(&m_logger);
 
    psynth::logger::instance().get_child ("oscclient").dattach_sink (&m_logsink);
    m_logsink.setWindow(NULL);
}

Window* ClientTab::createWindow()
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
			     Event::Subscriber(&ClientTab::onButtonClick, this));

    MultiLineEditbox* log_viewer = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    log_viewer->setPosition(UVector2(UDim(0, 0), UDim(0, 100)));
    log_viewer->setSize    (UVector2(UDim(1, 0), UDim(1, -100)));
    log_viewer->setReadOnly(true);
    m_logsink.setWindow(log_viewer);
    
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

void ClientTab::setConnected(bool con)
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

bool ClientTab::onButtonClick(const CEGUI::EventArgs &e)
{
    if (!m_connected) {
	lo_address add;
	add = lo_address_new(m_host->getText().c_str(),
			     itoa(m_rport->getCurrentValue(), 10));

	m_client->connect(add, itoa(m_lport->getCurrentValue(), 10));
    } else {
	m_client->disconnect();
    }
    
    return true;
}

bool ClientTab::handleClientConnect(OSCClient* client)
{
    setConnected(true);
    return false;
}

bool ClientTab::handleClientDisconnect(OSCClient* client, OSCClientError err)
{
    setConnected(false);
    return false;
}

ServerTab::ServerTab(OSCServer* server) :
    m_server(server),
    m_listening(false)
{
    m_server->addListener(this);
    m_server->addListener(&m_logger);
 
    psynth::logger::instance().get_child ("oscserver").attach_sink (&m_logsink);
}

ServerTab::~ServerTab()
{
    
    m_server->deleteListener(this);
    m_server->deleteListener(&m_logger);
    
    psynth::logger::instance().get_child ("oscserver").dattach_sink (&m_logsink);
    m_logsink.setWindow(NULL);
}

Window* ServerTab::createWindow()
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
			     Event::Subscriber(&ServerTab::onButtonClick, this));

    MultiLineEditbox* log_viewer = dynamic_cast<MultiLineEditbox*>(wmgr.createWindow("TaharezLook/MultiLineEditbox"));
    log_viewer->setPosition(UVector2(UDim(0, 0), UDim(0, 50)));
    log_viewer->setSize    (UVector2(UDim(1, 0), UDim(1, -50)));
    log_viewer->setReadOnly(true);
    m_logsink.setWindow(log_viewer);
    
    m_disable->addChildWindow(lport_label);
    m_disable->addChildWindow(m_lport);
    
    root->addChildWindow(m_disable);
    root->addChildWindow(m_button);
    root->addChildWindow(log_viewer);
    
    return root;
}

void ServerTab::setListening(bool lis)
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

bool ServerTab::handleServerStartListening(OSCServer* server)
{
    setListening(true);
    return false;
}

bool ServerTab::handleServerStopListening(OSCServer* server, OSCServerError err)
{
    setListening(false);
    return false;
}

bool ServerTab::onButtonClick(const CEGUI::EventArgs &e)
{
    if (!m_listening) {
	m_server->listen(itoa(m_lport->getCurrentValue(), 10));
    } else {
	m_server->stop();
    }
    return true;
}

NetworkWindow::NetworkWindow(OSCClient* client, OSCServer* server) :
    m_client_tab(new ClientTab(client)),
    m_server_tab(new ServerTab(server))
{
}

NetworkWindow::~NetworkWindow()
{
    delete m_server_tab;
    delete m_client_tab;
}

FrameWindow* NetworkWindow::createWindow()
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

    Window* client_win = m_client_tab->createWindow();
    Window* server_win = m_server_tab->createWindow();
    
    m_server_tab->externalDependant(client_win);
    m_client_tab->externalDependant(server_win);
    
    window->addChildWindow(container);
    container->addChildWindow(client_win);
    container->addChildWindow(server_win);
    
    return window;
}
