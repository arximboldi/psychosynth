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

#ifndef PSYNTH_AUDIOBUFFER_H
#define PSYNTH_AUDIOBUFFER_H

#include <cstring>

#include <libpsynth/common/audio_info.h>

namespace psynth
{

/**
 * A buffer containing audio data.
 * An AudioBuffer consists of deinterleaved samples. An AudioBuffer differs
 * from a ControlBuffer in that is has multiple channels and may contain
 * information about the contained audio data.
 *
 * @see audio_info
 */
class audio_buffer
{
    audio_info m_info;
    sample** m_data;

    void allocate();
	
    void liberate();

public:
    /** Default constructor. */
    audio_buffer() :
	m_info(0,0,0),
	m_data(NULL) {}

    /** Copy constructor. */
    audio_buffer(const audio_buffer& buf) :
	m_info(buf.m_info) {
	allocate();
	memcpy(*m_data, *buf.m_data, sizeof(sample) * m_info.num_channels * m_info.block_size);
    }
    
    /**
     * Construct the object with some @c audio_info. The buffer will have the same
     * size of @a info.block_size.
     * @param info The @c audio_info of the data contained.
     */
    audio_buffer(const audio_info& info) : 
	m_info(info) {
	allocate();
    }

    /**
     * Construct the object with some @c audio_info. The buffer will be of size
     * @a size, and its audio_info's @c block_size will be set to @a size.
     * @param info The @c audio_info of the data contained.
     * @param size The size of the AudioBuffer.
     */
    audio_buffer (const audio_info& info, int size) : 
	m_info(info.sample_rate, size, info.num_channels) {
	allocate();
    }

    /** Destructor */
    ~audio_buffer () {
	liberate();
    }

    /** Assigment operator. */
    audio_buffer& operator= (const audio_buffer& buf);

    /**
     * Sets the data contained in the buffer to reverse order.
     */
    void reverse () {
	reverse(size());
    }

    /**
     * Sets some data contained in the buffer to reverse order.
     * @param n_frames The number of audio frames to reverse.
     */
    void reverse (int n_frames);

    /**
     * Fills a raw buffer with the data of the buffer in interleaved format.
     * @param dest The buffer to fill.
     * @param n_frames The number of frames to fill.
     */
    void interleave (sample* dest, size_t n_frames) const;

    /**
     * Fills a raw buffer with the data of the buffer in interleaved format
     * and converts the samples to @c char.
     * @param dest The buffer to fill.
     * @param n_frames The number of frames to fill.
     */
    void interleave_c8 (char* dest, size_t n_frames) const;

    /**
     * Fills a raw buffer with the data of the buffer in interleaved format
     * and converts the samples to @c short int.
     * @param dest The buffer to fill.
     * @param n_frames The number of frames to fill.
     */
    void interleave_s16 (short int* dest, size_t n_frames) const;

    /**
     * Fills a raw buffer with the data of the buffer in interleaved format
     * and converts the samples to @c int.
     * @param dest The buffer to fill.
     * @param n_frames The number of frames to fill.
     */
    void interleave_i32 (int* dest, size_t n_frames) const;

    /**
     * Fills this buffer with data comming from a buffer with interleaved
     * samples.
     * @param src The interleaved buffer.
     * @param n_frames the number of samples to fill.
     * @param num_chan The number of channels in the interleaved buffer.
     */
    void deinterleave (const sample* src, size_t n_frames, int num_chan);

    /**
     * Fills this buffer with data comming from a buffer with interleaved
     * samples.
     * @param src The interleaved buffer.
     * @param start The start position where to fill.
     * @param end The last position where to fill.
     * @param num_chan The number of channels in the interleaved buffer.
     */
    void deinterleave (const sample* src, size_t start, size_t end, int num_chan);
    
    /**
     * Fills this buffer with data comming from a buffer with interleaved
     * @c char samples.
     * @param src The interleaved buffer.
     * @param n_frames the number of samples to fill.
     */
    void deinterleave_c8(const char* src, size_t n_frames);

    /**
     * Fills this buffer with data comming from a buffer with interleaved
     * @c short int samples.
     * @param src The interleaved buffer.
     * @param n_frames the number of samples to fill.
     */
    void deinterleave_s16(const short int* src, size_t n_frames);

    /**
     * Fills this buffer with data comming from a buffer with interleaved
     * @c int samples.
     * @param src The interleaved buffer.
     * @param n_frames the number of samples to fill.
     */
    void deinterleave_i32(const int* src, size_t n_frames);

    /**
     * Returns the @c audio_info of the buffer.
     */
    const audio_info& get_info() const {
	return m_info;
    }

    /**
     * Returns the data of a channel of the buffer.
     * @param n The index of the channel.
     */
    sample* get_channel (int n) {
	return m_data[n];
    }

    /**
     * Returns the data of a channel of the buffer. Const version.
     * @param n The index of the channel.
     */
    const sample* get_channel (int n) const {
	return m_data[n];
    }

    /**
     * Returns all the data contained in the buffer.
     */
    sample** get_data() {
	return m_data;
    }

    /**
     * Return all the data contained in the buffer. Const version.
     */
    sample** const get_data() const {
	return m_data;
    }

    /**
     * Returns the data of a channel of the buffer.
     * @param n The index of the channel.
     */
    sample* operator[] (int n) {
	return m_data[n];
    }

    /**
     * Returns the data of a channel of the buffer. Const version.
     * @param n The index of the channel.
     */
    const sample* operator[] (int n) const {
	return m_data[n];
    }

    /**
     * Returns the number of audio frames in the buffer.
     */
    size_t size() const {
	return m_info.block_size;
    }

    /**
     * Resizes the buffer to a new size destroying the old data.
     */
    void resize(size_t new_size) {
	liberate();
	m_info.block_size = new_size;
	allocate();
    }

    /**
     * Sets all samples to zero.
     */
    void zero() {
	memset(*m_data, 0, sizeof(sample) * m_info.block_size * m_info.num_channels);
    }

    /**
     * Changes the @c audio_info of the buffer, resizing it in a destructive way
     * if @c block_size or @num_channels has changed.
     * @param info The new @c audio_info.
     */
    void set_info(const audio_info& info) {
	if (m_info.block_size != info.block_size ||
	    m_info.num_channels != info.num_channels) {
	    liberate();
	    m_info = info;
	    allocate();
	} else
	    m_info = info;	    
    }

    /**
     * Changes the @c audio_info of the buffer, but setting @c block_size to
     * a different value.
     * @param info The new @c audio_info of the buffer.
     * @param size The new size of the buffer.
     */
    void set_info(const audio_info& info, int size) {
	if (m_info.block_size != size ||
	    m_info.num_channels != info.num_channels) {
	    liberate();
	    m_info = info;
	    m_info.block_size = size;
	    allocate();
	} else
	    m_info = info;	    
    }
};

} /* namespace psynth */

#endif /* PSYNTH_AUDIOBUFFER_H */

