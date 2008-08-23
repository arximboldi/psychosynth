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

#ifndef PSYNTH_OBJECTOSCILLATOR_H
#define PSYNTH_OBJECTOSCILLATOR_H

#include <iostream>
#include <cmath>

#include <libpsynth/object/node.h>
#include <libpsynth/object/KnownObjects.h>
#include <libpsynth/object/oscillator.h>

namespace psynth
{

class ObjectOscillator : public node
{		
public:	
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_FREQUENCY,
	IN_C_AMPLITUDE,
	IN_C_TRIGGER,
	N_IN_C_SOCKETS
    };

    enum WaveType {
	OSC_SINE     = oscillator::SINE,
	OSC_SQUARE   = oscillator::SQUARE,
	OSC_TRIANGLE = oscillator::TRIANGLE,
	OSC_SAWTOOTH = oscillator::SAWTOOTH,
	OSC_MOOGSAW  = oscillator::MOOGSAW,
	OSC_EXP      = oscillator::EXP,
	N_OSC_TYPES
    };

    enum ModType {
	MOD_FM = oscillator::FM,
	MOD_AM = oscillator::AM,
	MOD_PM = oscillator::PM,
	N_MOD_TYPES
    };
    
    enum ParamID {
	PARAM_WAVE = node::N_COMMON_PARAMS,
	PARAM_MODULATOR,
	PARAM_FREQUENCY,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static const float DEFAULT_FREQ = 110.0f;
    static const float DEFAULT_AMPL = 0.4f;
    
protected:
    oscillator m_oscillator;

    void updateOscParams();
    void updateOsc(sample* out);
    
private:
    int   m_param_wave;
    int   m_param_mod;
    float m_param_freq;
    float m_param_ampl;
    bool  m_restart;
    
public:
    ObjectOscillator(const audio_info& prop,
		     int obj_type,
		     const std::string& name,
		     int n_audio_out,
		     int n_control_out);
    
    ~ObjectOscillator();
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTOSCILLATOR_H */
