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

#include <algorithm>
#include <climits>
#include "object/ObjectNoise.h"

namespace psynth
{

ObjectNoise::ObjectNoise(const audio_info& prop,
			 int obj_type,
			 const std::string& name,
			 int n_audio_out,
			 int n_control_out) : 
    Object(prop,
	   obj_type,
	   name,
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   n_audio_out,
	   n_control_out),
    m_param_type(NOISE_PINK),
    m_param_ampl(DEFAULT_AMPL),
    m_b0(0.0f),
    m_b1(0.0f),
    m_b2(0.0f),
    m_b3(0.0f),
    m_b4(0.0f),
    m_b5(0.0f),
    m_b6(0.0f)
{    
    addParam("type", ObjParam::INT, &m_param_type);
    addParam("amplitude", ObjParam::FLOAT, &m_param_ampl);
}

ObjectNoise::~ObjectNoise()
{
}

void ObjectNoise::updateNoise(sample* buf)
{
    const sample_buffer* ampl_buf = getInput<sample_buffer>(LINK_CONTROL, IN_C_AMPLITUDE);
    const sample_buffer* trig_buf = getInput<sample_buffer>(LINK_CONTROL, IN_C_TRIGGER);
    EnvelopeSimple mod_env = getInEnvelope(LINK_CONTROL, IN_C_AMPLITUDE);
    EnvelopeSimple trig_env =  getInEnvelope(LINK_CONTROL, IN_C_TRIGGER);
    
    const sample* ampl = ampl_buf ? ampl_buf->get_data() : NULL;

    int n_samp = getInfo().block_size;
    sample* out = buf;
    if (m_param_type == NOISE_PINK)
	if (ampl)
	    while (n_samp--)
		*out++ = updatePink() * (m_param_ampl + m_param_ampl * *ampl++);
	else
	    while (n_samp--)
		*out++ = updatePink() * m_param_ampl;
    else
	if (ampl)
	    while (n_samp--)
		*out++ = updateWhite() * (m_param_ampl + m_param_ampl * *ampl++);
	else
	    while (n_samp--)
		*out++ = updateWhite() * m_param_ampl;
	
    /* Modulate amplitude with trigger envelope. */
    if (trig_buf) {
	n_samp = getInfo().block_size;
	out = buf;
	const sample* trig = trig_buf->get_data();
	trig_env = getInEnvelope (LINK_CONTROL, IN_C_TRIGGER);
	while (n_samp--) {
	    float env_val = trig_env.update();
	    *out = *out * ((1.0f - env_val) + (env_val * *trig));
	    ++out;
	    ++trig;
	}
    }
}

sample ObjectNoise::updatePink()
{
    sample white;
    sample pink;
    
    white = updateWhite() * 0.2;
		
    m_b0 = 0.99886f * m_b0 + white * 0.0555179f;
    m_b1 = 0.99332f * m_b1 + white * 0.0750759f;
    m_b2 = 0.96900f * m_b2 + white * 0.1538520f;
    m_b3 = 0.86650f * m_b3 + white * 0.3104856f;
    m_b4 = 0.55000f * m_b4 + white * 0.5329522f;
    m_b5 = -0.7616f * m_b5 - white * 0.0168980f;
    pink = m_b0 + m_b1 + m_b2 + m_b3 + m_b4 + m_b5 + m_b6 + white * 0.5362f;
    m_b6 = white * 0.115926f;

    return pink;
}

sample ObjectNoise::updateWhite()
{
    return 1.0f - ((rand() % INT_MAX) / (float)INT_MAX * 2.0);
}   

} /* namespace psynth */

