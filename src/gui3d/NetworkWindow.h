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

#include "net/OSCClient.h"
#include "net/OSCServer.h"
#include "net/OSCClientLogger.h"
#include "net/OSCServerLogger.h"

#include "gui3d/GuiLogSink.h"
#include "gui3d/ToggableWindow.h"

class ClientTab : public OSCClientListener
{
    GuiLogSink m_logsink;
    OSCClientLogger m_logger;
    
    OSCClient* m_client;
    bool m_connected;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Spinner* m_lport;
    CEGUI::Spinner* m_rport;
    CEGUI::Editbox* m_host;
    
    bool handleClientAccept(OSCClient* client) { return false; }
    bool handleClientTimeout(OSCClient* client);
    bool handleClientDrop(OSCClient* client);
    bool handleClientConnect(OSCClient* client);
    bool handleClientDisconnect(OSCClient* client);

    void setConnected(bool con);

public:
    ClientTab(OSCClient* client);
    ~ClientTab();
    
    CEGUI::Window* createWindow();

    bool onButtonClick(const CEGUI::EventArgs &e);
};

class ServerTab : public OSCServerListener
{
    GuiLogSink m_logsink;
    OSCServerLogger m_logger;
    
    OSCServer* m_server;
    bool m_listening;

    CEGUI::Window* m_button;
    CEGUI::Window* m_disable;
    CEGUI::Spinner* m_lport;

    bool handleStartListening(OSCServer* server);
    bool handleStopListening(OSCServer* server);
    bool handleClientConnect(OSCServer* server, int client_id) { return false; };
    bool handleClientDisconnect(OSCServer* server, int client_id) { return false; };
    bool handleClientTimeout(OSCServer* server, int client_id) { return false; };

    void setListening(bool listening);
    
public:
    ServerTab(OSCServer* server);
    ~ServerTab();
    CEGUI::Window* createWindow();
    
    bool onButtonClick(const CEGUI::EventArgs &e);
};
    
class NetworkWindow : public ToggableWindow
{
    ClientTab* m_client_tab;
    ServerTab* m_server_tab;

    virtual CEGUI::FrameWindow* createWindow();
public:

    NetworkWindow(OSCClient* client, OSCServer* server);
    ~NetworkWindow();
};

#endif /* NETWORKWINDOW_H */
