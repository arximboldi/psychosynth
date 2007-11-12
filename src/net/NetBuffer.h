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
 *   Most of the code in this file was taken from plib.                    *
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
*   NetBuffer - Network buffer class
*
* DESCRIPTION
*   Clients and servers built on top of NetBufferChannel
*   automatically support pipelining.
*
*   Pipelining refers to a protocol capability. Normally,
*   a conversation with a server has a back-and-forth
*   quality to it.  The client sends a command, and
*   waits for the response. If a client needs to send
*   many commands over a high-latency connection,
*   waiting for each response can take a long time.
*
*   For example, when sending a mail message to many recipients
*   with SMTP, the client will send a series of RCPT commands, one
*   for each recipient. For each of these commands, the server will
*   send back a reply indicating whether the mailbox specified is
*   valid. If you want to send a message to several hundred recipients,
*   this can be rather tedious if the round-trip time for each command
*   is long. You'd like to be able to send a bunch of RCPT commands
*   in one batch, and then count off the responses to them as they come.
*
*   I have a favorite visual when explaining the advantages of
*   pipelining. Imagine each request to the server is a boxcar on a train.
*   The client is in Los Angeles, and the server is in New York.
*   Pipelining lets you hook all your cars in one long chain; send
*   them to New York, where they are filled and sent back to you.
*   Without pipelining you have to send one car at a time.
*
*   Not all protocols allow pipelining. Not all servers support it;
*   Sendmail, for example, does not support pipelining because it tends
*   to fork unpredictably, leaving buffered data in a questionable state.
*   A recent extension to the SMTP protocol allows a server to specify
*   whether it supports pipelining. HTTP/1.1 explicitly requires that
*   a server support pipelining.
*
* NOTES
*   When a user passes in a buffer object, it belongs to
*   the user.  When the library gives a buffer to the user,
*   the user should copy it.
*
* AUTHORS
*   Sam Rushing <rushing@nightmare.com> - original version for Medusa
*   Dave McClurg <dpm@efn.org> - modified for use in PLIB
*
* CREATION DATE
*   Dec-2000
*
****/

#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include <cassert>

#include "net/NetChannel.h"


class NetBuffer
{
protected:
	int length ;
	int max_length ;
	char* data ;

public:
	NetBuffer(int _max_length):
		length(0),
		max_length(_max_length) {
		data = new char [ max_length+1 ] ;  //for null terminator
	}

	~NetBuffer() {
		delete[] data ;
	}

	NetBuffer(const NetBuffer& b):
		length(b.length),
		max_length(b.max_length) {
		data = new char [ max_length+1 ];
	}

	NetBuffer& operator= (const NetBuffer& b) {
		if (&b != this) {
			length = b.length;
			max_length = b.max_length;
			delete [] data;
			data = new char [max_length + 1];
		}
		return *this;
	}

	int getLength() const { return length ; }
	
	int getMaxLength() const { return max_length ; }

	char* getData() { data[length] = 0; return data; }
	
	const char* getData() const {((char*)data)[length] = 0; return data; }

	void remove() {
		length = 0 ;
	}

	void remove(int pos, int n) { /* TODO: Circular buffer? */
		assert(pos >= 0 && pos < length && (pos + n) <= length) ;
		
		memmove(&data[pos], &data[pos+n], length - (pos + n)) ;
		length -= n ;
	}

	bool append(const char* s, int n) {
		if ((length + n) <= max_length) { /* TODO: Make this an assert? */
			memcpy(&data[length], s, n) ;
			length += n ;
			return true ;
		}
		return false ;
	}

	bool append(int n)
	{
		if ((length + n) <= max_length) {
			length += n ;
			return true ;
		}
		
		return false ;
	}
};

class NetBufferChannel : public NetChannel
{
	NetBuffer in_buffer;
	NetBuffer out_buffer;
	int should_close ;

	virtual void handleRead(void) ;

	virtual void handleWrite(void) ;

public:
	NetBufferChannel(int in_buffer_size = 4096, int out_buffer_size = 16384) :
			in_buffer(in_buffer_size),
			out_buffer(out_buffer_size),
			should_close(0)
	{
	}

	virtual bool readable(void) {
		return (NetChannel::readable() &&
				(in_buffer.getLength() < in_buffer.getMaxLength()));
	}
	
	virtual bool writable(void) {
		return (NetChannel::writable() || (out_buffer.getLength() || should_close));
	}

	virtual void handleClose(void) {
		in_buffer.remove() ;
		out_buffer.remove() ;
		should_close = 0 ;
		NetChannel::handleClose() ;
	}

	void closeWhenDone(void) { should_close = 1 ; }

	virtual bool bufferSend(const char* msg, int msg_len) {
		if (out_buffer.append(msg, msg_len))
			return true ;
		WARNING("Output buffer overflow.");
		return false ;
	}

	virtual void handleBufferRead(NetBuffer& buffer) {
		/* do something here */
		buffer.remove();
	}
};

#endif // NET_BUFFER_H

