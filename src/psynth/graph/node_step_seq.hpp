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

#ifndef PSYNTH_OBJECTSTEPSEQ_H
#define PSYNTH_OBJECTSTEPSEQ_H

#include <psynth/graph/node.hpp>
#include <psynth/graph/node_factory.hpp>
#include <psynth/synth/multi_point_envelope.hpp>

namespace psynth
{
namespace graph
{

class node_step_seq : public node0
{
public:
    static constexpr int MAX_STEPS = 32;
    static constexpr float DEFAULT_BPM = 260.0;
    static constexpr int DEFAULT_NUM_STEPS = 12;
    static constexpr int DEFAULT_STEP = true;
    static constexpr float DEFAULT_HIGH = 0.8f;
    static constexpr float DEFAULT_SLOPE = 0.1f;

    enum shape {
	SHAPE_SQUARE,
	SHAPE_TRIANGLE,
	SHAPE_FWSAWTOOTH,
	SHAPE_BWSAWTOOTH,
	N_SHAPES
    };

    enum in_audio_socket_id {
	N_IN_A_SOCKETS
    };

    enum in_control_socket_id {
	IN_C_BPM,
	N_IN_C_SOCKETS
    };

    enum param_id {
	PARAM_BPM = node0::N_COMMON_PARAMS,
	PARAM_SHAPE,
	PARAM_HIGH,
	PARAM_SLOPE,
	PARAM_CURRENT_STEP,
	PARAM_NUM_STEPS,
	PARAM_STEP_0,
	PARAM_STEP_1,
	PARAM_STEP_2,
	PARAM_STEP_3,
	PARAM_STEP_4,
	PARAM_STEP_5,
	PARAM_STEP_6,
	PARAM_STEP_7,
	PARAM_STEP_8,
	PARAM_STEP_9,
	PARAM_STEP_10,
	PARAM_STEP_11,
	PARAM_STEP_12,
	PARAM_STEP_13,
	PARAM_STEP_14,
	PARAM_STEP_15,
	PARAM_STEP_16,
	PARAM_STEP_17,
	PARAM_STEP_18,
	PARAM_STEP_19,
	PARAM_STEP_20,
	PARAM_STEP_21,
	PARAM_STEP_22,
	PARAM_STEP_23,
	PARAM_STEP_24,
	PARAM_STEP_25,
	PARAM_STEP_26,
	PARAM_STEP_27,
	PARAM_STEP_28,
	PARAM_STEP_29,
	PARAM_STEP_30,
	PARAM_STEP_31,
	N_PARAM,
    };

    enum out_audio_socket_id {
	N_OUT_A_SOCKETS
    };

    enum out_constrol_socket_id {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };

private:
    void do_update (const node0* caller, int caller_port_type, int caller_port);
    void do_advance ();
    void on_info_change ();
    void create_shape ();
    void update_shape ();

    float m_param_bpm;
    int m_param_shape;
    float m_param_high;
    float m_param_slope;
    int m_param_num_steps;
    int m_param_step[MAX_STEPS];

    float m_old_param_high;
    int m_old_param_shape;

    synth::envelope_values<sound::bits32sf> m_hi_env_vals;
    synth::envelope_values<sound::bits32sf> m_lo_env_vals;
    synth::multi_point_envelope<sample_range> m_env;
    int m_cur_step;

    void init_envelope_values ();
    void update_envelope_values ();
    void update_envelope_factor (float mod);

public:
    node_step_seq(const audio_info& info);
};

PSYNTH_DECLARE_NODE_FACTORY(node_step_seq, "stepseq");

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTSTEPSEQ_H */
