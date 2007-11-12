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

#ifndef CORENETUPDATER_H
#define CORENETUPDATER_H

#include "core/Core.h"
#include "common/Packets.h"

class CoreNetUpdater
{
	Core* m_core;

	bool processAddObject(const NetMessage& msg);
	bool processMoveObject(const NetMessage& msg);
	bool processDeleteObject(const NetMessage& msg);
public:
	CoreNetUpdater(Core* core = NULL) : m_core(core) {}
	
	void setCore(Core* core) { m_core = core; }
	Core* getCore() const { return m_core; }
	
	bool processMessage(const NetMessage& msg);
};

#endif /* CORENETUPDATER_H */

