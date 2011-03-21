/**
 *  Time-stamp:  <2011-03-21 19:46:52 raskolnikov>
 *
 *  @file        scaler.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  @brief Scaling of audio data.
 */

/*
 *  Copyright (C) 2008, 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
 
#ifndef PSYNTH_SCALERST_H
#define PSYNTH_SCALERST_H

#include <soundtouch/SoundTouch.h>
#include <cstddef>
#include <psynth/sound/forwards.hpp>

namespace psynth
{
namespace synth
{

template <class Range>
class scaler_support;

/**
 * Scaler implementation using the RubberBand library.
 *
 * @see Scaler
 */
template <class Range>
class scaler
{    
public:
    static_assert (scaler_support<Range>::is_supported::value,
                   "Soundtouch supports only interleaved bits32sf ranges.");

    typedef Range range;
    
    /** Constructor. */
    scaler (std::size_t frame_rate)
    {
	_st.clear ();
        _st.setChannels (sound::num_samples<Range>::value);
        set_frame_rate (frame_rate);
    }

    /**
     * Sets the tempo factor.
     * @param tempo The new tempo factor.
     */
    void set_tempo (float tempo)
    {
	_tempo = tempo;
	_st.setTempo(tempo);
    }

    /**
     * Sets the new sampling rate factor.
     * @param rate The new rate factor.
     */
    void set_rate (float rate)
    {
	_rate = rate;
	_st.setRate(rate);
    }

    /**
     * Sets the new pitch factor.
     * @param pitch The new pitch factor.
     */
    void set_pitch (float pitch)
    {
	_pitch = pitch;
	_st.setPitch(pitch);
    }

    /**
     * Sets the sample rate of the original signal.
     * @param samplerate The sampling rate.
     */
    void set_frame_rate (int samplerate) {
	_st.setSampleRate (samplerate);
    }

    /**
     * Returns the tempo scaling factor.
     */
    float tempo ()
    { return _tempo; }

    /**
     * Returns the rate scaling factor.
     */
    float rate ()
    { return _rate; }

    /**
     * Returns the pitch scaling factor.
     */
    float pitch ()
    { return _pitch; }

    /**
     * Returns the ammount of data availible in the scaler.
     */
    std::size_t available ()
    { return _st.numSamples(); }

    /**
     * Pops some data already processed in the scaler.
     * @param data The buffer to store the data. It will be stored in
     * interleaved format.
     * @param samples The maximum number of samples to receive.
     * @return The actual number of samples received.
     */
    std::size_t receive (const Range& data)
    { return _st.receiveSamples ((soundtouch::SAMPLETYPE*) &data[0][0], data.size ()); }

    /**
     * Push some data for scaling.
     * @param data The buffer with the data to scale in interleaved format.
     * @param samples The number of samples to push.
     */
    void update (const Range& data)
    { _st.putSamples ((soundtouch::SAMPLETYPE*) &data[0][0], data.size ()); }

    /**
     * Clears all the data left in the scaler.
     */
    void clear ()
    {
	_st.clear();
    }

public:
    float _tempo;
    float _rate;
    float _pitch;

    soundtouch::SoundTouch  _st;
};

} /* namespace synth */
} /* namespace psynth */

#include <psynth/synth/scaler.tpp>

#endif /* PSYNTH_SCALERST_H */
