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

#ifndef PSYNTH_SCALERST_H
#define PSYNTH_SCALERST_H

#include <soundtouch/SoundTouch.h>
#include <libpsynth/common/Scaler.h>

namespace psynth
{

class ScalerST : public Scaler
{
    float m_tempo;
    float m_rate;
    float m_pitch;
    int m_chan;
    
    soundtouch::SoundTouch m_st;
    
public:
    void setTempo(float tempo) {
	m_tempo = tempo;
	m_st.setTempo(tempo);
    }
    
    void setRate(float rate) {
	m_rate = rate;
	m_st.setRate(rate);
    }
    
    void setPitch(float pitch) {
	m_pitch = pitch;
	m_st.setPitch(pitch);
    }
    
    void setChannels(int chan) {
	m_chan = chan;
	m_st.setChannels(chan);
    }

    void setSampleRate(int samplerate) {
	m_st.setSampleRate(samplerate);
    }
    
    float getTempo() {
	return m_tempo;;
    }
    
    float getRate() {
	return m_rate;
    }
    
    float getPitch() {
	return m_pitch;
    }
    
    int getChannels() {
	return m_chan;
    }
    
    int availible() {
	return m_st.numSamples();
    }
    
    int receive(float* data, int samples) {
	m_st.receiveSamples(data, samples);
    }
    
    void update(float* data, int samples) {
	m_st.putSamples(data, samples);
    };
};

} /* namespace psynth */

#endif /* PSYNTH_SCALERST_H */
