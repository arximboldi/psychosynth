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


#include "net/NetSocket.h"

#if defined(UL_CYGWIN) || !defined (UL_WIN32)

#if defined(UL_MAC_OSX)
#  include <netinet/in.h>
#endif

#include <cassert>

#include <cstdarg>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>    /* Need both for Mandrake 8.0!! */
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>

#else

#include <winsock.h>

#endif

#if defined(UL_MSVC) && !defined(socklen_t)
#define socklen_t int
#endif

/* Paul Wiltsey says we need this for Solaris 2.8 */

#ifndef INADDR_NONE
#define INADDR_NONE ((unsigned long)-1)
#endif

NetAddress::NetAddress(const char* host, int port)
{
	set (host, port) ;
}


void NetAddress::set (const char* host, int port)
{
	memset(this, 0, sizeof(NetAddress));

	sin_family = AF_INET ;
	sin_port = htons(port);

	/* Convert a string specifying a host name or one of a few symbolic
	** names to a numeric IP address.  This usually calls gethostbyname()
	** to do the work; the names "" and "<broadcast>" are special.
	*/

	if (host[0] == '\0')
		sin_addr = INADDR_ANY;
	else
		if (host[0] == '<' && strcmp(host, "<broadcast>") == 0)
			sin_addr = INADDR_BROADCAST;
		else {
			sin_addr = inet_addr(host) ;

			if (sin_addr == INADDR_NONE) {
				struct hostent *hp = gethostbyname(host) ;

				if (hp != NULL)
					memcpy((char *) &sin_addr, hp->h_addr, hp->h_length) ;
				else {
					perror("NetAddress::set") ;
					sin_addr = INADDR_ANY ;
				}
			}
		}
}


/* Create a string object representing an IP address.
   This is always a string of the form 'dd.dd.dd.dd' (with variable
   size numbers). */

const char* NetAddress::getHost() const
{
#if 0
	const char* buf = iNet_ntoa(sin_addr) ;
#else
	static char buf [32];
	long x = ntohl(sin_addr);
	sprintf(buf, "%d.%d.%d.%d",
	        (int)(x >> 24) & 0xff, (int)(x >> 16) & 0xff,
	        (int)(x >> 8) & 0xff, (int)(x >> 0) & 0xff);
#endif
	return buf;
}


int NetAddress::getPort() const
{
	return ntohs(sin_port);
}


const char* NetAddress::getLocalHost()
{
	//gethostbyname(gethostname())

	char buf[256];
	memset(buf, 0, sizeof(buf));
	gethostname(buf, sizeof(buf) - 1);
	const hostent *hp = gethostbyname(buf);

	if (hp && *hp->h_addr_list) {
		in_addr     addr = *((in_addr*) * hp->h_addr_list);
		const char* host = inet_ntoa(addr);

		if (host)
			return host ;
	}

	return "127.0.0.1" ;
}


bool NetAddress::getBroadcast() const
{
	return sin_addr == INADDR_BROADCAST;
}


NetSocket::NetSocket()
{
	handle = -1 ;
}


NetSocket::~NetSocket()
{
	close() ;
}


void NetSocket::setHandle(int _handle)
{
	close() ;
	handle = _handle ;
}


bool NetSocket::open(bool stream)
{
	close() ;
	handle = ::socket(AF_INET, (stream ? SOCK_STREAM : SOCK_DGRAM), 0) ;
	return (handle != -1);
}


void NetSocket::setBlocking(bool blocking)
{
	assert(handle != -1) ;

#if defined(UL_CYGWIN) || !defined (UL_WIN32)

	int delay_flag = ::fcntl(handle, F_GETFL, 0);

	if (blocking)
		delay_flag &= (~O_NDELAY);
	else
		delay_flag |= O_NDELAY;

	::fcntl(handle, F_SETFL, delay_flag);

#else

	u_long nblocking = blocking ? 0 : 1;
	::ioctlsocket(handle, FIONBIO, &nblocking);

#endif
}


void NetSocket::setBroadcast(bool broadcast)
{
	assert(handle != -1) ;
	int result;
	if (broadcast) {
		int one = 1;
#ifdef UL_WIN32
		result = ::setsockopt(handle, SOL_SOCKET, SO_BROADCAST, (char*) & one, sizeof(one));
#else
		result = ::setsockopt(handle, SOL_SOCKET, SO_BROADCAST, &one, sizeof(one));
#endif
	} else {
		result = ::setsockopt(handle, SOL_SOCKET, SO_BROADCAST, NULL, 0);
	}
	if (result < 0) {
		perror("set broadcast:");
	}
	assert(result != -1);
}


int NetSocket::bind(const char* host, int port)
{
	assert(handle != -1) ;
	NetAddress addr(host, port) ;
	return ::bind(handle, (const sockaddr*)&addr, sizeof(NetAddress));
}


int NetSocket::listen(int backlog)
{
	assert(handle != -1) ;
	return ::listen(handle, backlog);
}


int NetSocket::accept(NetAddress* addr)
{
	assert(handle != -1) ;

	if (addr == NULL) {
		return ::accept(handle, NULL, NULL);
	} else {
		socklen_t addr_len = (socklen_t) sizeof(NetAddress) ;
		return ::accept(handle, (sockaddr*)addr, &addr_len);
	}
}


int NetSocket::connect(const char* host, int port)
{
	assert(handle != -1) ;
	NetAddress addr(host, port) ;
	if (addr.getBroadcast()) {
		setBroadcast(true);
	}
	return ::connect(handle, (const sockaddr*)&addr, sizeof(NetAddress));
}


int NetSocket::send(const void * buffer, int size, int flags)
{
	assert(handle != -1) ;
	return ::send(handle, (const char*)buffer, size, flags);
}


int NetSocket::sendto(const void * buffer, int size,
                      int flags, const NetAddress* to)
{
	assert(handle != -1) ;
	return ::sendto(handle, (const char*)buffer, size, flags,
	                (const sockaddr*)to, sizeof(NetAddress));
}


int NetSocket::recv(void * buffer, int size, int flags)
{
	assert(handle != -1) ;
	return ::recv(handle, (char*)buffer, size, flags);
}


int NetSocket::recvfrom(void * buffer, int size,
                        int flags, NetAddress* from)
{
	assert(handle != -1) ;
	socklen_t fromlen = (socklen_t) sizeof(NetAddress) ;
	return ::recvfrom(handle, (char*)buffer, size, flags, (sockaddr*)from, &fromlen);
}


void NetSocket::close(void)
{
	if (handle != -1) {
#if defined(UL_CYGWIN) || !defined (UL_WIN32)
		::close(handle);
#else
		::closesocket(handle);
#endif
		handle = -1 ;
	}
}


bool NetSocket::isNonBlockingError()
{
#if defined(UL_CYGWIN) || !defined (UL_WIN32)
	switch (errno) {
		case EWOULDBLOCK: // always == NET_EAGAIN?
		case EALREADY:
		case EINPROGRESS:
			return true;
	}
	return false;
#else
	int wsa_errno = WSAGetLastError();
	if (wsa_errno != 0) {
		WSASetLastError(0);
		ulSetError(UL_WARNING, "WSAGetLastError() => %d", wsa_errno);
		switch (wsa_errno) {
			case WSAEWOULDBLOCK: // always == NET_EAGAIN?
			case WSAEALREADY:
			case WSAEINPROGRESS:
				return true;
		}
	}
	return false;
#endif
}


int NetSocket::select(NetSocket** reads, NetSocket** writes, int timeout)
{
	fd_set r, w;

	FD_ZERO(&r);
	FD_ZERO(&w);

	int i, k ;
	int num = 0 ;

	for (i = 0; reads[i]; i++) {
		int fd = reads[i]->getHandle();
		FD_SET(fd, &r);
		num++;
	}

	for (i = 0; writes[i]; i++) {
		int fd = writes[i]->getHandle();
		FD_SET(fd, &w);
		num++;
	}

	if (!num)
		return num ;

	/* Set up the timeout */
	struct timeval tv ;
	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	// It bothers me that select()'s first argument does not appear to
	// work as advertised... [it hangs like this if called with
	// anything less than FD_SETSIZE, which seems wasteful?]

	// Note: we ignore the 'exception' fd_set - I have never had a
	// need to use it.  The name is somewhat misleading - the only
	// thing I have ever seen it used for is to detect urgent data -
	// which is an unportable feature anyway.

	::select(FD_SETSIZE, &r, &w, 0, &tv);

	//remove sockets that had no activity

	num = 0 ;

	for (k = i = 0; reads[i]; i++) {
		int fd = reads[i]->getHandle();
		if (FD_ISSET(fd, &r)) {
			reads[k++] = reads[i];
			num++;
		}
	}
	reads[k] = NULL ;

	for (k = i = 0; writes[i]; i++) {
		int fd = writes[i]->getHandle();
		if (FD_ISSET(fd, &w)) {
			writes[k++] = writes[i];
			num++;
		}
	}
	writes[k] = NULL ;

	return num ;
}


/* Init/Exit functions */

static void NetExit(void)
{
#if defined(UL_CYGWIN) || !defined (UL_WIN32)
#else
	/* Clean up windows Networking */
	if (WSACleanup() == SOCKET_ERROR) {
		if (WSAGetLastError() == WSAEINPROGRESS) {
			WSACancelBlockingCall();
			WSACleanup();
		}
	}
#endif
}


int NetInit(int* argc, char** argv)
{
	/* Legacy */

	return NetInit() ;
}


int NetInit()
{
	assert(sizeof(sockaddr_in) == sizeof(NetAddress)) ;

#if defined(UL_CYGWIN) || !defined (UL_WIN32)
#else
	/* Start up the windows Networking */
	WORD version_wanted = MAKEWORD(1, 1);
	WSADATA wsaData;

	if (WSAStartup(version_wanted, &wsaData) != 0) {
		ulSetError(UL_WARNING, "Couldn't initialize Winsock 1.1");
		return(-1);
	}
#endif

	atexit(NetExit) ;
	return(0);
}


const char* NetFormat(const char* format, ...)
{
	static char buffer[ 256 ];
	va_list argptr;
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);
	return(buffer);
}


