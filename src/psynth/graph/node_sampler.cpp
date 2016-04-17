/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008, 2016                    *
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

#define PSYNTH_MODULE_NAME "psynth.io.node_sampler"

#include <boost/bind.hpp>

#include "base/misc.hpp"
#include "base/logger.hpp"
#include "base/file_manager.hpp"
#include "sound/output.hpp"
#include "synth/util.hpp"
#include "graph/node_types.hpp"
#include "graph/node_sampler.hpp"

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

PSYNTH_DEFINE_NODE_FACTORY (node_sampler);

node_sampler::node_sampler(const audio_info& info):
    node0 (info,
	  NODE_SAMPLER,
	  "sampler",
	  N_IN_A_SOCKETS,
	  N_IN_C_SOCKETS,
	  N_OUT_A_SOCKETS,
	  N_OUT_C_SOCKETS),
    m_fetcher (m_reader),
    m_inbuf (info.block_size),
    m_scaler (info.sample_rate),
    m_ctrl_pos (0),
    m_param_ampl(0.75f),
    m_param_rate (1.0f),
    m_param_tempo (1.0f),
    m_param_pitch (1.0f),
    m_restart (false)
{
    add_param ("file", node_param::STRING, &m_param_file,
	       boost::bind (&node_sampler::on_file_change, this, _1));
    add_param ("amplitude", node_param::FLOAT, &m_param_ampl);
    add_param ("rate", node_param::FLOAT, &m_param_rate);
    add_param ("tempo", node_param::FLOAT, &m_param_tempo);
    add_param ("pitch", node_param::FLOAT, &m_param_pitch);

    m_scaler.set_rate (1.0);
    m_scaler.set_frame_rate (info.sample_rate);

    m_fetcher.set_chunk_size (info.block_size);
    m_fetcher.start ();
}

node_sampler::~node_sampler ()
{
    m_fetcher.stop ();
}

void node_sampler::on_file_change (node_param& par)
{
    std::string val;
    boost::filesystem::path path;

    par.get (val);
    path = base::file_manager::self ().path("psychosynth.samples").find (val);

    std::unique_lock<std::mutex> lock (m_update_mutex);

    m_reader = io::new_file_input <interleaved_range> (path.string ());
    m_fetcher.set_input (m_reader);

    if (m_reader) {
        m_inbuf.recreate (get_info ().block_size);
        // m_scaler.set_channels (2); // HACK HACK
	// m_scaler.setSampleRate(m_fetcher.get_info().sample_rate);
    }
}

void node_sampler::do_update (const node0* caller, int caller_port_type, int caller_port)
{
    audio_buffer* out = get_output<audio_buffer> (node0::LINK_AUDIO, OUT_A_OUTPUT);
    const sample_buffer* trig = get_input<sample_buffer> (node0::LINK_CONTROL, IN_C_TRIGGER);
    const sample* trig_buf = trig ? (const sample*) &const_range(*trig)[0] : 0;
    link_envelope trig_env =  get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);

    /* Read the data. */
    size_t start = 0;
    size_t end = get_info ().block_size;

    if (m_reader)
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

	    read(*out, start, end);

	    float env_val = (float) (audio_sample) trig_env.update (end - start);
	    if (env_val == 1.0f && trig_buf && trig_buf[end - 1] == 0.0f)
		m_restart = true;

	    start = end;
	}
    } else
	fill_frames (range (*out), audio_frame (0));

    /* Set amplitude. */
    transform_frames (
        range (*out), range (*out),
        [&] (audio_frame in) -> audio_frame {
            static_transform (in, in, [&] (audio_sample in) -> audio_sample {
                    return in * this->m_param_ampl;
                });
            return in;
        });

    /* Apply trigger envelope. */
    if (trig_buf) {
	for (size_t i = 0; i < get_info ().num_channels; ++i) {
	    sample* buf = (sample*) &range (*out)[0][i];
	    int n_samp = get_info ().block_size;
	    trig_buf = (const sample*) &const_range (*trig)[0];
	    trig_env = get_in_envelope (LINK_CONTROL, IN_C_TRIGGER);

	    while (n_samp--) {
		float env_val = (float) (audio_sample) trig_env.update();
		*buf = *buf * ((1.0f - env_val) + (env_val * *trig_buf));
		++buf;
		++trig_buf;
	    }
	}
    }

}

void node_sampler::restart()
{
    m_fetcher.seek (0, io::seek_dir::beg);
    m_scaler.clear();
}

void node_sampler::read (audio_buffer& buf, int start, int end)
{
    const sample_buffer* rate = get_input<sample_buffer> (
        node0::LINK_CONTROL, IN_C_RATE);
    const sample* rate_buf = rate ? (const sample*) &const_range (*rate) [0] : 0;

    float base_factor =
	(float) m_reader->frame_rate () / get_info ().sample_rate * m_param_rate;

    int must_read;
    int nread;
    float factor;

    bool backwards = false;;
    bool high_latency = false;

    if (m_update_mutex.try_lock ())
    {
        m_scaler.set_tempo (m_param_tempo);
        m_scaler.set_pitch (m_param_pitch);
        m_update_mutex.unlock ();
    }

    if (m_param_tempo != 1.0f ||
	m_param_pitch != 1.0f)
	high_latency = true;

    if (base_factor < 0) {
	backwards = true;
	base_factor = -base_factor;
    }
    factor = base_factor;

    assert(end >= start);
    while(m_scaler.available() < static_cast<std::size_t>(
            (high_latency ?
             TIME_STRETCH_MIN_SAMPLES :
             end - start))) {
        if (rate)
	    factor = base_factor + base_factor * rate_buf[(int) m_ctrl_pos];

	if (backwards != m_fetcher.is_backwards ())
	    m_fetcher.set_backwards (backwards);

	if (factor < 0.2)
	    factor = 0.2;

	must_read = high_latency ? get_info ().block_size : SMALL_BLOCK_SIZE;
	if (factor * m_param_tempo < 1.0)
	    must_read = (float)must_read * factor * m_param_tempo;
	else
	    must_read = must_read;

	m_update_mutex.lock();
	nread = m_fetcher.take (sub_range (range (m_inbuf), 0, must_read));

	if (nread)
        {
	    m_ctrl_pos += (float) nread / (factor * m_param_tempo);
	    if (m_ctrl_pos >= get_info ().block_size)
		m_ctrl_pos = base::phase(m_ctrl_pos) +
                    ((int) m_ctrl_pos % get_info ().block_size);

            m_scaler.set_rate (factor);
	    m_scaler.update (sub_range (range (m_inbuf), 0, nread));
	}

	m_update_mutex.unlock();
    }

    m_update_mutex.lock();
    m_scaler.receive (sub_range (range (m_inbuf), 0, end - start));
    m_update_mutex.unlock();

    copy_and_convert_frames (sub_range (range (m_inbuf), 0, end - start),
                             sub_range (range (buf), start, end - start));
}

void node_sampler::do_advance ()
{
}

void node_sampler::on_info_change ()
{
}

} /* namespace graph */
} /* namespace psynth */
