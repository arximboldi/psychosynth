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

#ifndef PSYNTH_NODE_DOUBLE_SAMPLER_H
#define PSYNTH_NODE_DOUBLE_SAMPLER_H

#include <psynth/graph/node.hpp>
#include <psynth/io/file_input.hpp>
#include <psynth/io/caching_file_input.hpp>
#include <psynth/graph/node_factory.hpp>
#include <psynth/synth/scaler.hpp>

namespace psynth
{
namespace graph
{

class node_double_sampler : public node0
{
public:
    enum in_audio_socket_id {
	N_IN_A_SOCKETS
    };

    enum in_control_socket_id {
	IN_C_BLEND,
	IN_C_TRIGGER,
	N_IN_C_SOCKETS
    };

    enum param_id {
	PARAM_FILE_ONE = node0::N_COMMON_PARAMS,
	PARAM_FILE_TWO,
	PARAM_AMPLITUDE,
	PARAM_BLEND,
	N_PARAM
    };

    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

    enum out_control_socket_id {
	N_OUT_C_SOCKETS
    };

private:
    typedef sound::stereo32sf_range  interleaved_range;
    typedef sound::stereo32sf_buffer interleaved_buffer;

    io::file_input_ptr<interleaved_range> m_reader_one;
    io::caching_file_input_adapter<interleaved_range,
                                   decltype (m_reader_one)>
    m_fetcher_one;
    io::file_input_ptr<interleaved_range> m_reader_two;
    io::caching_file_input_adapter<interleaved_range, decltype (m_reader_two)>
    m_fetcher_two;

    interleaved_buffer m_inbuf_one;
    interleaved_buffer m_inbuf_two;

    float m_param_ampl;
    float m_param_blend;

    bool m_restart;

    std::string m_param_file_one;
    std::string m_param_file_two;

    std::mutex m_update_mutex;

    void on_file_one_change (node_param& par);
    void on_file_two_change (node_param& par);
    void read (audio_buffer& buf, int start, int end);
    void restart();

    void do_update (const node0* caller, int caller_port_type, int caller_port);
    void do_advance ();
    void on_info_change ();

public:
    node_double_sampler (const audio_info& info);
    ~node_double_sampler ();
};

PSYNTH_DECLARE_NODE_FACTORY (node_double_sampler, "double_sampler");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTSAMPLER_H */
