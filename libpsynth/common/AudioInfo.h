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

typedef float Sample;

struct AudioInfo {
	int sample_rate;
	int block_size;
	int num_channels;
		
	AudioInfo(const AudioInfo& i) :
		sample_rate(i.sample_rate), block_size(i.block_size), num_channels(i.num_channels) {}
	AudioInfo(int r = 0, int s = 0, int c = 0) :
		sample_rate(r), block_size(s), num_channels(c) {}
};

} /* namespace psynth */

#endif /* PSYNTH_AUDIOINFO_H */

