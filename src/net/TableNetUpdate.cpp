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

#include "core/CoreNetUpdater.h"
#include "common/Error.h"
#include "object/Object.h"
#include "object/ObjectOscillator.h"

static Object* createObject(int type)
{
    switch (type) {
    case OBJ_OSCILLATOR:  return new ObjectOscillator(AudioInfo()); /* TODO!! */
    default:              return NULL;
    }
}

bool CoreNetUpdater::processMessage(const NetMessage& msg)
{
    msg.seek(0);
	
    unsigned short packet_type = msg.getw();
    switch (packet_type) {
    case PACKET_ADDOBJECT:    return processAddObject(msg);
    case PACKET_MOVEOBJECT:   return processMoveObject(msg);
    case PACKET_DELETEOBJECT: return processDeleteObject(msg);
    default:                  WARNING("Unknwon message"); return false;
    }
}

bool CoreNetUpdater::processAddObject(const NetMessage& msg)
{
    PacketAddObject packet;
    msg.geta(&packet, sizeof(packet));
    return m_core->addObject(createObject(packet.type), packet.id, packet.x, packet.y);
}

bool CoreNetUpdater::processMoveObject(const NetMessage& msg)
{
    PacketMoveObject packet;
    msg.geta(&packet, sizeof(packet));
    m_core->moveObject(m_core->findObject(packet.id), packet.x, packet.y);
    return true;
}

bool CoreNetUpdater::processDeleteObject(const NetMessage& msg)
{
    PacketDeleteObject packet;
    msg.geta(&packet, sizeof(packet));
    m_core->deleteObject(m_core->findObject(packet.id));
    return true;
}
