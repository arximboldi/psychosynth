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

#ifndef PSYNTH_RINGAUDIOBUFFER_H
#define PSYNTH_RINGAUDIOBUFFER_H

#include <libpsynth/common/AudioInfo.h>
#include <libpsynth/common/AudioBuffer.h>
#include <libpsynth/common/RingBuffer.h>

namespace psynth
{

class Scaler;

/**
 * A ring buffer of audio data. In contrast with an AudioBuffer, the size of the
 * buffer does not need to match the AudioInfo @c block_size.
 * @see AudioBuffer.
 */
class RingAudioBuffer : public RingBuffer
{
public:
	
private:
    AudioInfo m_info;
    Sample** m_data;

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
    RingAudioBuffer();

    /**
     * Constructor.
     * @param size Size of the buffer.
     */
    RingAudioBuffer(int size);

    /**
     * Constructor.
     * @param info Audio info of the data contained.
     */
    RingAudioBuffer(const AudioInfo& info);

    /**
     * Constructor.
     * @param info of the contained data.
     */
    RingAudioBuffer(const AudioInfo& info, int size);

    /** Copy constructor. */
    RingAudioBuffer(const RingAudioBuffer& buf);

    /** Destructor. */
    ~RingAudioBuffer() {
	liberate();
    }

    /** Assignment operator. */
    RingAudioBuffer& operator= (RingAudioBuffer& buf);

    /**
     * Returns the AudioInfo of the data contained in the buffer.
     */
    const AudioInfo& getAudioInfo() const {
	return m_info;
    }

    /**
     * Changes the AudioInfo of the data contained in the buffer. If the
     * number of channels changes the data is destroyed on the resize.
     * @param info The new AudioInfo of the buffer.
     */
    void setAudioInfo(const AudioInfo& info) {
	if (info.num_channels != m_info.num_channels) {
	    liberate();
	    m_info = info;
	    allocate();
	} else
	    m_info = info;
    }

    /**
     * Changes the AudioInfo of the data contained in the buffer and
     * the size of the buffer. The buffer data is destroyed.
     * @param info The new AudioInfo of the buffer.
     * @param new_size The new size of the buffer.
     */
    void setAudioInfo(const AudioInfo& info, int new_size) {
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
    void resize(int new_size) {
	liberate();
	m_size = new_size;
	allocate();
    }

    /**
     * Fills an AudioBuffer with data from the ring buffer.
     * @param r The position where we want to start reading.
     * @param buf The buffer to fill.
     */
    int read(ReadPtr& r, AudioBuffer& buf) const {
	return read(r, buf, buf.size());
    };

    /**
     * Fills an AudioBuffer with data from the ring buffer.
     * @param r The position where we want to start reading.
     * @param buf The buffer to fill.
     * @param samples The number of samples to write.
     */
    int read(ReadPtr& r, AudioBuffer& buf, int samples) const;

    /**
     * Writes data comming in interleaved format into the ring buffer.
     * @param buf The buffer with the interleaved data.
     * @param samples The number of samples to write.
     */
    void deinterleave(const Sample* buf, int samples);

    /**
     * Writes all the data contained in a AudioBuffer into the ring buffer.
     * @param buf The buffer with the data to write.
     */
    void write(const AudioBuffer& buf) {
	write(buf, buf.size());
    }

    /**
     * Writes some data contained in a AudioBuffer into the ring buffer.
     * @param buf The buffer with the data to write.
     * @param samples The number of samples to write.
     */
    void write(const AudioBuffer& buf, int samples);

    /**
     * Writes all the data contained in a AudioBuffer into the ring buffer,
     * passing it through a scaler first.
     * @param buf The buffer with the data to write.
     * @param scaler The Scaler to apply to the data.
     */
    void writeScaler(const AudioBuffer& buf, Scaler& scaler) {
	writeScaler(buf, buf.size(), scaler);
    }

    /**
     * Writes some data contained in a AudioBuffer into the ring buffer,
     * passing it through a scaler first.
     * @param buf The buffer with the data to write.
     * @param buf The number of samples from the original buffer to write.
     * @param scaler The Scaler to apply to the data.
     */
    void writeScaler(const AudioBuffer& buf, int samples, Scaler& scaler);

    /**
     * Writes the data in the AudioBuffer applying some fast resampling to it.
     * @param buf The buffer with the data.
     * @param factor The resampling factor: original_sample_rate / new_sample_rate.
     */
    void writeFastResample(const AudioBuffer& buf, float factor) {
	writeFastResample(buf, buf.size(), factor);
    }

    /**
     * Writes the data in the AudioBuffer applying some fast resampling to it.
     * @param buf The buffer with the data.
     * @param samples The number of samples from the original buffer to write.
     * @param factor The resampling factor: original_sample_rate / new_sample_rate.
     */
    void writeFastResample(const AudioBuffer& buf, int samples, float factor);

    /**
     * Sets all the data in the buffer to zero.
     */
    void zero() {
	if (m_data)
	    memset(*m_data, 0, sizeof(Sample) * m_size * m_info.num_channels);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGAUDIOBUFFER_H */
