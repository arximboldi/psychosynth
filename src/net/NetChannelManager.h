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

#ifndef _NETCHANNELMANAGER_H_
#define _NETCHANNELMANAGER_H_


#include "net/NetChannel.h"

class NetChannelManager
{
	std::list<NetChannel*> channels;
	int timeout;
	
public:
	NetChannelManager(int to = 0)
		: timeout(to) {};
	
	int getTimeout() const {
		return timeout;
	};
	
	void setTimeout(int to) {
		timeout = to;
	};
	
	void manage(NetChannel* chan) {
		channels.push_back(chan);
	};
	
	void unmanage(NetChannel* chan) {
		channels.remove(chan);
	};
	
	int numChannels() {
		return channels.size();
	};
	
	bool poll();
};

#endif

