/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#include <boost/bind.hpp>

#include "synth/audio_buffer.hpp"
#include "base/misc.hpp"
#include "base/file_manager.hpp"
#include "node/node_types.hpp"
#include "node/node_double_sampler.hpp"

using namespace std;

#define TIME_STRETCH_MIN_SAMPLES 4096
#define SMALL_BLOCK_SIZE 64
#define BIG_BLOCK_SIZE   1024

//#define SAMPLER_BLOCK_SIZE  getInfo().block_size
#define SCALER_TRAINER_SIZE 1024

namespace psynth
{

PSYNTH_DEFINE_NODE_FACTORY (node_double_sampler);

node_double_sampler::node_double_sampler(const audio_info& info): 
    node (info,
	  NODE_DOUBLE_SAMPLER,
	  "double_sampler",
	  N_IN_A_SOCKETS, 
	  N_IN_C_SOCKETS,
	  N_OUT_A_SOCKETS,
	  N_OUT_C_SOCKETS),
    m_fetcher_one (&m_reader_one),
    m_fetcher_two (&m_reader_two),
    m_inbuf_one (info),
    m_inbuf_two (info),
    m_param_ampl(0.75f),
    m_param_blend(1.0f),
    m_restart(false)
{
    add_param ("file_one", node_param::STRING, &m_param_file_one,
	       boost::bind (&node_double_sampler::on_file_one_change, this, _1));
    add_param ("file_two", node_param::STRING, &m_param_file_one,
	       boost::bind (&node_double_sampler::on_file_two_change, this, _1));
    add_param ("amplitude", node_param::FLOAT, &m_param_ampl);
    add_param ("blend", node_param::FLOAT, &m_param_blend);
    
    m_fetcher_one.start ();
    m_fetcher_two.start ();
}

node_double_sampler::~node_double_sampler ()
{
    m_fetcher_one.finish ();
    m_fetcher_two.finish ();
}

void node_double_sampler::on_file_one_change (node_param& par)
{
    std::string val;
    boost::filesystem::path path;
    
    par.get (val);
    path = base::file_manager::self ().path("psychosynth.samples").find (val);
    m_update_mutex.lock();

    if (m_fetcher_one.is_open ())
	m_fetcher_one.close();

    m_fetcher_one.open (path.native ());

    if (m_fetcher_one.is_open()) {
	m_inbuf_one.set_info (m_fetcher_one.get_info(), get_info ().block_size);
    }
    
    m_update_mutex.unlock();
}

void node_double_sampler::on_file_two_change (node_param& par)
{
    std::string val;
    boost::filesystem::path path;
    
    par.get (val);
    path = base::file_manager::self ().path("psychosynth.samples").find (val);
        
    m_update_mutex.lock();

    if (m_fetcher_two.is_open ())
	m_fetcher_two.close();
    
    m_fetcher_two.open (path.native ());

    if (m_fetcher_two.is_open()) {
	m_inbuf_two.set_info (m_fetcher_two.get_info(), get_info ().block_size);
    }
    
    m_update_mutex.unlock();
}


void node_double_sampler::do_update (const node* caller, int caller_port_type, int caller_port)
{
    audio_buffer* out = get_output<audio_buffer> (node::LINK_AUDIO, OUT_A_OUTPUT);
    const sample_buffer* trig = get_input<sample_buffer> (node::LINK_CONTROL, IN_C_TRIGGER);
    const sample* trig_buf = trig ? trig->get_data() : 0;
    envelope_simple trig_env =  get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);

    /* Read the data. */
    size_t start = 0;
    size_t end = get_info ().block_size;

    if (m_fetcher_one.is_open () &&
	m_fetcher_two.is_open ())
    {
	while (start < get_info ().block_size) {
	    if (m_restart) {
		if (trig_buf && trig_buf[start] != 0.0f) {
		    restart();
		    m_restart = false;
		}
	    }
	    
	    if (trig)
		end = trig->find_hill (start);

	    read (*out, start, end);
	
	    float env_val = trig_env.update(end - start);
	    if (env_val == 1.0f && trig_buf && trig_buf[end - 1] == 0.0f)
		m_restart = true;
	    
	    start = end;
	} 
    } else
	out->zero();
    
    /* Set amplitude. */
    sample* buf = out->get_data()[0];
    int count = get_info ().block_size * get_info ().num_channels;
    while(count--)
	*buf++ *= m_param_ampl;

    /* Apply trigger envelope. */
    if (trig_buf) {
	for (size_t i = 0; i < get_info ().num_channels; ++i) {
	    buf = out->get_channel(i);
	    int n_samp = get_info ().block_size;
	    trig_buf = trig->get_data();
	    trig_env = get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);

	    while (n_samp--) {
		float env_val = trig_env.update();
		*buf = *buf * ((1.0f - env_val) + (env_val * *trig_buf));
		++buf;
		++trig_buf;
	    }
	}
    }
    
}

void node_double_sampler::restart()
{
    m_fetcher_one.force_seek (0);
    m_fetcher_two.force_seek (0);
}

void node_double_sampler::read (audio_buffer& buf, int start, int end)
{
    const sample_buffer* blend = get_input<sample_buffer> (node::LINK_CONTROL, IN_C_BLEND);
    const sample* blend_buf = blend ? blend->get_data() : 0;

    size_t must_read;
    size_t nread;
    size_t tbeg, tend;

    m_update_mutex.lock();
    must_read = end - start;
    tbeg = tend = 0;
    while (must_read) {
	nread = m_fetcher_one.read (m_inbuf_one, must_read);
	if (nread) {
	    must_read -= nread;
	    tend += nread;
	    for (size_t i = 0; i < buf.get_info().num_channels; ++i) {
		for (size_t j = tbeg; j < tend; j++) {
		    buf[i][j] =
			m_inbuf_one[i%m_inbuf_one.get_info().num_channels][j-tbeg]
			* (m_param_blend *
			   (blend ? blend_buf[j] : 1.0));
		}
	    }
	    tbeg += nread;
	} else
	    m_fetcher_one.seek (0);
    }

    must_read = end - start;
    tbeg = tend = 0;
    while (must_read) {
	nread = m_fetcher_two.read (m_inbuf_two, must_read);
	if (nread) {
	    must_read -= nread;
	    tend += nread;
	    for (size_t i = 0; i < buf.get_info().num_channels; ++i) {
		for (size_t j = tbeg; j < tend; j++) {
		    buf[i][j] +=
			m_inbuf_two[i%m_inbuf_two.get_info().num_channels][j-tbeg]
			* (1 - m_param_blend *
			   (blend ? blend_buf[j] : 1.0));
		}
	    }
	    tbeg += nread;
	} else
	    m_fetcher_two.seek (0);
    }
    m_update_mutex.unlock();
}

void node_double_sampler::do_advance ()
{
}

void node_double_sampler::on_info_change ()
{
}

} /* namespace psynth */

