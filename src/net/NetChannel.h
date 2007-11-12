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
*   NetChannel - Network channel class
*
* DESCRIPTION
*   NetChannel is adds event-handling to the low-level
*   NetSocket class.  Otherwise, it can be treated as
*   a normal non-blocking socket object.
*
*   The direct interface between the NetPoll() loop and
*   the channel object are the handleReadEvent and
*   handleWriteEvent methods. These are called
*   whenever a channel object 'fires' that event.
*
*   The firing of these low-level events can tell us whether
*   certain higher-level events have taken place, depending on
*   the timing and state of the connection.
*
* AUTHORS
*   Sam Rushing <rushing@nightmare.com> - original version for Medusa
*   Dave McClurg <dpm@efn.org> - modified for use in PLIB
*
* CREATION DATE
*   Dec-2000
*
****/

#ifndef NET_CHANNEL_H
#define NET_CHANNEL_H

#include "net/NetSocket.h"
#include "common/Error.h"

class NetChannel : public NetSocket
{
	bool closed, 
		 connected,
		 accepting,
		 write_blocked,
		 should_delete;

	friend bool NetPoll(unsigned int timeout);

public:
	NetChannel() ;
	virtual ~NetChannel() ;

	void setHandle(int s, bool is_connected = true);
	bool isConnected() const { return connected; }
	bool isClosed() const { return closed; }
	void setShouldDelete() { should_delete = true ; }
	bool shouldDelete() { return should_delete; }
	
	// --------------------------------------------------
	// socket methods
	// --------------------------------------------------

	bool  open(void) ;
	void  close(void) ;
	int   listen(int backlog) ;
	int   connect(const char* host, int port) ;
	int   send(const void * buf, int size, int flags = 0) ;
	int   recv(void * buf, int size, int flags = 0) ;

	// poll() eligibility predicates
	virtual bool readable(void) { return (connected || accepting); }
	virtual bool writable(void) { return (!connected || write_blocked); }

	// --------------------------------------------------
	// event handlers
	// --------------------------------------------------

	void handleReadEvent(void);
	void handleWriteEvent(void);

	// These are meant to be overridden.
	virtual void handleClose(void) {
		//ulSetError(UL_WARNING,"Network: %d: unhandled close",getHandle());
	}
	
	virtual void handleRead(void) {
		WARNING("Network:" << getHandle() << ": Unhandled read");
	}
	
	virtual void handleWrite(void) {
		WARNING("Network:" << getHandle() << ": Unhandled write");
	}
	
	virtual void handleAccept(void) {
		WARNING("Network:" << getHandle() << ": Unhandled accept");
	}
	
	virtual void handleError(int error) {
		WARNING("Network:" << getHandle() << ": Error " << errno);
	}
};

#endif // NET_CHANNEL_H
