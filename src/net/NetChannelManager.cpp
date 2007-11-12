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

#include <list>
#include <cassert>

#include "net/NetChannelManager.h"

using namespace std;

bool NetChannelManager::poll()
{
	if (channels.empty())
		return false;

	enum { MAX_SOCKETS = 256 } ;
	NetSocket* reads [ MAX_SOCKETS+1 ] ;
	NetSocket* writes [ MAX_SOCKETS+1 ] ;
	NetSocket* deletes [ MAX_SOCKETS+1 ] ;
	int nreads = 0 ;
	int nwrites = 0 ;
	int ndeletes = 0 ;
	int nopen = 0 ;
	list<NetChannel*>::iterator it;
	NetChannel* ch;
	
	for (it = channels.begin(); it != channels.end(); it++) {
		if ((*it)->shouldDelete()) {
			assert(ndeletes < MAX_SOCKETS);
			deletes[ndeletes++] = *it ;
		} else if (! (*it)->isClosed()) {
			nopen++ ;
			if ((*it)->readable()) {
				assert(nreads < MAX_SOCKETS);
				reads[nreads++] = (*it);
			}
			if ((*it)->writable()) {
				assert(nwrites < MAX_SOCKETS);
				writes[nwrites++] = (*it);
			}
		}
	}
	
	reads[nreads] = NULL;
	writes[nwrites] = NULL;
	deletes[ndeletes] = NULL;

	int i ;
	for (i = 0; deletes[i]; i++) {
		ch = (NetChannel*)deletes[i];
		delete ch ;
	}

	if (!nopen)
		return false ;
	if (!nreads && !nwrites)
		return true ; //hmmm- should we shutdown?

	NetSocket::select(reads, writes, timeout) ;

	for (i = 0; reads[i]; i++) {
		ch = (NetChannel*)reads[i];
		if (! ch -> isClosed())
			ch -> handleReadEvent();
	}

	for (i = 0; writes[i]; i++) {
		ch = (NetChannel*)writes[i];
		if (! ch ->isClosed())
			ch -> handleWriteEvent();
	}

	return true ;
}
