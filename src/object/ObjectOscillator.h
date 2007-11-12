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
	
    enum OutControlSocketID{
	N_OUT_C_SOCKETS
    };

    enum WaveType {
	OSC_SINE,
	OSC_SQUARE,
	OSC_TRIANGLE,
	OSC_SAWTOOTH,
	N_OSC_TYPES
    };

    enum ParamID {
	PARAM_WAVE,
	PARAM_FREQUENCY,
	PARAM_AMPLITUDE,
	N_PARAM
    };

private:
    const static int DEFAULT_FREQ = 110;
    const static float  DEFAULT_AMPL = 1;
    const static int TABLE_SIZE = 1<<12;

    static Sample m_table[N_OSC_TYPES][TABLE_SIZE];
    static bool m_initialized;
    
    float m_time;
    
    int   m_param_mode;
    float m_param_freq;
    float m_param_ampl;
    float m_old_freq;
	
    void doUpdate();
    void initializeSine(Sample* tab, int size);
    void initializeTriangle(Sample* tab, int size);
    void initializeSquare(Sample* tab, int size);
    void initializeSawtooth(Sample* tab, int size);
    
public:
    ObjectOscillator(const AudioInfo& prop, int mode = OSC_SINE);
    ~ObjectOscillator();
};

#define NINT(f) ((f)-floor(f) < ceil(f)-(f) ? floor(f) : ceil(f))

#define MPI 3.14159265

#endif /* OBJECTOSCILLATOR_H */
