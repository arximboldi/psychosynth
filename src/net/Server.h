/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef __SERVER_H__
#define __SERVER_H__

#include <list>

#include "net/NetMessage.h"
#include "net/NetChannelManager.h"
#include "common/Thread.h"
#include "core/Core.h"
#include "core/CoreNetUpdater.h"
#include "server/SConnection.h"

class Server : private NetMessageChannel, public CoreListener, public SelfThread {
	friend class SConnection;

	typedef std::list<SConnection*> SConList;
	typedef std::list<SConnection*>::iterator SConIter;
	
	CoreNetUpdater m_updater;
	
	int m_port;
	NetChannelManager m_manager;
	SConList m_connections;
	
	int m_next_is_mine;
	
	void sendWelcome(SConnection* con);

	void broadcastMessage(SConnection* reciver, const NetMessage& msg);
	void processMessage(SConnection* reciver, const NetMessage& msg);

public:
	Server(int port = DEFAULT_PORT) :
		m_port(port),
		m_next_is_mine(0) {};
	
	~Server();

	void setCore(Core* core) { m_updater.setCore(core); }
	Core* getCore() const { return m_updater.getCore(); }
	
	virtual void run();
	
	/* NetMessageChannel events */
	virtual void handleAccept();

	/* Core events */
	virtual void handleAddObject(Object* obj);
	virtual void handleMoveObject(Object* obj);
	virtual void handleDeleteObject(Object* obj);
};

#endif
