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

#ifndef PATCHER_H
#define PATCHER_H

#include <map>
#include <set>

#include "object/Object.h"

struct PatcherEvent {
    Object* src;
    Object* dest;
    int src_socket;
    int dest_socket;
    int socket_type;

    PatcherEvent(Object* s, Object* d, int ss, int ds, int st):
	src(s), dest(d), src_socket(ss), dest_socket(ds), socket_type(st) {};
};

class PatcherListener {
public:
    virtual ~PatcherListener() {};
    virtual void handleLinkAdded(const PatcherEvent& ev) = 0;
    virtual void handleLinkDeleted(const PatcherEvent& ev) = 0;
};

class PatcherSubject {
    std::list<PatcherListener*> m_list;

protected:
    void notifyLinkAdded(const PatcherEvent& ev) {
	for (std::list<PatcherListener*>::iterator it = m_list.begin();
	     it != m_list.end(); )
	    (*it++)->handleLinkAdded(ev);
    };
    
    void notifyLinkDeleted(const PatcherEvent& ev) {
	for (std::list<PatcherListener*>::iterator it = m_list.begin();
	     it != m_list.end(); )
	    (*it++)->handleLinkDeleted(ev);
    };
    
public:
    void addListener(PatcherListener* l) {
	m_list.push_back(l);
    };
    
    void deleteListener(PatcherListener* l) {
	m_list.remove(l);
    };
};

class Patcher : public PatcherSubject
{
public:
    virtual ~Patcher() {};
    
    virtual bool addObject(Object* obj) = 0;
    virtual bool deleteObject(Object* obj) = 0;
    virtual void moveObject(Object* obj) = 0;
    virtual void update() = 0;
    virtual void clear() = 0;
};

#endif /* PATCHER_H */
