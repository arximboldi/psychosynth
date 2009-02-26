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

#ifndef PSYNTH_RING_SAMPLE_BUFFER
#define PSYNTH_RING_SAMPLE_BUFFER

#include <psynth/common/sample_buffer.hpp>
#include <psynth/common/ring_buffer.hpp>

namespace psynth
{

/**
 * A ring buffer for control data.
 * @see sample_buffer.
 */
class ring_sample_buffer : public ring_buffer
{
private:
    sample* m_data;
    float m_fr_count;

    void allocate();
	
    void liberate() {
	delete [] m_data;
    }

public:
    /** Constructor. */
    ring_sample_buffer();

    /**
     * Constructor.
     * @param size Size of the ring buffer.
     */
    ring_sample_buffer(int size);

    /** Copy constructor. */
    ring_sample_buffer(const ring_sample_buffer& buf);

    /** Destructor. */
    ~ring_sample_buffer() {
	liberate();
    }

    /** Assignment operator. */
    ring_sample_buffer& operator= (ring_sample_buffer& buf);

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
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     */
    int read(read_ptr& r, sample_buffer& buf) const {
	return read(r, buf, buf.size());
    };

    /**
     * Fills a sample_buffer with data from the ring buffer.
     * @param r The reader pointer.
     * @param buf The buffer to fill with the data.
     * @param samples The number of samples to read.
     */
    int read(read_ptr& r, sample_buffer& buf, int samples) const;

    /**
     * Write all the data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     */
    void write(const sample_buffer& buf) {
	write(buf, buf.size());
    }

    /**
     * Write some data in a sample_buffer to the ring buffer.
     * @param buf The buffer to write.
     * @param samples The number of samples to write.
     */
    void write(const sample_buffer& buf, int samples);

    /**
     * Write all the data from a sample_buffer in the ring buffer applying
     * some resampling to it.
     * @param buf The buffer to write.
     * @param factor The resampling factor: old_sample_rate / new_sample_rate.
     */
    void write_fast_resample(const sample_buffer& buf, float factor) {
	write_fast_resample(buf, buf.size(), factor);
    }

    /**
     * Write all the data from a sample_buffer in the ring buffer applying
     * some resampling to it.
     * @param buf The buffer to write.
     * @param samples Number of samples from the original buffer to write.
     * @param factor The resampling factor: old_sample_rate / new_sample_rate.
     */
    void write_fast_resample(const sample_buffer& buf, int samples, float factor);

    /**
     * Sets to zero all the contents of the buffer.
     */
    void zero() {
	memset(m_data, 0, m_size * sizeof(sample));
    }
};

} /* namespace psynth */

#endif /* PSYNTH_RINGCONTROLBUFFER */
