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

namespace psynth {

/**
 * An audio Sample type.
 */
typedef float Sample;

/**
 * Basic information of an audio stream.
 */
struct AudioInfo
{
    int sample_rate;   /**< Sampling rate of the stream. */
    int block_size;    /**< Size of processing buffers. */
    int num_channels;  /**< Number of channels of the audio stream. */
    
    AudioInfo(const AudioInfo& i) :
	sample_rate(i.sample_rate), block_size(i.block_size), num_channels(i.num_channels) {}

    AudioInfo(int r = 0, int s = 0, int c = 0) :
	sample_rate(r), block_size(s), num_channels(c) {}

    bool operator== (const AudioInfo& i) {
	return
	    sample_rate == i.sample_rate &&
	    block_size == i.block_size &&
	    num_channels == i.num_channels;
    }

    bool operator!= (const AudioInfo& i) {
	return
	    sample_rate != i.sample_rate ||
	    block_size != i.block_size ||
	    num_channels != i.num_channels;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_AUDIOINFO_H */

