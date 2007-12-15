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

#include "OSCServer.h"

using namespace std;

void OSCServerSubject::notifyClientDisconnect(OSCServer* server, int c_id)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientDisconnect(server, c_id);
}

void OSCServerSubject::notifyClientConnect(OSCServer* server, int c_id)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientConnect(server, c_id);
}

void OSCServerSubject::notifyClientTimeout(OSCServer* server, int c_id)
{
    for (list<OSCServerListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientTimeout(server, c_id);
}

OSCServer::OSCServer()
{
}

OSCServer::~OSCServer()
{
}
    
bool OSCServer::listen(const char* port)
{
    return true;
}

void OSCServer::stop()
{
}

int OSCServer::update(int msec)
{
    return 0;
}

int OSCServer::_alive_cb(const char* path, const char* types,
			  lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}

int OSCServer::_connect_cb(const char* path, const char* types,
			    lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}

int OSCServer::_get_state_cb(const char* path, const char* types,
			      lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}

int OSCServer::_disconnect_cb(const char* path, const char* types,
			       lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}
