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

#ifndef PSYNTH_RINGCONTROLBUFFER
#define PSYNTH_RINGCONTROLBUFFER

#include <libpsynth/common/ControlBuffer.h>
#include <libpsynth/common/RingBuffer.h>

namespace psynth
{

/**
 * A ring buffer for control data.
 * @see ControlBuffer.
 */
class RingControlBuffer : public RingBuffer
{
private:
    Sample* m_data;
    float m_fr_count;

    void allocate();
	
    void liberate() {
	delete [] m_data;
    }

public:
    /** Constructor. */
    RingControlBuffer();

    /**
     * Constructor.
     * @param size Size of the ring buffer.
     */
    RingControlBuffer(int size);

    /** Copy constructor. */
    RingControlBuffer(const RingControlBuffer& buf);

    /** Destructor. */
    ~RingControlBuffer() {
	liberate();
    }

    /** Assignment operator. */
    RingControlBuffer& operator= (RingControlBuffer& buf);

    /**
     * Changes the size of the ring buffer destroying any previous data.
     * @param new_size The new size of the buffer.
     */
    void resize(int new_size) {
	liberate();
	m_size = new_size;
	allocate();
    }

    /**
     * Fills a ControlBuffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     */
    int read(ReadPtr& r, ControlBuffer& buf) const {
	return read(r, buf, buf.size());
    };

    /**
     * Fills a ControlBuffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     * @param samples The number of samples to read.
     */
    int read(ReadPtr& r, ControlBuffer& buf, int samples) const;

    /**
     * Write all the data in a ControlBuffer to the ring buffer.
     * @param buf The buffer to write.
     */
    void write(const ControlBuffer& buf) {
	write(buf, buf.size());
    }

    /**
     * Write some data in a ControlBuffer to the ring buffer.
     * @param buf The buffer to write.
     * @param samples The number of samples to write.
     */
    void write(const ControlBuffer& buf, int samples);

    /**
     * Write all the data from a ControlBuffer in the ring buffer applying
     * some resampling to it.
     * @param buf The buffer to write.
     * @param factor The resampling factor: old_sample_rate / new_sample_rate.
     */
    void writeFastResample(const ControlBuffer& buf, float factor) {
	writeFastResample(buf, buf.size(), factor);
    }

    /**
     * Write all the data from a ControlBuffer in the ring buffer applying
     * some resampling to it.
     * @param buf The buffer to write.
     * @param samples Number of samples from the original buffer to write.
     * @param factor The resampling factor: old_sample_rate / new_sample_rate.
     */
    void writeFastResample(const ControlBuffer& buf, int samples, float factor);

    /**
     * Sets to zero all the contents of the buffer.
     */
    void zero() {
	memset(m_data, 0, m_size * sizeof(Sample));
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGCONTROLBUFFER */
