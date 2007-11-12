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

#ifndef PACKETS_H
#define PACKETS_H

#include "net/NetMessage.h"

enum PacketType {
	PACKET_CONFIG,
	PACKET_ADDOBJECT,
	PACKET_MOVEOBJECT,
	PACKET_DELETEOBJECT
};

struct PacketConfig {
	
	PacketConfig() {}
	
	PacketType getType() const {return PACKET_CONFIG;}
};

struct PacketAddObject {
	unsigned short type;
	unsigned short id;
	float x;
	float y;
	
	PacketAddObject() {};
	PacketAddObject(unsigned short type_, unsigned short id_, 
					float x_, float y_) :
		type(type_), id(id_), x(x_), y(y_) {};
	
	PacketType getType() const {return PACKET_ADDOBJECT;}
};

struct PacketMoveObject {
	unsigned short id;
	float x;
	float y;
	
	PacketMoveObject() {};
	PacketMoveObject(short int id_, float x_, float y_) :
		id(id_), x(x_), y(y_) {};

	PacketType getType() const {return PACKET_MOVEOBJECT;}
};

struct PacketDeleteObject {
	unsigned short id;
	
	PacketDeleteObject() {};
	PacketDeleteObject(short int id_) : id(id_) {};

	PacketType getType() const {return PACKET_DELETEOBJECT;}
};

template<class SomePacket>
inline NetMessage makePacketMessage(const SomePacket packet)
{
	NetMessage msg(sizeof(unsigned short) + sizeof(packet));
	
	msg.putw(packet.getType());
	msg.puta(&packet, sizeof(packet));
	
	return msg;
};

#endif /* PACKETS_H */
