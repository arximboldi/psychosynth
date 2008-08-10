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

#include <algorithm>
#include <iostream>

#include "object/KnownObjects.h"
#include "object/ObjectOscillator.h"

using namespace std;

namespace psynth
{

ObjectOscillator::ObjectOscillator(const AudioInfo& prop,
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
    m_oscillator(prop),
    m_param_wave(OSC_SINE),
    m_param_mod(MOD_FM),
    m_param_freq(DEFAULT_FREQ),
    m_param_ampl(DEFAULT_AMPL),
    m_restart(false)
{    
    addParam("wave", ObjParam::INT, &m_param_wave);
    addParam("modulator", ObjParam::INT, &m_param_mod);
    addParam("frequency", ObjParam::FLOAT, &m_param_freq);
    addParam("amplitude", ObjParam::FLOAT, &m_param_ampl);

    updateOscParams();
}

ObjectOscillator::~ObjectOscillator()
{
}

void ObjectOscillator::updateOscParams()
{
    m_oscillator.setFrequency(m_param_freq);
    m_oscillator.setAmplitude(m_param_ampl);
    m_oscillator.setWave((Oscillator::WaveType)m_param_wave);
    m_oscillator.setModulator((Oscillator::ModType)m_param_mod);
}

void ObjectOscillator::updateOsc(Sample* out)
{
    const ControlBuffer* pitch_buf = getInput<ControlBuffer>(LINK_CONTROL, IN_C_FREQUENCY);
    const ControlBuffer* trig_buf = getInput<ControlBuffer>(LINK_CONTROL, IN_C_TRIGGER);
    EnvelopeSimple mod_env = getInEnvelope(LINK_CONTROL, IN_C_FREQUENCY);
    EnvelopeSimple trig_env =  getInEnvelope(LINK_CONTROL, IN_C_TRIGGER);
    
    const Sample* mod = pitch_buf ? pitch_buf->getData() : NULL;

    updateOscParams();

    /* Fill the output. */
    size_t start = 0;
    size_t end = getInfo().block_size;

    while(start < getInfo().block_size) {
	if (m_restart) {
	    if (trig_buf && trig_buf->getData()[start] != 0.0f)
		m_oscillator.restart();
	    m_restart = false;
	}
	
	if (trig_buf)
	    end = trig_buf->findHill(start);
	
	if (mod) {
	    m_oscillator.update(out + start, mod, mod_env, end - start);
	} else
	    m_oscillator.update(out + start, end - start);

	float env_val = trig_env.update(end - start);
	if (env_val == 1.0f && trig_buf && trig_buf->getData()[end - 1] == 0.0f)
	    m_restart = true;
	    
	start = end;
    }

    /* Modulate amplitude with trigger envelope. */
    if (trig_buf) {
	int n_samp = getInfo().block_size;
	const Sample* trig = trig_buf->getData();
	trig_env = getInEnvelope(LINK_CONTROL, IN_C_TRIGGER);
	while (n_samp--) {
	    float env_val = trig_env.update();
	    *out = *out * ((1.0f - env_val) + (env_val * *trig));
	    ++out;
	    ++trig;
	}
    }
}

} /* namespace psynth */
