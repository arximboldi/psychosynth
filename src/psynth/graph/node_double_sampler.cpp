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

#include "base/misc.hpp"
#include "base/file_manager.hpp"
#include "synth/util.hpp"
#include "sound/buffer_range_factory.hpp"
#include "graph/node_types.hpp"
#include "graph/node_double_sampler.hpp"

using namespace std;

#define TIME_STRETCH_MIN_SAMPLES 4096
#define SMALL_BLOCK_SIZE 64
#define BIG_BLOCK_SIZE   1024

//#define SAMPLER_BLOCK_SIZE  getInfo().block_size
#define SCALER_TRAINER_SIZE 1024

namespace psynth
{
namespace graph
{

PSYNTH_DEFINE_NODE_FACTORY (node_double_sampler);

node_double_sampler::node_double_sampler(const audio_info& info):
    node0 (info,
	  NODE_DOUBLE_SAMPLER,
	  "double_sampler",
	  N_IN_A_SOCKETS,
	  N_IN_C_SOCKETS,
	  N_OUT_A_SOCKETS,
	  N_OUT_C_SOCKETS),
    m_fetcher_one (m_reader_one),
    m_fetcher_two (m_reader_two),
    m_inbuf_one (info.block_size),
    m_inbuf_two (info.block_size),
    m_param_ampl (0.75f),
    m_param_blend (1.0f),
    m_restart (false)
{
    add_param ("file_one", node_param::STRING, &m_param_file_one,
	       boost::bind (&node_double_sampler::on_file_one_change, this, _1));
    add_param ("file_two", node_param::STRING, &m_param_file_one,
	       boost::bind (&node_double_sampler::on_file_two_change, this, _1));
    add_param ("amplitude", node_param::FLOAT, &m_param_ampl);
    add_param ("blend", node_param::FLOAT, &m_param_blend);

    m_fetcher_one.set_chunk_size (info.block_size);
    m_fetcher_two.set_chunk_size (info.block_size);
    m_fetcher_one.start ();
    m_fetcher_two.start ();
}

node_double_sampler::~node_double_sampler ()
{
    m_fetcher_one.stop ();
    m_fetcher_two.stop ();
}

void node_double_sampler::on_file_one_change (node_param& par)
{
    std::string val;
    boost::filesystem::path path;

    par.get (val);
    path = base::file_manager::self ().path("psychosynth.samples").find (val);

    std::unique_lock<mutex> (m_update_mutex);
    m_reader_one = io::new_file_input <interleaved_range> (path.string ());
    m_fetcher_one.set_input (m_reader_one);

    if (m_reader_one) {
        m_inbuf_one.recreate (get_info ().block_size);
        // m_scaler.set_channels (2); // HACK HACK
	// m_scaler.setSampleRate(m_fetcher.get_info().sample_rate);
    }
}

void node_double_sampler::on_file_two_change (node_param& par)
{
    std::string val;
    boost::filesystem::path path;

    par.get (val);
    path = base::file_manager::self ().path("psychosynth.samples").find (val);

    std::unique_lock<mutex> (m_update_mutex);
    m_reader_two = io::new_file_input <interleaved_range> (path.string ());
    m_fetcher_two.set_input (m_reader_two);

    if (m_reader_two) {
        m_inbuf_two.recreate (get_info ().block_size);
        // m_scaler.set_channels (2); // HACK HACK
	// m_scaler.setSampleRate(m_fetcher.get_info().sample_rate);
    }
}


void node_double_sampler::do_update (const node0* caller, int caller_port_type, int caller_port)
{
    audio_buffer* out = get_output<audio_buffer> (node0::LINK_AUDIO, OUT_A_OUTPUT);
    const sample_buffer* trig = get_input<sample_buffer> (node0::LINK_CONTROL, IN_C_TRIGGER);
    const sample* trig_buf = trig ? (const sample*) &const_range (*trig)[0] : 0;
    link_envelope trig_env =  get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);

    /* Read the data. */
    size_t start = 0;
    size_t end = get_info ().block_size;

    if (m_reader_one && m_reader_two)
    {
	while (start < get_info ().block_size) {
	    if (m_restart) {
		if (trig_buf && trig_buf[start] != 0.0f) {
		    restart();
		    m_restart = false;
		}
	    }

	    if (trig)
		end = synth::find_hill (const_range (*trig), start);

	    read (*out, start, end);

	    float env_val = (float) (audio_sample) trig_env.update (end - start);
	    if (env_val == 1.0f && trig_buf && trig_buf[end - 1] == 0.0f)
		m_restart = true;

	    start = end;
	}
    } else
        fill_frames (range (*out), audio_frame (0));

    /* Set amplitude. */
    sample* buf = (sample*) &range (*out)[0][0];
    int count = get_info ().block_size * get_info ().num_channels;
    while(count--)
	*buf++ *= m_param_ampl;

    /* Apply trigger envelope. */
    if (trig_buf) {
	for (size_t i = 0; i < get_info ().num_channels; ++i)
        {
            sample* buf = (sample*) &range (*out)[0][i];
	    int n_samp = get_info ().block_size;
	    trig_buf = (const sample*) &const_range (*trig)[0];
	    trig_env = get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);

	    while (n_samp--) {
		float env_val = (audio_sample) trig_env.update();
		*buf = *buf * ((1.0f - env_val) + (env_val * *trig_buf));
		++buf;
		++trig_buf;
	    }
	}
    }
}

void node_double_sampler::restart()
{
    m_fetcher_one.seek (0, io::seek_dir::beg);
    m_fetcher_two.seek (0, io::seek_dir::beg);
}

void node_double_sampler::read (audio_buffer& buf, int start, int end)
{
    const sample_buffer* blend = get_input<sample_buffer> (node0::LINK_CONTROL,
                                                           IN_C_BLEND);
    const sample* blend_buf = blend ?
        (const sample*) &const_range (*blend)[0] : 0;

    size_t must_read;
    size_t nread;
    size_t tbeg, tend;

    std::unique_lock<std::mutex> lock (m_update_mutex);

    must_read = end - start;
    tbeg = tend = 0;

    while (must_read) {
	nread = m_fetcher_one.take (sub_range (range (m_inbuf_one), 0, must_read));

	if (nread) {
	    must_read -= nread;
	    tend += nread;
	    for (size_t i = 0; i < range (buf).num_samples (); ++i) {
		for (size_t j = tbeg; j < tend; j++) {
		    range (buf)[j][i] =
			range (m_inbuf_one) [j - tbeg][i]
			* (m_param_blend * (blend ? blend_buf [j] : 1.0));
		}
	    }
	    tbeg += nread;
	} else
	    m_fetcher_one.seek (0, io::seek_dir::beg);
    }

    must_read = end - start;
    tbeg = tend = 0;
    while (must_read) {
	nread = m_fetcher_two.take (sub_range (range (m_inbuf_two), 0, must_read));
	if (nread) {
	    must_read -= nread;
	    tend += nread;
	    for (size_t i = 0; i < range (buf).num_samples (); ++i) {
                for (size_t j = tbeg; j < tend; j++) {
		    range (buf)[j][i] +=
			range (m_inbuf_two) [j - tbeg][i]
			* (1 - m_param_blend * (blend ? blend_buf [j] : 1.0));
		}
	    }
	    tbeg += nread;
	} else
	    m_fetcher_two.seek (0, io::seek_dir::beg);
    }
}

void node_double_sampler::do_advance ()
{
}

void node_double_sampler::on_info_change ()
{
}

} /* namespace graph */
} /* namespace psynth */
