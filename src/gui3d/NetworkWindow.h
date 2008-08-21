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

#ifndef NETWORKWINDOW_H
#define NETWORKWINDOW_H

#include <libpsynth/net/osc_client.h>
#include <libpsynth/net/osc_server.h>
#include <libpsynth/net/osc_client_logger.h>
#include <libpsynth/net/osc_server_logger.h>

#include "gui3d/GuiLogSink.h"
#include "gui3d/ToggableWindow.h"

class ClientTab : public psynth::osc_client_listener
{
    GuiLogSink m_logsink;
    psynth::osc_client_logger m_logger;
    
    psynth::osc_client* m_client;
    bool m_connected;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Window* m_ext_disable;
    CEGUI::Spinner* m_lport;
    CEGUI::Spinner* m_rport;
    CEGUI::Editbox* m_host;
    
    bool handle_client_accept(psynth::osc_client* client) { return false; }
    bool handle_client_connect(psynth::osc_client* client);
    bool handle_client_disconnect(psynth::osc_client* client,
				  psynth::osc_client_error err);

    void setConnected(bool con);

public:
    ClientTab(psynth::osc_client* client);
    ~ClientTab();
    
    CEGUI::Window* createWindow();    
    bool onButtonClick(const CEGUI::EventArgs &e);

    void externalDependant(CEGUI::Window* win) {
	m_ext_disable = win;
    }
};

class ServerTab : public psynth::osc_server_listener
{
    GuiLogSink m_logsink;
    psynth::osc_server_logger m_logger;
    psynth::osc_server* m_server;
    bool m_listening;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Window* m_ext_disable;
    CEGUI::Spinner* m_lport;

    bool handle_server_start_listening(psynth::osc_server* server);
    bool handle_server_stop_listening(psynth::osc_server* server,
				   psynth::osc_server_error err);
    bool handle_server_client_connect(psynth::osc_server* server,
				   int client_id) { return false; };
    bool handle_server_client_disconnect(psynth::osc_server* server,
				      int client_id,
				      psynth::osc_server_client_error err) { return false; };
    bool handle_server_client_timeout(psynth::osc_server* server,
				   int client_id) { return false; };

    void setListening(bool listening);
    
public:
    ServerTab(psynth::osc_server* server);
    ~ServerTab();
    CEGUI::Window* createWindow();
    
    bool onButtonClick(const CEGUI::EventArgs &e);

    void externalDependant(CEGUI::Window* win) {
	m_ext_disable = win;
    }
};
    
class NetworkWindow : public ToggableWindow
{
    ClientTab* m_client_tab;
    ServerTab* m_server_tab;

    virtual CEGUI::FrameWindow* createWindow();
public:

    NetworkWindow(psynth::osc_client* client, psynth::osc_server* server);
    ~NetworkWindow();
};

#endif /* NETWORKWINDOW_H */
