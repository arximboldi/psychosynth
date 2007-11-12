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

#include <iostream>
#include <cmath>

#include "client/Client.h"
#include "output/OutputAlsa.h"
#include "output/OutputOss.h"
#include "object/ObjectOscillator.h"
#include "net/NetChannelManager.h"

using namespace std;

void Client::handleMessage(const NetMessage &msg)
{
	if ((unsigned)msg.getLength() < sizeof(unsigned short)) {
		WARNING("Message too short!");
		return;
	}
	
	DEBUG("NEW MESSAGE");
	msg.print();
	
	unsigned short packet_type = msg.getw();
	switch (packet_type) {
		case PACKET_CONFIG:
			processConfig(msg); 
			break;
		default: 
			m_next_is_mine++;
			if (!m_updater.processMessage(msg)) 
				m_next_is_mine--;
			break;
	}
}

void Client::processConfig(const NetMessage& msg)
{
	PacketConfig packet;
	msg.geta(&packet, sizeof(packet));
	/* TODO */
}

void Client::run()
{
	open();
	connect(m_host.c_str(), m_port);
	m_manager.manage(this);
	
	m_manager.setTimeout(1000);

	while(m_manager.poll());

	close();
}

void Client::handleAddObject(Object* obj)
{
	if (!m_next_is_mine) {
		PacketAddObject packet(obj->getType(), obj->getId(), obj->getX(), obj->getY());
		sendMessage(makePacketMessage(packet));
	} else m_next_is_mine--;
}

void Client::handleMoveObject(Object* obj)
{
	if (!m_next_is_mine) {
		PacketMoveObject packet(obj->getId(), obj->getX(), obj->getY());
		sendMessage(makePacketMessage(packet));
	} else m_next_is_mine--;
}

void Client::handleDeleteObject(Object* obj)
{
	if (!m_next_is_mine) {
		PacketDeleteObject packet(obj->getId());
		sendMessage(makePacketMessage(packet));
	} else m_next_is_mine--;
}
