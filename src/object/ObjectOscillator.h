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

#ifndef OBJECTOSCILLATOR_H
#define OBJECTOSCILLATOR_H

#include <iostream>
#include <cmath>

#include "object/Object.h"
#include "object/KnownObjects.h"
#include "object/WaveTable.h"

class ObjectOscillator : public Object
{		
public:	
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_FREQUENCY,
	IN_C_AMPLITUDE,
	N_IN_C_SOCKETS
    };
	
    enum OutAudioSocketID {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };
	
    enum OutControlSocketID {
	N_OUT_C_SOCKETS
    };

    enum WaveType {
	OSC_SINE     = WaveTable::SINE,
	OSC_SQUARE   = WaveTable::SQUARE,
	OSC_TRIANGLE = WaveTable::TRIANGLE,
	OSC_SAWTOOTH = WaveTable::SAWTOOTH,
	N_OSC_TYPES
    };

    enum ParamID {
	PARAM_WAVE,
	PARAM_FREQUENCY,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static const float DEFAULT_FREQ = 220.0f;
    static const float DEFAULT_AMPL = 0.5f;
    
private:
    WaveTable& m_table;

    float m_time;
    
    int   m_param_mode;
    float m_param_freq;
    float m_param_ampl;
    float m_old_freq;
	
    void doUpdate();
    
public:
    ObjectOscillator(const AudioInfo& prop, int mode = OSC_SINE);
    ~ObjectOscillator();
};

#endif /* OBJECTOSCILLATOR_H */
