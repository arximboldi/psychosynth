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


// TODO:
// have all socket-related functions assert that the socket has not
// been closed.  [a read event may close it, and a write event may try
// to write or something...]
// Maybe assert valid handle, too?

#include <cassert>

#include "net/NetChannel.h"

NetChannel::NetChannel()
{
	closed = true ;
	connected = false ;
	accepting = false ;
	write_blocked = false ;
	should_delete = false ;
}

NetChannel::~NetChannel()
{
	close();
}

void
NetChannel::setHandle(int handle, bool is_connected)
{
	close() ;
	NetSocket::setHandle(handle) ;
	connected = is_connected ;
	//if ( connected ) this->handleConnect();
	closed = false ;
}

bool
NetChannel::open(void)
{
	close();
	if (NetSocket::open(true)) {
		closed = false ;
		setBlocking(false) ;
		return true ;
	}
	return false ;
}

int
NetChannel::listen(int backlog)
{
	accepting = true ;
	return NetSocket::listen(backlog) ;
}

int
NetChannel::connect(const char* host, int port)
{
	int result = NetSocket::connect(host, port);
	
	if (result == 0) {
		connected = true ;
		//this->handleConnect();
		return 0;
	} else if (isNonBlockingError()) { /* TODO: Correctly detect connection */
		return 0;
	} else {
		// some other error condition
		this->handleError(result);
		close();
		return -1;
	}
}

int
NetChannel::send(const void * buffer, int size, int flags)
{
	int result = NetSocket::send(buffer, size, flags);

	if (result == (int)size) {
		// everything was sent
		write_blocked = false ;
		return result;
	} else if (result >= 0) {
		// not all of it was sent, but no error
		write_blocked = true ;
		return result;
	} else if (isNonBlockingError()) {
		write_blocked = true ;
		return 0;
	} else {
		this->handleError(result);
		close();
		return -1;
	}

}

int
NetChannel::recv(void * buffer, int size, int flags)
{
	int result = NetSocket::recv(buffer, size, flags);

	if (result > 0) {
		return result;
	} else if (result == 0) {
		close();
		return 0;
	} else if (isNonBlockingError()) {
		return 0;
	} else {
		this->handleError(result);
		close();
		return -1;
	}
}

void
NetChannel::close(void)
{
	if (!closed) {
		this->handleClose();

		closed = true ;
		connected = false ;
		accepting = false ;
		write_blocked = false ;
	}

	NetSocket::close() ;
}

void
NetChannel::handleReadEvent(void)
{
	if (accepting) {
		if (!connected) {
			connected = true ;
			//this->handleConnect();
		}
		this->handleAccept();
	} else if (!connected) {
		connected = true ;
		//this->handleConnect();
		this->handleRead();
	} else {
		this->handleRead();
	}
}

void
NetChannel::handleWriteEvent(void)
{
	if (!connected) {
		connected = true ;
		//this->handleConnect();
	}
	write_blocked = false ;
	this->handleWrite();
}
