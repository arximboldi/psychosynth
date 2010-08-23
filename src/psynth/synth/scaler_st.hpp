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
#include <psynth/synth/scaler.hpp>

namespace psynth
{

/**
 * Scaler implementation using the SoundTouch library.
 *
 * @see Scaler
 */
class scaler_st : public scaler
{
    float m_tempo;
    float m_rate;
    float m_pitch;
    int m_chan;
    
    soundtouch::SoundTouch m_st;
    
public:
    /** Constructor. */
    scaler_st () {
	m_st.clear();
    }

    /**
     * Sets the tempo factor.
     * @param tempo The new tempo factor.
     */
    void set_tempo(float tempo) {
	m_tempo = tempo;
	m_st.setTempo(tempo);
    }

    /**
     * Sets the new sampling rate factor.
     * @param rate The new rate factor.
     */
    void set_rate(float rate) {
	m_rate = rate;
	m_st.setRate(rate);
    }

    /**
     * Sets the new pitch factor.
     * @param pitch The new pitch factor.
     */
    void set_pitch(float pitch) {
	m_pitch = pitch;
	m_st.setPitch(pitch);
    }

    /**
     * Sets the new number of channels.
     * @param chan The new number of channels.
     */
    void set_channels(int chan) {
	m_chan = chan;
	m_st.setChannels(chan);
    }

    /**
     * Sets the sample rate of the original signal.
     * @param samplerate The sampling rate.
     */
    void set_sample_rate(int samplerate) {
	m_st.setSampleRate(samplerate);
    }

    /**
     * Returns the tempo scaling factor.
     */
    float get_tempo() {
	return m_tempo;;
    }

    /**
     * Returns the rate scaling factor.
     */
    float get_rate() {
	return m_rate;
    }

    /**
     * Returns the pitch scaling factor.
     */
    float get_pitch() {
	return m_pitch;
    }

    /**
     * Returns the number of channels of the scaler.
     */
    int get_channels() {
	return m_chan;
    }

    /**
     * Returns the ammount of data availible in the scaler.
     */
    int availible() {
	return m_st.numSamples();
    }

    /**
     * Pops some data already processed in the scaler.
     * @param data The buffer to store the data. It will be stored in
     * interleaved format.
     * @param samples The maximum number of samples to receive.
     * @return The actual number of samples received.
     */
    int receive (float* data, int samples) {
	return m_st.receiveSamples(data, samples);
    }

    /**
     * Push some data for scaling.
     * @param data The buffer with the data to scale in interleaved format.
     * @param samples The number of samples to push.
     */
    void update (float* data, int samples) {
	m_st.putSamples(data, samples);
    };

    /**
     * Clears all the data left in the scaler.
     */
    void clear() {
	m_st.clear();
    }
};

} /* namespace psynth */

#endif /* PSYNTH_SCALERST_H */
