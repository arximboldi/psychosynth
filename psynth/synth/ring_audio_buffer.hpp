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

#ifndef PSYNTH_RING_AUDIO_BUFFER_H
#define PSYNTH_RING_AUDIO_BUFFER_H

#include <psynth/synth/audio_info.hpp>
#include <psynth/synth/audio_buffer.hpp>
#include <psynth/synth/ring_buffer.hpp>

namespace psynth
{

class scaler;

/**
 * A ring buffer of audio data. In contrast with an audio_buffer, the size of the
 * buffer does not need to match the audio_info @c block_size.
 * @see audio_buffer.
 */
class ring_audio_buffer : public ring_buffer
{
public:
	
private:
    audio_info m_info;
    sample** m_data;

    float m_fr_count;
    float m_prev_val;
    
    void allocate();
	
    void liberate() {
	if (m_data) {
	    delete [] *m_data;
	    delete [] m_data;
	}
    }

public:
    /** Constructor. */
    ring_audio_buffer();

    /**
     * Constructor.
     * @param size Size of the buffer.
     */
    ring_audio_buffer(int size);

    /**
     * Constructor.
     * @param info Audio info of the data contained.
     */
    ring_audio_buffer(const audio_info& info);

    /**
     * Constructor.
     * @param info of the contained data.
     */
    ring_audio_buffer(const audio_info& info, int size);

    /** Copy constructor. */
    ring_audio_buffer(const ring_audio_buffer& buf);

    /** Destructor. */
    ~ring_audio_buffer() {
	liberate();
    }

    /** Assignment operator. */
    ring_audio_buffer& operator= (ring_audio_buffer& buf);

    /**
     * Returns the audio_info of the data contained in the buffer.
     */
    const audio_info& getAudioInfo() const {
	return m_info;
    }

    /**
     * Changes the audio_info of the data contained in the buffer. If the
     * number of channels changes the data is destroyed on the resize.
     * @param info The new audio_info of the buffer.
     */
    void set_info (const audio_info& info) {
	if (info.num_channels != m_info.num_channels) {
	    liberate();
	    m_info = info;
	    allocate();
	} else
	    m_info = info;
    }

    /**
     * Changes the audio_info of the data contained in the buffer and
     * the size of the buffer. The buffer data is destroyed.
     * @param info The new audio_info of the buffer.
     * @param new_size The new size of the buffer.
     */
    void set_info (const audio_info& info, int new_size) {
	if (info.num_channels != m_info.num_channels
	    || m_size != new_size) {
	    liberate();
	    m_size = new_size;
	    m_info = info;
	    allocate();
	} else
	    m_info = info;
    }

    /**
     * Changes the buffer size.
     * @param new_size The new size of the buffer.
     */
    void resize (int new_size) {
	liberate();
	m_size = new_size;
	allocate();
    }

    /**
     * Fills an audio_buffer with data from the ring buffer.
     * @param r The position where we want to start reading.
     * @param buf The buffer to fill.
     */
    int read (read_ptr& r, audio_buffer& buf) const {
	return read(r, buf, buf.size());
    };

    /**
     * Fills an audio_buffer with data from the ring buffer.
     * @param r The position where we want to start reading.
     * @param buf The buffer to fill.
     * @param samples The number of samples to write.
     */
    int read (read_ptr& r, audio_buffer& buf, int samples) const;

    /**
     * Writes data comming in interleaved format into the ring buffer.
     * @param buf The buffer with the interleaved data.
     * @param samples The number of samples to write.
     */
    void deinterleave (const sample* buf, int samples);

    /**
     * Writes all the data contained in a audio_buffer into the ring buffer.
     * @param buf The buffer with the data to write.
     */
    void write (const audio_buffer& buf) {
	write(buf, buf.size());
    }

    /**
     * Writes some data contained in a audio_buffer into the ring buffer.
     * @param buf The buffer with the data to write.
     * @param samples The number of samples to write.
     */
    void write (const audio_buffer& buf, int samples);

    /**
     * Writes all the data contained in a audio_buffer into the ring buffer,
     * passing it through a scaler first.
     * @param buf The buffer with the data to write.
     * @param scaler The scaler to apply to the data.
     */
    void write_scaler (const audio_buffer& buf, scaler& scaler) {
	write_scaler(buf, buf.size(), scaler);
    }

    /**
     * Writes some data contained in a audio_buffer into the ring buffer,
     * passing it through a scaler first.
     * @param buf The buffer with the data to write.
     * @param buf The number of samples from the original buffer to write.
     * @param scaler The scaler to apply to the data.
     */
    void write_scaler (const audio_buffer& buf, int samples, scaler& scaler);

    /**
     * Writes the data in the audio_buffer applying some fast resampling to it.
     * @param buf The buffer with the data.
     * @param factor The resampling factor: original_sample_rate / new_sample_rate.
     * @todo This function is not backwards safe yet.
     */
    void write_fast_resample (const audio_buffer& buf, float factor) {
	write_fast_resample (buf, buf.size(), factor);
    }

    /**
     * Writes the data in the audio_buffer applying some fast resampling to it.
     * @param buf The buffer with the data.
     * @param samples The number of samples from the original buffer to write.
     * @param factor The resampling factor: original_sample_rate / new_sample_rate.
     * @todo This function is not backwards safe yet.
     */
    void write_fast_resample(const audio_buffer& buf, int samples, float factor);

    /**
     * Sets all the data in the buffer to zero.
     */
    void zero () {
	if (m_data)
	    memset(*m_data, 0, sizeof(sample) * m_size * m_info.num_channels);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGAUDIOBUFFER_H */
