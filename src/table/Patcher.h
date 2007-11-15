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
#include "object/ObjectOutput.h"
#include "object/ObjectMixer.h"
#include "object/ObjectOscillator.h"
    
const int PATCHER_ANY  = -1;

struct PatcherData {
    int socket_type;
    int src_socket;
    int dest_socket;
};

/*
 * Each object has several inputs and output sockets. On a dinamically patched synth we only
 * want one output connected with one input. This table tells the dinamic patcher which
 * output should be connected with which inpunt on each object.
 * The first dimension is the source object type and the second is the destiny object type.
 */
const PatcherData PATCHER_TABLE[N_OBJECTS][N_OBJECTS] =
{
    /* ObjectOutput */
    {
	{Object::LINK_NONE, 0, 0}, /* ObjectOutput */
	{Object::LINK_NONE, 0, 0}, /* ObjectMixer */
	{Object::LINK_NONE, 0, 0}  /* ObjectOscillator */
    },

    /* ObjectMixer */
    {
	{Object::LINK_AUDIO, ObjectMixer::OUT_A_OUTPUT, ObjectOutput::IN_A_INPUT},  /* ObjectOutput */
	{Object::LINK_AUDIO, ObjectMixer::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectMixer */
	{Object::LINK_NONE, 0, 0} /* ObjectOscillator */
    },

    /* ObjectOscillator */
    {
	{Object::LINK_NONE, 0, 0}, /* ObjectOutput */
	{Object::LINK_AUDIO, ObjectMixer::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectMixer */
	{Object::LINK_NONE, 0, 0} /* ObjectOscillator */
    }
};

class PatcherListener {
public:
    virtual void handleLinkAdded() = 0;
    virtual void handleLinkDeleted() = 0;
};

class PatcherSubject {
public:
    
};

class Patcher
{
    struct Link {
	Object* src;
	Object* dest;
	float dist;
	int sock_type;
	int out_sock;
	int in_sock;
	int actual_in_sock;
	
	Link(Object* s, Object* d, float ds, int t, int os, int is) :
	    src(s), dest(d), dist(ds),
	    sock_type(t), out_sock(os), in_sock(is), actual_in_sock(-1)
	    {}
	
	bool operator< (const Link& l) const {
	    return dist < l.dist;
	}
    };
    
    struct Node {
	Object* obj;
	Object* dest;
	bool out_used; /* We output to one object only */
	int actual_sock_type;
	int actual_in_sock; 

	Node(Object* o) :
	    obj(o),
	    dest(NULL),
	    out_used(false),
	    actual_sock_type(-1),
	    actual_in_sock(-1) {}
    };

    class LinkPtrCmp {
    public:
	bool operator() (const Link* a, const Link* b) {
	  return *a < *b;
	}
    };

    bool m_changed;
    
    std::map<int, Node> m_nodes;
    std::multiset<Link*, LinkPtrCmp> m_links;

    inline void undoLink(Link& l);
    inline void makeLink(Link& l);
    inline void findInSock(Link& l);
    inline bool isLinked(Link& l);
    
public:
    Patcher();
    ~Patcher();
    
    bool addObject(Object* obj);
    bool deleteObject(Object* obj);
    void moveObject(Object* obj);
    void update();
};

#endif /* PATCHER_H */
