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
 
#ifndef PSYNTH_NODE_OUTPUT_H
#define PSYNTH_NODE_OUTPUT_H

#include <list>

#include <boost/thread/mutex.hpp>

#include <psynth/common/ring_audio_buffer.hpp>
#include <psynth/node/node.hpp>
#include <psynth/output/output.hpp>
#include <psynth/node/node_factory.hpp>

namespace psynth
{

class node_manager;

class node_output : public node
{
    struct slot {
	ring_audio_buffer::read_ptr m_ptr;
	output* m_out;
	node_output* m_parent;
	audio_buffer m_buf;
	
	slot (output* out, node_output* parent,
	     ring_audio_buffer::read_ptr ptr, const audio_info& info)
	    : m_ptr(ptr)
	    , m_out(out)
	    , m_parent(parent)
	    , m_buf(info) {
	    out->set_callback (&node_output::output_callback, this);
	}
		
	~slot() {
	    delete m_out;
	}
    };
	
    ring_audio_buffer m_buffer;
    
    node_manager* m_manager;
    std::list<slot*> m_slots;
    std::list<output*> m_passive_slots;

    /*
      RWLock m_buflock;
      Mutex m_passive_lock;
      Mutex m_global_lock;
    */
    
    static void output_callback (int nframes, void* arg);
    
    void do_output (slot& slot, size_t nframes);
	
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change ();
    
public:
    enum in_audio_socket_id {
	IN_A_INPUT,
	N_IN_A_SOCKETS
    } ;
	
    enum in_control_socket_id {
	N_IN_C_SOCKETS
    };
	
    enum out_audio_socket_id {
	N_OUT_A_SOCKETS
    };
	
    enum  out_control_socket_id {
	N_OUT_C_SOCKETS
    };

    enum param_id {
	N_PARAM
    };
	
    node_output (const audio_info& info);

    ~node_output ();
	
    bool set_manager (node_manager* mgr) {
	if (m_manager != NULL && mgr != NULL) 
	    return false; /* Already attached */
		
	m_manager = mgr;
	return true;
    }
	
    void attach_output (output* out) {
	//m_buflock.readLock();
	m_slots.push_back(new slot (out, this, m_buffer.end(), get_info()));
	//m_buflock.unlock();
    };
    
    void detach_output (output* out) {
	for (std::list<slot*>::iterator i = m_slots.begin(); i != m_slots.end();) {
	    if ((*i)->m_out == out) {
		(*i)->m_out->set_callback (NULL, NULL); /* This should take threading into account. */
		(*i)->m_out = NULL;
		delete *i;
		m_slots.erase(i++);
	    } else
		++i;
	}
    };

    void attach_passive_output (output* out) {
	//m_passive_lock.lock();
	m_passive_slots.push_back (out);
	//m_passive_lock.unlock();
    };

    void detach_passive_output (output* out) {
	//m_passive_lock.lock();
	m_passive_slots.remove(out);
	//m_passive_lock.unlock();
    };
};

PSYNTH_DECLARE_NODE_FACTORY(node_output, "output");

} /* namespace psynth */

#endif /* PSYNTH_OUBJECTOUTPUT_H */
