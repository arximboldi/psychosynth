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

/***************************************************************************
 *                                                                         *
 *   Some of the code in this file was taken from plib.                    *
 *                                                                         *
 *   PLIB                                                                  *
 *   ====                                                                  *
 *                                                                         *    
 *   Copyright (C) 1998,2002  Steve Baker                                  *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Library General Public License for more details.                      *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA              *
 *   02111-1307 USA                                                        *
 *                                                                         *
 *   For further information visit http://plib.sourceforge.net             *
 *                                                                         *
 ***************************************************************************/


/****
* NAME
*   NetSocket - Network sockets
*
* DESCRIPTION
*   NetSocket is a thin C++ wrapper over bsd sockets to
*   facilitate porting to other platforms
*
* AUTHOR
*   Dave McClurg <dpm@efn.org>
*
* CREATION DATE
*   Dec-2000
*
****/

#ifndef NET_SOCKET_H
#define NET_SOCKET_H


#include "common/Error.h"
#include <errno.h>

/*
 * Socket address, internet style.
 */
class NetAddress
{
	/* DANGER!!!  This MUST match 'struct sockaddr_in' exactly! */
	short          sin_family     ;
	unsigned short sin_port       ;
	unsigned int   sin_addr       ;
	char           sin_zero [ 8 ] ;

public:
	NetAddress() {}
	NetAddress(const char* host, int port) ;

	void set (const char* host, int port) ;
	const char* getHost() const ;
	int getPort() const ;

	static const char* getLocalHost() ;

	bool getBroadcast() const ;
};


/*
 * Socket type
 */
class NetSocket
{
	int handle ;

public:

	NetSocket() ;
	virtual ~NetSocket() ;

	int getHandle() const { return handle; }
	void setHandle(int handle) ;

	bool  open(bool stream = true) ;
	void  close(void) ;
	int   bind(const char* host, int port) ;
	int   listen(int backlog) ;
	int   accept(NetAddress* addr) ;
	int   connect(const char* host, int port) ;
	int   send(const void * buffer, int size, int flags = 0) ;
	int   sendto(const void * buffer, int size, int flags, const NetAddress* to) ;
	int   recv(void * buffer, int size, int flags = 0) ;
	int   recvfrom(void * buffer, int size, int flags, NetAddress* from) ;

	void setBlocking(bool blocking) ;
	void setBroadcast(bool broadcast) ;

	static bool isNonBlockingError() ;
	static int select(NetSocket** reads, NetSocket** writes, int timeout) ;
} ;


int NetInit(int* argc, char** argv = NULL) ;  /* Legacy */

int NetInit() ;


const char* NetFormat(const char* fmt, ...) ;


#endif // NET_SOCKET_H

