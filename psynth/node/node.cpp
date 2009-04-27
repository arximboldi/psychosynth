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

#include <algorithm>

#include "node/node.hpp"

#include <cmath>

#define ENV_RISE_SECONDS  0.004f
#define ENV_FALL_SECONDS  0.004f
#define ENV_DELTA(srate, sec)  (1/((sec)*(srate)))

using namespace std;

namespace psynth
{

node::node(const audio_info& info, int type,
	       const std::string& name,
	       int n_in_audio, int n_in_control,
	       int n_out_audio, int n_out_control,
	       bool single_update) :
    m_audioinfo(info),
    m_outdata_audio(n_out_audio, audio_buffer(info)),
    m_outdata_control(n_out_control, sample_buffer (info.block_size)),
    m_nparam(0),
    m_id(NULL_ID),
    m_type(type),
    m_name(name),
    m_param_position(0,0),
    m_param_radious(5.0f),
    m_param_mute(false),
    m_updated(false),
    m_single_update(single_update)
{
    add_param("position", node_param::VECTOR2F, &m_param_position);
    add_param("radious", node_param::FLOAT, &m_param_radious);
    add_param("mute", node_param::INT, &m_param_mute);
    
    m_out_sockets[LINK_AUDIO].resize(n_out_audio, out_socket(LINK_AUDIO));
    m_out_sockets[LINK_CONTROL].resize(n_out_control, out_socket(LINK_CONTROL));
    m_in_sockets[LINK_AUDIO].resize(n_in_audio, in_socket_manual(LINK_AUDIO));
    m_in_sockets[LINK_CONTROL].resize(n_in_control, in_socket_manual(LINK_CONTROL));
    m_in_envelope[LINK_AUDIO].resize(n_in_audio);
    m_in_envelope[LINK_CONTROL].resize(n_in_control);
    m_out_stable_value[LINK_AUDIO].resize(n_out_audio, 0.0f);
    m_out_stable_value[LINK_CONTROL].resize(n_out_control, 0.0f);
    
    set_envelopes_deltas();
}

node::~node()
{
    for_each(m_params.begin(), m_params.end(), deleter<node_param*>());
//    cout << "Deleting object.\n";
}

void node::set_envelopes_deltas()
{
    int i;
    
    float rise_dt = ENV_DELTA(m_audioinfo.sample_rate, ENV_RISE_SECONDS);
    float fall_dt = -ENV_DELTA(m_audioinfo.sample_rate, ENV_FALL_SECONDS);

    for (i = 0; i < LINK_TYPES; ++i)
	for (vector<envelope_simple>::iterator it = m_in_envelope[i].begin();
	     it != m_in_envelope[i].end();
	     ++it) {
	    it->set_deltas (rise_dt, fall_dt);
	}

    m_out_envelope.set_deltas (rise_dt, fall_dt);
    m_out_envelope.set (1.0f);
}

void node::add_param (const std::string& name, int type, void* val)
{
    m_params.push_back(new node_param);
    m_params[m_nparam]->configure(m_nparam, name, type, val);
    m_nparam++;   
}

void node::add_param(const std::string& name, int type, void* val,
		     node_param::event ev)
{
    m_params.push_back(new node_param);
    m_params[m_nparam]->configure(m_nparam, name, type, val, ev);
    m_nparam++;
}

void node::del_param(int index)
{
    
    m_nparam--;
}

node_param& node::param(const std::string& name)
{
    for (vector<node_param*>::iterator it = m_params.begin();
	 it != m_params.end();
	 ++it)
	if (name == (*it)->get_name())
	    return **it;
    
    return m_null_param;
}

const node_param& node::param (const std::string& name) const
{
    for (vector<node_param*>::const_iterator it = m_params.begin();
	 it != m_params.end();
	 ++it)
	if (name == (*it)->get_name())
	    return **it;
    
    return m_null_param;
}

void node::clear_connections()
{
    size_t i, j;

    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    if (!m_in_sockets[i][j].is_empty())
		connect_in(i, j, NULL, 0);    
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_out_sockets[i].size(); ++j)
	    if (!m_out_sockets[i][j].is_empty())
		m_out_sockets[i][j].clear_references();
}

bool node::has_connections ()
{
    size_t i, j;

    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    if (!m_in_sockets[i][j].is_empty())
		return true;
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_out_sockets[i].size(); ++j)
	    if (!m_out_sockets[i][j].is_empty())
		return true;

    return false;
}

void node::connect_in (int type, int in_socket, node* src, int out_socket)
{
    m_in_sockets[type][in_socket].src_obj = src;
    m_in_sockets[type][in_socket].src_sock = out_socket;

    if (!m_in_envelope[type][in_socket].finished()) {
	m_in_sockets[type][in_socket].must_update = true;
	m_in_envelope[type][in_socket].release();
    } else {
	m_in_sockets[type][in_socket].must_update = false;
	force_connect_in (type, in_socket, src, out_socket);
    }
}

void node::force_connect_in (int type, int in_socket, node* src, int out_socket)
{
    m_in_envelope[type][in_socket].press();
    
    if (m_in_sockets[type][in_socket].m_srcobj)
	m_in_sockets[type][in_socket].m_srcobj->
	    m_out_sockets[type][out_socket].remove_reference (this, in_socket);
    
    m_in_sockets[type][in_socket].set(src, out_socket);
    
    if (src)
	src->m_out_sockets[type][out_socket].add_reference (this, in_socket);    
}

inline bool node::can_update (const node* caller, int caller_port_type,
			      int caller_port)
{
    bool ret;

    if (m_single_update || !caller)
	ret = !m_updated;
    else
	ret =
	    m_updated_links[caller_port_type].insert(make_pair(caller->get_id (),
							       caller_port)).second;
    m_updated = true;
    
    return ret;
}

void node::in_socket::update_input (const node* caller, int caller_port_type,
				    int caller_port)
{
    if (m_srcobj) {
	m_srcobj->update(caller, caller_port_type, caller_port);

	if (m_type == LINK_AUDIO) {
	    const audio_buffer* buf =
		m_srcobj->get_output <audio_buffer> (m_type, m_srcport);
	    if (buf)
		for (list<watch*>::iterator it = m_watchs.begin();
		     it != m_watchs.end(); ++it)
		    (*it)->update(*buf);
	} else {
	    const sample_buffer* buf =
		m_srcobj->get_output <sample_buffer> (m_type, m_srcport);
	    if (buf)
		for (list<watch*>::iterator it = m_watchs.begin();
		     it != m_watchs.end(); ++it) {
		    (*it)->update(*buf);
		}
	}
    }
}

void node::blend_buffer (sample* buf, int n_elem,
			 sample stable_value, envelope_simple env)
{
    while(n_elem--) {
	float env_val = env.update();
	*buf = (*buf * env_val) + (stable_value * (1 - env_val));
	*buf++;
    }
}

void node::update_envelopes ()
{
    size_t i, j;
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (vector<envelope_simple>::iterator it = m_in_envelope[i].begin();
	     it != m_in_envelope[i].end();
	     ++it)
	    it->update (m_audioinfo.block_size);

    /* Apply envelopes to output (for soft muting) */
    for (i = 0; i < m_outdata_audio.size(); ++i)
	for (j = 0; j < m_audioinfo.num_channels; ++j)
	    blend_buffer(m_outdata_audio[i][j], m_audioinfo.block_size,
			 m_out_stable_value[LINK_AUDIO][i], m_out_envelope);
    
    for (i = 0; i < m_outdata_control.size(); ++i)
	blend_buffer(m_outdata_control[i].get_data (), m_audioinfo.block_size,
		     m_out_stable_value[LINK_CONTROL][i], m_out_envelope);

    m_out_envelope.update(m_audioinfo.block_size);
}

void node::update_in_sockets ()
{
    int i, j;

    /* TODO: Thread synch! */
    for (j = 0; j < LINK_TYPES; ++j)
	for (i = 0; i < m_in_envelope[j].size (); ++i) {
	    if (m_in_sockets[j][i].must_update &&
		m_in_envelope[j][i].finished ()) {
		force_connect_in (j, i,
				  m_in_sockets[j][i].src_obj,
				  m_in_sockets[j][i].src_sock);
		m_in_sockets[j][i].must_update = false;
	    }
	}
}

void node::update_params_in ()
{
    size_t j;
    
    for (vector<node_param*>::iterator i = m_params.begin(); i != m_params.end(); ++i)
	(*i)->update_in ();

    if (m_param_mute)
	m_out_envelope.release ();
    else
	m_out_envelope.press ();
}

void node::update_params_out ()
{
    size_t j;
    
    for (vector<node_param*>::iterator i = m_params.begin(); i != m_params.end(); ++i)
	(*i)->update_out ();
}

void node::update_inputs ()
{
    size_t j, i;

    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    m_in_sockets[i][j].update_input(this, i, j);
}

void node::update (const node* caller, int caller_port_type, int caller_port)
{
    if (can_update (caller, caller_port_type, caller_port)) {
	update_params_in();

	if (!m_param_mute || !m_out_envelope.finished()) {
	    update_inputs ();
	    do_update (caller, caller_port_type, caller_port); 
	    
	}
	
	update_params_out ();
	update_envelopes ();
	update_in_sockets ();
    }
}

void node::set_info (const audio_info& info)
{
    int i;
    
    for (i = 0; i < m_outdata_audio.size(); ++i)
	m_outdata_audio[i].set_info(info);

    if (m_audioinfo.block_size != info.block_size)
	for (i = 0; i < m_outdata_control.size(); ++i)
	    m_outdata_control[i].resize(info.block_size);

    m_audioinfo = info;

    set_envelopes_deltas ();
}   

} /* namespace psynth */
