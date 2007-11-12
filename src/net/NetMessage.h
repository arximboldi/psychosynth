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
*   NetMessage - message buffer and channel classes
*
* DESCRIPTION
*   messages are a binary format for sending buffers over a channel.
*   message headers contain a type field and length.
*
* AUTHOR
*   Dave McClurg <dpm@efn.org>
*
* CREATION DATE
*   Dec-2000
****/

#ifndef __NET_MESSAGE__
#define __NET_MESSAGE__


#include "net/NetBuffer.h"

#ifndef UL_MSVC
#include <netinet/in.h> // ntohs() etc prototypes
#endif

#ifdef __FreeBSD__
#include <arpa/inet.h>
#endif


class NetMessage : public NetBuffer
{
	int pos;

public:
	NetMessage(const NetMessage &m) :
		NetBuffer(m),
		pos(0) {}

	NetMessage(int n) :
		NetBuffer(n),
		pos(0)
		{}
	
	NetMessage(const char* s, int n) : 
		NetBuffer(n), 
		pos(0) {
		append(s, n);
	}

	void geta(void* a, int n) const {
		memcpy(a, &data[pos], n) ;
		seek(pos + n);
	}
	
	void puta(const void* a, int n) {
		assert(length + n <= max_length); /* TODO: Dynamic memory? */
		append((const char*)a, n);
		pos = length;
	}

	int getbyte() const {
		unsigned char temp ;
		geta(&temp, sizeof(temp)) ;
		return temp ;
	}
	
	void putbyte(int c) {
		unsigned char temp = c ;
		puta(&temp, sizeof(temp)) ;
	}

	bool getb() const {
		unsigned char temp ;
		geta(&temp, sizeof(temp)) ;
		return temp != 0 ;
	}
	
	void putb(bool b) {
		unsigned char temp = b ? 1 : 0 ;
		puta(&temp, sizeof(temp)) ;
	}

	int getw() const
	{
		unsigned short temp ;
		geta(&temp, sizeof(temp)) ;
		return int(ntohs(temp)) ;
	}
	
	void putw(int i) {
		unsigned short temp = htons((unsigned short) i);
		puta(&temp, sizeof(temp)) ;
	}

	int geti() const {
		unsigned int temp ;
		geta(&temp, sizeof(temp)) ;
		return int(ntohl(temp)) ;
	}
	
	void puti(int i) {
		unsigned int temp = htonl((unsigned int) i) ;
		puta(&temp, sizeof(temp)) ;
	}

	void getfv(float* fv, int n) const {
		unsigned int* v = (unsigned int*)fv;
		geta(v, (n << 2)) ;
		for (int i = 0; i < n; i++)
			v[i] = ntohl(v[i]) ;
	}
	
	void putfv(const float* fv, int n) {
		const unsigned int* v = (const unsigned int*)fv;
		for (int i = 0; i < n; i++) {
			unsigned int temp = htonl(v[i]) ;
			puta(&temp, sizeof(temp)) ;
		}
	}

	float getf() const {
		/** TODO Revisar este código.
		unsigned int temp ;
		temp = ntohl(temp) ;
		return *((float*)(&temp)) ; // cutreeee
		*/
		float temp;
		geta(&temp, sizeof(temp)) ;
		return temp;
	}
	
	void putf(float f) {
		/*unsigned int temp = *((unsigned int*) & f) ;
		temp = htonl(temp) ;
		puta(&temp, sizeof(temp)) ;*/ // Revisaaaar
		puta(&f, sizeof(f));
	}

	void gets(char* s, int n) const {
		char* src = &data[pos];
		char* dst = s;
		while (pos < length) {
			char ch = *src++;
			if ((dst - s) < (n - 1))
				*dst++ = ch ;
			((NetMessage*)this)->pos++;
			if (ch == 0)
				break;
		}
		*dst = 0 ;
	}
	
	void puts(const char* s) {
		puta(s, strlen(s) + 1);
	}

	void seek(int new_pos) const {
		if (new_pos < 0)           new_pos = 0 ;
		else if (new_pos > length) new_pos = length ;

		((NetMessage*)this) -> pos = new_pos; /* Logical constness WTF!? */ 
	}

	void skip(int off) const {
		seek(pos + off);
	}

	void print(FILE *fd = stderr) const {
		fprintf(fd, "NetMessage: %p, length=%d\n", this, length);
		fprintf(fd, "  data = ") ;
		for (int i = 0; i < length; i++)
			fprintf(fd, "%02x ", data[i]) ;
		fprintf(fd, "\n") ;
	}
};


class NetMessageChannel : public NetBufferChannel
{
	virtual void handleBufferRead(NetBuffer& buffer) ;

	public:

	bool sendMessage(const NetMessage& msg) {
		/* TODO: Check all this carefully. */
		unsigned short len = msg.getLength();
		return bufferSend((const char*)&len, sizeof(unsigned short))
			&& bufferSend(msg.getData(), msg.getLength()) ;
	}

	virtual void handleMessage(const NetMessage& msg) {
		WARNING("Network:" << getHandle() << ": Unhandled message");
	};
};


#endif //__NET_MESSAGE__
