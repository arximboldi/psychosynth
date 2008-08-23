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

#include <algorithm>
#include "table/PatcherDynamic.h"

#include "object/node.h"
#include "object/ObjectOutput.h"
#include "object/ObjectAudioMixer.h"
#include "object/ObjectControlMixer.h"
#include "object/ObjectAudioOscillator.h"
#include "object/ObjectLFO.h"
#include "object/ObjectFilter.h"
#include "object/ObjectSampler.h"
#include "object/ObjectStepSeq.h"
#include "object/ObjectAudioNoise.h"
#include "object/ObjectControlNoise.h"
#include "object/ObjectEcho.h"
#include "object/ObjectDelay.h"

using namespace std;

namespace psynth
{

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
	{node::LINK_NONE, 0, 0},  /* ObjectOutput */
	{node::LINK_NONE, 0, 0},  /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0},  /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0},  /* ObjectLFO */
	{node::LINK_NONE, 0, 0},   /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */	
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectEcho */
	{node::LINK_NONE, 0, 0} /* ObjectDelay */
    },

    /* ObjectAudioMixer */
    {
	{node::LINK_AUDIO, ObjectAudioMixer::OUT_A_OUTPUT, ObjectOutput::IN_A_INPUT},  /* ObjectOutput */
	{node::LINK_AUDIO, ObjectAudioMixer::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectAudioMixer::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectAudioMixer::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectAudioMixer::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT} /* ObjectDelay */
    },

    /* ObjectControlMixer */
    {
	{node::LINK_NONE, 0, 0},  /* ObjectOutput */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectMixer::IN_C_AMPLITUDE}, /* ObjectAudioMixer */
	/* TODO: Decide a way to allow controlling control signals amplitude. */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, PATCHER_ANY}, /* ObjectControlMixer */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectOscillator::IN_C_FREQUENCY},/* ObjectAudioOscillator */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectOscillator::IN_C_FREQUENCY}, /* ObjectLFO */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectFilter::IN_C_CUTOFF}, /* ObjectFilter */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectSampler::IN_C_RATE}, /* ObjectSampler */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectStepSeq::IN_C_BPM}, /* ObjectStepSeq */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectAudioNoise::IN_C_AMPLITUDE}, /* ObjectAudioNoise */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectControlNoise::IN_C_AMPLITUDE}, /* ObjectControlNoise */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectEcho::IN_C_FEEDBACK}, /* ObjectEcho */
	{node::LINK_CONTROL, ObjectControlMixer::OUT_C_OUTPUT, ObjectEcho::IN_C_DELAY} /* ObjectDelay */
    },
    
    /* ObjectAudioOscillator */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_AUDIO, ObjectAudioOscillator::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectAudioOscillator::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectAudioOscillator::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectAudioOscillator::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT}, /* ObjectDelay */
    },

    /* ObjectLFO */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */	
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectMixer::IN_C_AMPLITUDE}, /* ObjectAudioMixer */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, PATCHER_ANY}, /* ObjectControlMixer */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectOscillator::IN_C_FREQUENCY}, /* ObjectAudioOscillator */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectOscillator::IN_C_FREQUENCY}, /* ObjectLFO */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectFilter::IN_C_CUTOFF},  /* ObjectFilter */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectSampler::IN_C_RATE}, /* ObjectSampler */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectStepSeq::IN_C_BPM}, /* ObjectStepSeq */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectAudioNoise::IN_C_AMPLITUDE}, /* ObjectAudioNoise */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectControlNoise::IN_C_AMPLITUDE}, /* ObjectControlNoise */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectEcho::IN_C_FEEDBACK}, /* ObjectEcho */
	{node::LINK_CONTROL, ObjectLFO::OUT_C_OUTPUT, ObjectDelay::IN_C_DELAY}, /* ObjectDelay */
    },

    /* ObjectFilter */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_AUDIO, ObjectFilter::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectFilter::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectFilter::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectFilter::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT} /* ObjectDelay */
    },

    /* ObjectSampler */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_AUDIO, ObjectSampler::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectSampler::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectSampler::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectSampler::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT}, /* ObjectDelay */
    },

    /* ObjectStepSeq */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_CONTROL, ObjectStepSeq::OUT_C_OUTPUT, ObjectOscillator::IN_C_TRIGGER}, /* ObjectAudioOscillator */
	{node::LINK_CONTROL, ObjectStepSeq::OUT_C_OUTPUT, ObjectOscillator::IN_C_TRIGGER}, /* ObjectLFO */
	{node::LINK_NONE, 0, 0}, /* ObjectFilter */
	{node::LINK_CONTROL, ObjectStepSeq::OUT_C_OUTPUT, ObjectSampler::IN_C_TRIGGER}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_CONTROL, ObjectStepSeq::OUT_C_OUTPUT, ObjectNoise::IN_C_TRIGGER}, /* ObjectAudioNoise */
	{node::LINK_CONTROL, ObjectStepSeq::OUT_C_OUTPUT, ObjectNoise::IN_C_TRIGGER}, /* ObjectControlNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectEcho */
	{node::LINK_NONE, 0, 0} /* ObjectDelay */
    },

    /* ObjectAudioNoise */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_AUDIO, ObjectAudioNoise::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectAudioNoise::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectAudioNoise::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectAudioNoise::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT}, /* ObjectDelay */
    },

    /* ObjectControlNoise */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */	
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectMixer::IN_C_AMPLITUDE}, /* ObjectAudioMixer */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, PATCHER_ANY}, /* ObjectControlMixer */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectOscillator::IN_C_FREQUENCY}, /* ObjectAudioOscillator */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectOscillator::IN_C_FREQUENCY}, /* ObjectLFO */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectFilter::IN_C_CUTOFF},  /* ObjectFilter */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectSampler::IN_C_RATE}, /* ObjectSampler */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectStepSeq::IN_C_BPM}, /* ObjectStepSeq */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectAudioNoise::IN_C_AMPLITUDE}, /* ObjectAudioNoise */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectControlNoise::IN_C_AMPLITUDE}, /* ObjectControlNoise */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectEcho::IN_C_FEEDBACK}, /* ObjectEcho */
	{node::LINK_CONTROL, ObjectControlNoise::OUT_C_OUTPUT, ObjectDelay::IN_C_DELAY} /* ObjectDelay */
    },

    /* ObjectEcho */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_AUDIO, ObjectEcho::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectEcho::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectEcho::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectEcho::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT}, /* ObjectDelay */
    },

    /* ObjectDelay */
    {
	{node::LINK_NONE, 0, 0}, /* ObjectOutput */
	{node::LINK_AUDIO, ObjectDelay::OUT_A_OUTPUT, PATCHER_ANY}, /* ObjectAudioMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectControlMixer */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioOscillator */
	{node::LINK_NONE, 0, 0}, /* ObjectLFO */
	{node::LINK_AUDIO, ObjectDelay::OUT_A_OUTPUT, ObjectFilter::IN_A_INPUT}, /* ObjectFilter */
	{node::LINK_NONE, 0, 0}, /* ObjectSampler */
	{node::LINK_NONE, 0, 0}, /* ObjectStepSeq */
	{node::LINK_NONE, 0, 0}, /* ObjectAudioNoise */
	{node::LINK_NONE, 0, 0}, /* ObjectControlNoise */
	{node::LINK_AUDIO, ObjectDelay::OUT_A_OUTPUT, ObjectEcho::IN_A_INPUT}, /* ObjectEcho */
	{node::LINK_AUDIO, ObjectDelay::OUT_A_OUTPUT, ObjectDelay::IN_A_INPUT}, /* ObjectDelay */
    }
};

PatcherDynamic::PatcherDynamic() :
    m_changed(false)
{
}

PatcherDynamic::~PatcherDynamic()
{
    for (multiset<Link*, LinkPtrCmp>::iterator i = m_links.begin(); i != m_links.end(); ++i)
	delete *i;
}

bool PatcherDynamic::addNode(node* obj)
{
    if (m_nodes.insert(pair<int, Node>(obj->get_id(), Node(obj))).second) {
	map<int, Node>::iterator i;
	int this_type = obj->get_type ();
	int other_type;
	
	for (i = m_nodes.begin(); i != m_nodes.end(); ++i) {
	    if (i->first == obj->get_id ())
		continue;
	    
	    other_type = i->second.obj->get_type (); 

	    if (PATCHER_TABLE[this_type][other_type].socket_type != node::LINK_NONE) {
		m_links.insert(new Link(obj, (*i).second.obj,
					obj->sqr_distance_to(*i->second.obj),
					i->second.obj->sqr_distance_to_center (),
					PATCHER_TABLE[this_type][other_type].socket_type,
					PATCHER_TABLE[this_type][other_type].src_socket,
					PATCHER_TABLE[this_type][other_type].dest_socket));
	    }

	    if (PATCHER_TABLE[other_type][this_type].socket_type != node::LINK_NONE) {
		m_links.insert(new Link(i->second.obj, obj,
					i->second.obj->sqr_distance_to(*obj),
					obj->sqr_distance_to_center(),
					PATCHER_TABLE[other_type][this_type].socket_type,
					PATCHER_TABLE[other_type][this_type].src_socket,
					PATCHER_TABLE[other_type][this_type].dest_socket));
	    }
	}
	
	m_changed = true;
	return true;
    }
    
    return false;
}

bool PatcherDynamic::deleteNode(node* obj)
{
    if (m_nodes.erase(obj->get_id())) {
	multiset<Link*>::iterator i, r;
	
	for (i = m_links.begin(); i != m_links.end();) {
	    if ((*i)->src == obj || (*i)->dest == obj) {
		r = i++;
		undoLink(**r);
		delete *r;
		m_links.erase(r);
	    } else
		++i;
	}

	m_changed = true;
	return true;
    }
    
    return false;
}

void PatcherDynamic::setParamNode(node* obj, int id)
{
    if (id == node::PARAM_POSITION) {
	multiset<Link*>::iterator i, r;
	list<Link*> readd;
    
	for (i = m_links.begin(); i != m_links.end();) {
	    if ((*i)->src == obj || (*i)->dest == obj) {
		r = i++;

		(*r)->dist = (*r)->src->sqr_distance_to(*(*r)->dest);
		if ((*r)->dest == obj)
		    (*r)->dist_to_center = obj->sqr_distance_to_center();

		readd.push_back(*r);
		m_links.erase(r);
		m_changed = true;
	    }
	    else
		++i;
	}

	for (list<Link*>::iterator it = readd.begin(); it != readd.end(); ++it)
	    m_links.insert(*it);
    }
}

void PatcherDynamic::makeLink(Link& l)
{
    //cout << "making link, source: " << l.src->getID() << " dest: "<< l.dest->getID() << endl;

    node* old_src = l.dest->get_in_socket(l.sock_type, l.actual_in_sock).get_source_node();
    if (old_src != NULL) {
	notifyLinkDeleted(PatcherEvent(old_src, l.dest, l.out_sock, l.actual_in_sock, l.sock_type));
	//cout << "undoing link, source: " << old_src->getID() << " dest: "<< l.dest->getID() << endl;
    }
    
    l.dest->connect_in(l.sock_type, l.actual_in_sock, l.src, l.out_sock);
    notifyLinkAdded(PatcherEvent(l.src, l.dest, l.out_sock, l.actual_in_sock, l.sock_type));
}

void PatcherDynamic::undoLink(Link& l)
{
    if (l.actual_in_sock >= 0) {
	if (l.dest->get_in_socket(l.sock_type, l.actual_in_sock).get_source_node() == l.src) {
	    //cout << "undoing link, source: " << l.src->getID() << " dest: "<< l.dest->getID() << endl;
	    l.dest->connect_in(l.sock_type, l.actual_in_sock, NULL, l.out_sock);

	    notifyLinkDeleted(PatcherEvent(l.src, l.dest, l.out_sock, l.actual_in_sock, l.sock_type));
	}
    }
}

bool PatcherDynamic::isLinked(Link& l)
{
    if (l.actual_in_sock >= 0) {
	if (l.dest->get_in_socket(l.sock_type, l.actual_in_sock).get_source_node() == l.src)
	    return true;
    }
    return false;
}

void PatcherDynamic::findInSock(Link &l)
{
    float max_dist = l.dist;
    
    l.actual_in_sock = -1; 

    if (l.in_sock == -1) {
	for (int i = 0; i < l.dest->get_num_input (l.sock_type); i++) {
	    node* obj = l.dest->get_in_socket (l.sock_type, i).get_source_node (); 
	    float dist;
	    
	    if (obj == NULL) {
		l.actual_in_sock = i;
		return;
	    }
	    else if ((dist = obj->sqr_distance_to (*l.dest)) >= max_dist) {
		max_dist = dist;
		l.actual_in_sock = i;
	    }
	}
    } else {
	node* obj = l.dest->get_in_socket (l.sock_type, l.in_sock).get_source_node (); 
	if (obj == NULL || l.src->sqr_distance_to (*l.dest) < obj->sqr_distance_to (*l.dest))
	    l.actual_in_sock = l.in_sock;
    }
}

void PatcherDynamic::update()
{
    if (!m_changed)
	return;
    
    for (map<int, Node>::iterator i = m_nodes.begin(); i != m_nodes.end(); ++i)
	(*i).second.out_used = false;

    for (multiset<Link*>::iterator i = m_links.begin(); i != m_links.end(); ++i) {
	map<int, Node>::iterator n = m_nodes.find((*i)->src->get_id());
	map<int, Node>::iterator n2 = m_nodes.find((*i)->dest->get_id());
	if (n == m_nodes.end()) {
	    WARNING("Object with id " << (*i)->src->get_id () << "not found.");
	}
	
	Node& node_src = (*n).second;
	Node& node_dest = (*n2).second;
	Link& link = **i;

	//cout << "trying, src: " << link.src->getID() << " dest: " << link.dest->getID() << " out: " << node_src.out_used << endl; 

	if (!node_src.out_used &&
	    !(node_dest.out_used == true && node_dest.dest == link.src))
	{
	    if (!isLinked(link)) {
		findInSock(link);
		if (link.actual_in_sock >= 0) {
		    makeLink(link);
		    node_src.out_used = true;
		    node_src.dest = link.dest;
		}
	    } else {
		node_src.out_used = true;
	    }
	} else {
	    undoLink(link);
	    link.actual_in_sock = -1;
	}
    }
    
    //cout << "---" << endl;

    m_changed = false;
}

void PatcherDynamic::clear()
{
    for (multiset<Link*>::iterator i = m_links.begin(); i != m_links.end(); ++i)
	undoLink(**i);
}

} /* namespace psynth */
