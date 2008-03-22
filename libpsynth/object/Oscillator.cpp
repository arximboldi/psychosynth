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

#include <iostream>
#include <object/Oscillator.h>
#include <object/EnvelopeSimple.h>

using namespace std;

namespace psynth
{

bool Oscillator::m_table_init = false;
WaveTable Oscillator::TABLE[Oscillator::WAVE_TYPES];

void Oscillator::initializeTables()
{
    TABLE[SINE].fill(TABLE_SIZE, &computeSine);
    TABLE[SQUARE].fill(TABLE_SIZE, &computeSquare);
    TABLE[TRIANGLE].fill(TABLE_SIZE, &computeTriangle);
    TABLE[SAWTOOTH].fill(TABLE_SIZE, &computeSawtooth);
    TABLE[MOOGSAW].fill(TABLE_SIZE, &computeMoogsaw);
    TABLE[EXP].fill(TABLE_SIZE, &computeExp);
    
    m_table_init = true;
}

void Oscillator::update(Sample* out_buf, size_t n_frames)
{
    float speed = m_freq / m_info.sample_rate;
    for (size_t i = 0; i < n_frames; ++i) {
	*out_buf++ = computeSample(m_x) * m_ampl;
	m_x += speed;
    }

    m_x = phase(m_x);
}

void Oscillator::updateFM(Sample* out_buf, const Sample* mod_buf,
			  EnvelopeSimple& mod_env, size_t n_frames)
{
    for (size_t i = 0; i < n_frames; ++i) {
	*out_buf++ = computeSample(m_x) * m_ampl;
	m_x += (m_freq + m_freq * *mod_buf++ * mod_env.update()) / m_info.sample_rate;
    }

    m_x = phase(m_x);
}

void Oscillator::updatePM(Sample* out_buf, const Sample* mod_buf,
			  EnvelopeSimple& mod_env, size_t n_frames)
{
    float speed = m_freq / m_info.sample_rate;
    for (size_t i = 0; i < n_frames; ++i) {
	*out_buf++ = computeSample(m_x + *mod_buf * mod_env.update()) * m_ampl;
	m_x += speed;
    }

    m_x = phase(m_x);
}

void Oscillator::updateAM(Sample* out_buf, const Sample* mod_buf,
			  EnvelopeSimple& mod_env, size_t n_frames)
{
    float speed = m_freq / m_info.sample_rate;
    for (size_t i = 0; i < n_frames; ++i) {
	*out_buf++ = computeSample(m_x) * (m_ampl + m_ampl * *mod_buf++ * mod_env.update());
	m_x += speed;
    }

    m_x = phase(m_x);
}

} /* namespace psynth */
