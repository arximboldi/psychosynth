/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#ifndef PSYNTH_NODE_H
#define PSYNTH_NODE_H

#include <list>
#include <vector>
#include <cmath>
#include <set>
#include <iostream>
#include <algorithm>

#include <boost/thread/mutex.hpp>

#include <psynth/synth/sample_buffer.hpp>
#include <psynth/synth/audio_buffer.hpp>
#include <psynth/base/vector_2d.hpp>
#include <psynth/base/deleter.hpp>
#include <psynth/synth/envelope_simple.hpp>
#include <psynth/node/node_param.hpp>
#include <psynth/node/watch.hpp>

namespace psynth
{

class node
{
public:
    static const int NULL_ID = -1;
    
    enum link_type {
	LINK_NONE = -1,
	LINK_AUDIO,
	LINK_CONTROL,
	LINK_TYPES
    };
    
    enum commom_params {
	PARAM_POSITION = 0,
	PARAM_RADIOUS,
	PARAM_MUTE,
	N_COMMON_PARAMS
    };
    
    class out_socket {
	friend class node;

	int m_type;
	std::list<std::pair<node*, int> > m_ref; /* Objetos y puertos destino */

    protected:
	out_socket (int type)
	    : m_type(type)
	    {}

	bool is_empty () {
	    return m_ref.empty();
	}
	
	void add_reference (node* obj, int port) {
	    m_ref.push_back(std::pair<node*, int>(obj, port));
	}

	void remove_reference (node* obj, int port) {
	    m_ref.remove(std::pair<node*, int>(obj, port));
	}
	
	void clear_references () {
	    std::list<std::pair<node*, int> >::iterator i, r;
	    for (i = m_ref.begin(); i != m_ref.end(); ) {
		r = i++;
		r->first->connect_in (m_type, r->second, NULL, 0);
	    }
	}

    public:
	const std::list<std::pair<node*, int> >& get_references () const {
	    return m_ref;
	}
    };
	
    class in_socket
    {
	friend class node;

	void attach_watch (watch* watch) {
	    m_watchs.push_back(watch);
	}

	void detach_watch (watch* watch) {
	    m_watchs.remove(watch);
	};
	
    protected:
	int m_type;
	node* m_srcobj;
	int m_srcport;
	std::list<watch*> m_watchs;
	
	in_socket (int type) :
	    m_type(type), m_srcobj(NULL), m_srcport(0) {}
	
	bool is_empty () {
	    return m_srcobj == NULL;
	}
	
	void set(node* srcobj, int port) {
	    m_srcobj = srcobj;
	    m_srcport = port;
	}
		
	void update_input (const node* caller, int caller_port_type, int caller_port);

	template <typename SocketDataType>
	const SocketDataType* get_data (int type) const {
	    if (m_srcobj)
		return m_srcobj->get_output <SocketDataType> (type, m_srcport);
	    else
		return NULL;
	}
	
    public:
	virtual ~in_socket() {
	    for_each (m_watchs.begin(), m_watchs.end(), deleter<watch*>());
	}
	
	virtual node* get_source_node () const {
	    return m_srcobj;
	}

	virtual int get_source_socket () const {
	    return m_srcport;
	}
    };
    
    class in_socket_manual : public in_socket
    {
	friend class node;
	
	bool must_update;
	node* src_obj;
	int src_sock;

    public:
	in_socket_manual(int type) :
	    in_socket(type),
	    must_update(false),
	    src_obj(NULL),
	    src_sock(-1) {}

	virtual node* get_source_node () const {
	    return src_obj;
	}

	virtual int get_source_socket () const {
	    return src_sock;
	}	
    };

private:
    audio_info m_audioinfo;

    std::vector<audio_buffer> m_outdata_audio;
    std::vector<sample_buffer> m_outdata_control;

    std::vector<out_socket> m_out_sockets[LINK_TYPES];
    std::vector<sample> m_out_stable_value[LINK_TYPES];
    std::vector<in_socket_manual> m_in_sockets[LINK_TYPES];
    std::vector<envelope_simple> m_in_envelope[LINK_TYPES];
    envelope_simple m_out_envelope;
    
    std::vector<node_param*> m_params;
    node_param m_null_param;
    int m_nparam;
    
    int m_id;
    int m_type;
    std::string m_name;
    
    vector_2f m_param_position;
    float m_param_radious;
    int m_param_mute;
    
    /* For !m_single_update, contains the nodes that has
     * been updated (<obj_id, port_id>) */
    std::set<std::pair<int,int> > m_updated_links[LINK_TYPES];
    bool m_updated;
    bool m_single_update;
    
    boost::mutex m_paramlock;

    void blend_buffer (sample* buf, int n_elem,
		       sample stable_value, envelope_simple env);
    void update_params_out ();
    void update_inputs ();
    void update_in_sockets ();
    void set_envelopes_deltas ();
    void update_envelopes ();
    bool can_update (const node* caller, int caller_port_type,
		     int caller_port);

protected:
    template <typename SocketDataType>
    SocketDataType* get_output (int type, int socket) {
	if (m_param_mute && m_out_envelope.finished())
	    return NULL;
	
	/* TODO: Find a way to do type checking */
	switch(type) {
	case LINK_AUDIO:
	    return reinterpret_cast<SocketDataType*>(&(m_outdata_audio[socket]));
	    break;
	case LINK_CONTROL:
	    return reinterpret_cast<SocketDataType*>(&(m_outdata_control[socket]));
	    break;
	default:
	    break;
	}
	
	return NULL;
    }

    void set_output_stable_value (int sock_type, int sock_num, sample value) {
	m_out_stable_value[sock_type][sock_num] = value;
    }
    
    virtual void do_update (const node* caller, int caller_port_type, int caller_port) = 0;
    virtual void do_advance () = 0;
    virtual void on_info_change () = 0;

    void del_param (int index);
    void add_param (const std::string&, int type, void* val);
    void add_param (const std::string&, int type, void* val, node_param::event ev);

    envelope_simple get_in_envelope (int type, int sock) {
	return m_in_envelope[type][sock];
    }

public:
    node (const audio_info& prop, int type,
	  const std::string& name,
	  int inaudiosocks, int incontrolsocks,
	  int outaudiosocks, int outcontrolsocks,
	  bool single_update = true);
    
    virtual ~node();
    
    int get_type () const {
	return m_type;
    };

    const std::string& get_name () const {
	return m_name;
    }

    void update_params_in ();
    void update (const node* caller, int caller_port_type, int caller_port);
	
    void advance () {
	m_updated = false;
	if (!m_single_update) {
	    m_updated_links[LINK_AUDIO].clear();
	    m_updated_links[LINK_CONTROL].clear();
	}

	do_advance();
    }
	
    const audio_info& get_info () const {
	return m_audioinfo;
    }

    void set_info(const audio_info& info);

    void attach_watch (int type, int in_socket, watch* watch) {
	watch->set_info (m_audioinfo);
	m_in_sockets[type][in_socket].attach_watch(watch);
    }

    void detach_watch (int type, int in_socket, watch* watch) {
	m_in_sockets[type][in_socket].detach_watch(watch);
    }
    
    void connect_in (int type, int in_socket, node* src, int out_socket);

    void force_connect_in (int type, int in_socket, node* src, int out_socket);

    void clear_connections ();

    bool has_connections ();
    
    node_param& param (int id) {
	return *m_params[id];
    }

    const node_param& param (int id) const {
	return *m_params[id];
    }

    node_param& param (const std::string& name);

    const node_param& param (const std::string& name) const;
    
    template <typename SocketDataType>
    const SocketDataType* get_output (int type, int socket) const {
	return get_output <SocketDataType> (type, socket);
    }

    template <typename SocketDataType>
    const SocketDataType* get_input(int type, int socket) const {
	return m_in_sockets[type][socket].get_data<SocketDataType>(type);
    }

    const out_socket& get_out_socket(int type, int socket) const {
	return m_out_sockets[type][socket];
    }
    
    const in_socket& get_in_socket(int type, int socket) const {
	return m_in_sockets[type][socket];
    }
    
    int get_num_output (int type) const {
	return m_out_sockets[type].size();
    }
	
    int get_num_input (int type) const {
	return m_in_sockets[type].size();
    }
    
    int get_id () const {
	return m_id;
    }
	
    void set_id (int id) {
	m_id = id;
    }
        
    float distance_do (const node &obj) const {
	return m_param_position.distance(obj.m_param_position);
    }
	
    float sqr_distance_to (const node &obj) const {
	return m_param_position.sqr_distance (obj.m_param_position);
    }

    float distance_to_center () const {
	return m_param_position.length();
    }
	
    float sqr_distance_to_center () const {
	return m_param_position.sqr_length();
    }
    
    float distance_to (const vector_2f& obj) const {
	return m_param_position.distance(obj);
    }
	
    float sqr_distance_to (const vector_2f &obj) const {
	return m_param_position.sqr_distance(obj);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_NODE_H */
