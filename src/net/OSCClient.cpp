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

#include "net/OSCClient.h"

using namespace std;

void OSCClientSubject::notifyClientAccept(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientAccept(param);
}

void OSCClientSubject::notifyClientTimeout(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientTimeout(param);
}

void OSCClientSubject::notifyClientDrop(OSCClient* param)
{
    for (list<OSCClientListener*>::iterator it = m_list.begin();
	 it != m_list.end(); ++it)
	(*it)->handleClientDrop(param);
}

OSCClient::OSCClient() :
    OSCController(false),
    m_server(NULL)
{
}

OSCClient::~OSCClient()
{
}

void OSCClient::connect(lo_address target)
{
}

void OSCClient::disconnect()
{
}

bool OSCClient::isConnected()
{
}

int OSCClient::update(int msec)
{
}

int OSCClient::_drop_cb(const char* path, const char* types,
			lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}

int OSCClient::_accepted_cb(const char* path, const char* types,
			    lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}

int OSCClient::_alive_cb(const char* path, const char* types,
			 lo_arg** argv, int argc, lo_message msg)
{
    return 0;
}
