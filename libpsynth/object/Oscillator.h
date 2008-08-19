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

#ifndef PSYNTH_OSCILLATOR_H
#define PSYNTH_OSCILLATOR_H

#include <cmath>

#include <libpsynth/common/audio_buffer.h>
#include <libpsynth/object/WaveTable.h>

namespace psynth
{

/* TODO: Find a way to avoid this. */
class EnvelopeSimple;

class Oscillator
{
public:
    enum WaveType {
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
	MOOGSAW,
	EXP,
	WAVE_TYPES
    };

    enum ModType {
	FM,
	AM,
	PM,
	MOD_TYPES
    };

private:
    static inline float phase(float x) {
	return 	x - (x >= 0.0f ? static_cast<int>(x) :
		     static_cast<int>(x) - 1);
    }

    static const size_t TABLE_SIZE = 1 << 12;
    static WaveTable TABLE[WAVE_TYPES];
    static bool m_table_init;

    audio_info m_info;
    WaveType m_type;
    ModType m_mod;
    float m_x;
    float m_freq;
    float m_ampl;
    bool m_wave_table;

    void initializeTables();

    void updateFM(sample* out_buf, const sample* mod_buf,
		  EnvelopeSimple& mod_env, size_t n_frames);
    void updateAM(sample* out_buf, const sample* mod_buf,
		  EnvelopeSimple& mod_env, size_t n_frames);
    void updatePM(sample* out_buf, const sample* mod_buf,
		  EnvelopeSimple& mod_env, size_t n_frames);    

public:
    Oscillator(const audio_info& info,
	       float freq = 220.0f,
	       float ampl = 1.0f,
	       float phase = 0.0f,
	       WaveType type = SINE,
	       ModType mod = FM,
	       bool wave_table = true) :
	m_info(info),
	m_type(type),
	m_mod(mod),
	m_x(phase),
	m_freq(freq),
	m_ampl(ampl),
	m_wave_table(wave_table) {
	if (wave_table && !m_table_init)
	    initializeTables();
    }

    void restart() {
	m_x = 0.0f;
    }
    
    static sample computeSine(float x) {
	return sin(x * 2 * M_PI);
    }
    
    static sample computeSquare(float x) {
	return phase(x) > 0.5f ? -1.0 : 1.0;
    }
    
    static sample computeTriangle(float x) {
	float p = phase(x);
	if(p <= 0.25f)
	    return p * 4.0f;
	else if(p <= 0.75f)
	    return 2.0f - p * 4.0f;
	return p * 4.0f - 4.0f;
    }
    
    static sample computeSawtooth(float x) {
	return -1.0f + phase(x) * 2.0f;
    }
    
    static sample computeMoogsaw(float x) {
	float p = phase(x);
	if(p < 0.5f )
	    return -1.0f + p * 4.0f;
	return 1.0f - 2.0f * p;
    }
    
    static sample computeExp(float x) {
	float p = phase(x);
	if(p > 0.5f)
	    p = 1.0f - p;
	return -1.0f + 8.0f * p * p;
    }

    void setInfo(const audio_info& info) {
	m_info = info;
    }

    void setFrequency(float freq) {
	m_freq = freq;
    }

    void setAmplitude(float ampl) {
	m_ampl = ampl;
    }

    void setWave(WaveType type) {
	m_type = type;
    }

    void setModulator(ModType mod) {
	m_mod = mod;
    }
    
    void setWaveTable(bool wave_table) {
	if (wave_table && !m_table_init)
	    initializeTables();
	m_wave_table = wave_table;
    }

    void update(sample* out_buf, size_t n_frames);
        
    void update(sample* out_buf, const sample* mod_buf,
		EnvelopeSimple& mod_env, size_t n_frames) {
	switch(m_mod) {
	case AM:
	    updateAM(out_buf, mod_buf, mod_env, n_frames);
	    break;
	case FM:
	    updateFM(out_buf, mod_buf, mod_env, n_frames);
	    break;
	case PM:
	    updatePM(out_buf, mod_buf, mod_env, n_frames);
	    break;
	default:
	    break;
	}
    };
    
    sample computeSample(float x) {
	if (m_wave_table)
	    return TABLE[m_type].get(x);
	else
	    switch(m_type) {
	    case SINE:
		return computeSine(x);
		break;
	    case SQUARE:
		return computeSquare(x);
		break;
	    case TRIANGLE:
		return computeTriangle(x);
		break;
	    case SAWTOOTH:
		return computeSawtooth(x);
		break;
	    case MOOGSAW:
		return computeMoogsaw(x);
		break;
	    case EXP:
		return computeExp(x);
		break;
	    default:
		return 0.0f;
		break;
	    }
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OSCILLATOR_H */
