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

#include <psynth/net/osc_client.hpp>
#include <psynth/net/osc_server.hpp>
#include <psynth/net/osc_client_logger.hpp>
#include <psynth/net/osc_server_logger.hpp>

#include "gui3d/gui_log_sink.hpp"
#include "gui3d/toggable_window.hpp"

class client_tab : public psynth::osc_client_listener
{
    gui_log_sink m_logsink;
    psynth::osc_client_logger m_logger;
    
    psynth::osc_client* m_client;
    bool m_connected;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Window* m_ext_disable;
    CEGUI::Spinner* m_lport;
    CEGUI::Spinner* m_rport;
    CEGUI::Editbox* m_host;
    
    bool handle_client_accept (psynth::osc_client* client) { return false; }
    bool handle_client_connect (psynth::osc_client* client);
    bool handle_client_disconnect (psynth::osc_client* client,
				  psynth::osc_client_error err);

    void set_connected (bool con);

public:
    client_tab(psynth::osc_client* client);
    ~client_tab();
    
    CEGUI::Window* create_window ();    
    bool on_button_click (const CEGUI::EventArgs &e);

    void external_dependant (CEGUI::Window* win) {
	m_ext_disable = win;
    }
};

class server_tab : public psynth::osc_server_listener
{
    gui_log_sink m_logsink;
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

    void set_listening (bool listening);
    
public:
    server_tab (psynth::osc_server* server);
    ~server_tab ();
    CEGUI::Window* create_window ();
    
    bool on_button_click (const CEGUI::EventArgs &e);

    void external_dependant (CEGUI::Window* win) {
	m_ext_disable = win;
    }
};
    
class network_window : public toggable_window
{
    client_tab* m_client_tab;
    server_tab* m_server_tab;

    virtual CEGUI::FrameWindow* create_window ();
public:

    network_window (psynth::osc_client* client, psynth::osc_server* server);
    ~network_window ();
};

#endif /* NETWORKWINDOW_H */
