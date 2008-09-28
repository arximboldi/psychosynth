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

#include "input/file_reader_wave.h"
#include "common/logger.h"

using namespace std;

namespace psynth
{

void file_reader_wave::open (const std::string& file)
{
    if (!is_open()) {
	SF_INFO sfinfo;
	
	m_file = sf_open (file.c_str (), SFM_READ, &sfinfo);
    
	if (m_file == NULL) {
	    logger::instance() ("wave", log::ERROR, string("Could not open file: ") + file);
	    return;
	}
	
	audio_info info;
	info.sample_rate  = sfinfo.samplerate;
	info.num_channels = sfinfo.channels;
	info.block_size   = sfinfo.frames;
	set_info(info);
		
	set_is_open(true);
    }
}

void file_reader_wave::seek(size_t pos)
{
    sf_seek(m_file, pos, SEEK_SET);
}

int file_reader_wave::read(audio_buffer& outbuf, int n_samples)
{
    int n_read;

    float buf [n_samples * get_info().num_channels];
    
    n_read = sf_readf_float(m_file, buf, n_samples);

    if (n_read)
	outbuf.deinterleave(buf, n_read, get_info().num_channels);
    
    return n_read;
}

void file_reader_wave::close()
{
    if (is_open()) {
	sf_close(m_file);
	set_is_open(false);
    }
}

} /* namespace psynth */
