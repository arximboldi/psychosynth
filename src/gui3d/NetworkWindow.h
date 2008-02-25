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

#include <libpsynth/net/OSCClient.h>
#include <libpsynth/net/OSCServer.h>
#include <libpsynth/net/OSCClientLogger.h>
#include <libpsynth/net/OSCServerLogger.h>

#include "gui3d/GuiLogSink.h"
#include "gui3d/ToggableWindow.h"

class ClientTab : public psynth::OSCClientListener
{
    GuiLogSink m_logsink;
    psynth::OSCClientLogger m_logger;
    
    psynth::OSCClient* m_client;
    bool m_connected;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Window* m_ext_disable;
    CEGUI::Spinner* m_lport;
    CEGUI::Spinner* m_rport;
    CEGUI::Editbox* m_host;
    
    bool handleClientAccept(psynth::OSCClient* client) { return false; }
    bool handleClientConnect(psynth::OSCClient* client);
    bool handleClientDisconnect(psynth::OSCClient* client,
				psynth::OSCClientError err);

    void setConnected(bool con);

public:
    ClientTab(psynth::OSCClient* client);
    ~ClientTab();
    
    CEGUI::Window* createWindow();    
    bool onButtonClick(const CEGUI::EventArgs &e);

    void externalDependant(CEGUI::Window* win) {
	m_ext_disable = win;
    }
};

class ServerTab : public psynth::OSCServerListener
{
    GuiLogSink m_logsink;
    psynth::OSCServerLogger m_logger;
    psynth::OSCServer* m_server;
    bool m_listening;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Window* m_ext_disable;
    CEGUI::Spinner* m_lport;

    bool handleServerStartListening(psynth::OSCServer* server);
    bool handleServerStopListening(psynth::OSCServer* server,
				   psynth::OSCServerError err);
    bool handleServerClientConnect(psynth::OSCServer* server,
				   int client_id) { return false; };
    bool handleServerClientDisconnect(psynth::OSCServer* server,
				      int client_id,
				      psynth::OSCServerClientError err) { return false; };
    bool handleServerClientTimeout(psynth::OSCServer* server,
				   int client_id) { return false; };

    void setListening(bool listening);
    
public:
    ServerTab(psynth::OSCServer* server);
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

    NetworkWindow(psynth::OSCClient* client, psynth::OSCServer* server);
    ~NetworkWindow();
};

#endif /* NETWORKWINDOW_H */
