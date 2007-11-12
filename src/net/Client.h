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

#ifndef CLIENT_H
#define CLIENT_H

#include "core/Core.h"
#include "core/CoreNetUpdater.h"
#include "common/Thread.h"
#include "net/NetMessage.h"
#include "common/Packets.h"
#include "net/NetChannelManager.h"

class Client : private NetMessageChannel, public CoreListener, public SelfThread {
	CoreNetUpdater m_updater;
	
	NetChannelManager m_manager;
	std::string m_host;
	int m_port;

	int m_next_is_mine;
	
	void processConfig(const NetMessage& msg);
public:
	Client(std::string host = "localhost", int port = DEFAULT_PORT) :
		m_host(host),
		m_port(port),
		m_next_is_mine(0)
		{};
	
	~Client()
		{};
	
	void setCore(Core* core) { m_updater.setCore(core); };
	Core* getCore() const { return m_updater.getCore(); };
		
	void run();
	bool update() { return m_manager.poll(); };
	
	/* NetMessageChannel events */
	virtual void handleMessage(const NetMessage &msg);
	
	/* Core events */
	virtual void handleAddObject(Object* obj);
	virtual void handleMoveObject(Object* obj);
	virtual void handleDeleteObject(Object* obj);
};

#endif
