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

#ifndef OBJECTLFO_H
#define OBJECTLFO_H

#include "object/Object.h"
#include "object/KnownObjects.h"
#include "object/WaveTable.h"

class ObjectLFO : public Object
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
	N_OUT_A_SOCKETS
    };
	
    enum OutControlSocketID {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };

    enum WaveType {
	LFO_SINE     = WaveTable::SINE,
	LFO_SQUARE   = WaveTable::SQUARE,
	LFO_TRIANGLE = WaveTable::TRIANGLE,
	LFO_SAWTOOTH = WaveTable::SAWTOOTH,
	N_LFO_TYPES
    };

    enum ParamID {
	PARAM_WAVE,
	PARAM_FREQUENCY,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static const float DEFAULT_FREQ = 2.0f;
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
    ObjectLFO(const AudioInfo& prop, int mode = LFO_SINE);
    ~ObjectLFO();
};

#endif /* OBJECTLFO_H */
