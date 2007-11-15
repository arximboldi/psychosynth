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

#include <iostream>

#include "object/KnownObjects.h"
#include "object/ObjectOscillator.h"

using namespace std;

bool ObjectOscillator::m_initialized = false;
Sample ObjectOscillator::m_table[ObjectOscillator::N_OSC_TYPES][ObjectOscillator::TABLE_SIZE];

void ObjectOscillator::initializeSine(Sample* tab, int size)
{
    for (int i = 0; i < size; i++)
	*tab++ = sin(i * 2*M_PI/size);
}

void ObjectOscillator::initializeTriangle(Sample* tab, int size)
{
    int i;
    for (i = 0; i < size/4; i++)
	*tab++ = (Sample)i / (size/4);
    for (i = size/4; i > -size/4; i--)
	*tab++ = (Sample)i / (size/4);
    for (i = -size/4; i < 0; i++)
	*tab++ = (Sample)i / (size/4);
}

void ObjectOscillator::initializeSquare(Sample* tab, int size)
{
    for (int i = 0; i < size/2; i++)
	*tab++ = 0;
    for (int i = 0; i < size/2; i++)
	*tab++ = 1;
}

void ObjectOscillator::initializeSawtooth(Sample* tab, int size)
{
    for (int i = -size/2; i < size/2; i++)
	*tab++ = (Sample)i / (size/2);
}

ObjectOscillator::ObjectOscillator(const AudioInfo& prop, int mode) : 
    Object(prop,
	   OBJ_OSCILLATOR,
	   N_PARAM,
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_time(0),
    m_param_mode(mode),
    m_param_freq(DEFAULT_FREQ),
    m_param_ampl(DEFAULT_AMPL),
    m_old_freq(DEFAULT_FREQ)
{
    if (!m_initialized) {
	initializeSine(m_table[OSC_SINE], TABLE_SIZE);
	initializeTriangle(m_table[OSC_TRIANGLE], TABLE_SIZE);
	initializeSawtooth(m_table[OSC_SAWTOOTH], TABLE_SIZE);
	initializeSquare(m_table[OSC_SQUARE], TABLE_SIZE);
	m_initialized = true;
    }
    
    configureParam(PARAM_WAVE, PARAM_INT, &m_param_mode);
    configureParam(PARAM_FREQUENCY, PARAM_FLOAT, &m_param_freq);
    configureParam(PARAM_AMPLITUDE, PARAM_FLOAT, &m_param_ampl);
}

ObjectOscillator::~ObjectOscillator()
{
}

#define NINT(f) ((f)-floor(f) < ceil(f)-(f) ? floor(f) : ceil(f))

#define MPI 3.14159265

void ObjectOscillator::doUpdate()
{
    AudioBuffer* buf = getOutput<AudioBuffer>(LINK_AUDIO, OUT_A_OUTPUT);
    Sample* chan = (*buf)[0];
    float rate = getAudioInfo().sample_rate;
    size_t size = getAudioInfo().block_size;
    float speed = TABLE_SIZE * m_param_freq  / rate;
    size_t i;

    for (i = 0; i < size; ++i, ++chan) {
	while ( m_time < 0.0 )
	    m_time += TABLE_SIZE;
	while ( m_time >= TABLE_SIZE )
	    m_time -= TABLE_SIZE;

	size_t index = m_time;
	size_t alpha = m_time - index;

	*chan = m_table[m_param_mode][index];
	*chan += alpha * (m_table[m_param_mode][++index] - *chan);
	*chan *= m_param_ampl;
	
	//cout << "val: " << *chan << " m_time: " << m_time << endl;

	m_time += speed;
    }

    for (i = 1; i < (size_t)getAudioInfo().num_channels; i++)
	memcpy((*buf)[i], (*buf)[0], sizeof(Sample) * getAudioInfo().block_size);
}
