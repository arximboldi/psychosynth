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

#include "psychosynth.h"
#include "common/Packets.h"
#include "server/Server.h"
#include "net/NetSocket.h"

using namespace std;

void Server::handleAccept()
{
	NetAddress addr;
	
	cout << "accepting connection\n";
	
	SConnection* newcon = new SConnection(this);
	newcon->setHandle( accept(&addr) );
	
	m_connections.push_back(newcon);
	m_manager.manage(newcon);
	
	sendWelcome(newcon);
}

void Server::sendWelcome(SConnection* con)
{
	PacketConfig pconfig();
	
//TODO	con->sendMessage(makePacketMessage(pconfig));
	
	for (Core::Iterator i = m_updater.getCore()->begin();
		 i != m_updater.getCore()->end();
		 i++) {
		
		PacketAddObject paddobj((*i)->getType(), (*i)->getId(),
			(*i)->getX(), (*i)->getY());
		
		con->sendMessage(makePacketMessage(paddobj));
	}
}

void Server::run()
{
	open();
	bind("", m_port);
	listen(5);
	
	m_manager.manage(this);
	
	m_manager.setTimeout(1000);
	while(m_manager.poll());
	
	close();
}

void Server::processMessage(SConnection* reciver, const NetMessage& msg)
{
	if ((unsigned)msg.getLength() < sizeof(unsigned short)) {
		WARNING("Message too short!");
		return;
	}
	
	cout << "Message received:\n";
	msg.print();
	
	unsigned short packet_type = msg.getw();
	
	switch (packet_type) {
		default:
			m_next_is_mine++;
			if (m_updater.processMessage(msg))
				broadcastMessage(reciver, msg);
			else m_next_is_mine--;
			break;
	}
}

void Server::broadcastMessage(SConnection* reciver, const NetMessage& msg)
{
	SConIter i;
	
	for (i = m_connections.begin(); i != m_connections.end(); i++) {
		if (*i != reciver)
			(*i)->sendMessage(msg);
	}
}

Server::~Server()
{
	for (list<SConnection*>::iterator i = m_connections.begin();
		i != m_connections.end(); i++)
		delete *i;
}

void Server::handleAddObject(Object* obj)
{
	if (!m_next_is_mine) {
		PacketAddObject packet(obj->getType(), obj->getId(), obj->getX(), obj->getY());
		broadcastMessage(NULL, makePacketMessage(packet));
	} else m_next_is_mine--;
}

void Server::handleMoveObject(Object* obj)
{
	if (!m_next_is_mine) {
		PacketMoveObject packet(obj->getId(), obj->getX(), obj->getY());
		broadcastMessage(NULL, makePacketMessage(packet));
	} else m_next_is_mine--;
}

void Server::handleDeleteObject(Object* obj)
{	
	if (!m_next_is_mine) {
		PacketDeleteObject packet(obj->getId());
		broadcastMessage(NULL, makePacketMessage(packet));
	} else m_next_is_mine--;
}
