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

#ifndef PSYNTH_AUDIOINFO_H
#define PSYNTH_AUDIOINFO_H

#include <cstring>

namespace psynth {

/**
 * An audio Sample type.
 */
typedef float sample;

/**
 * Basic information of an audio stream.
 */
struct audio_info
{
    size_t sample_rate;   /**< Sampling rate of the stream. */
    size_t block_size;    /**< Size of processing buffers. */
    size_t num_channels;  /**< Number of channels of the audio stream. */
    
    audio_info (const audio_info& i) :
	sample_rate(i.sample_rate), block_size(i.block_size), num_channels(i.num_channels) {}

    audio_info (size_t r = 0, size_t s = 0, size_t c = 0) :
	sample_rate(r), block_size(s), num_channels(c) {}

    bool operator== (const audio_info& i) {
	return
	    sample_rate == i.sample_rate &&
	    block_size == i.block_size &&
	    num_channels == i.num_channels;
    }

    bool operator!= (const audio_info& i) {
	return
	    sample_rate != i.sample_rate ||
	    block_size != i.block_size ||
	    num_channels != i.num_channels;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_AUDIOINFO_H */

