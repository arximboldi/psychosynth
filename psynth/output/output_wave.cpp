/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#include "common/logger.hpp"
#include "output/output_wave.hpp"

using namespace std;

namespace psynth
{

output_wave::output_wave(const audio_info& info)
    : output(info)
    , m_file_name("")
{
}

output_wave::output_wave(const audio_info& info, const std::string& fname)
    : output(info)
    , m_file_name(fname)
{
}

output_wave::~output_wave()
{
    if (get_state () != NOTINIT)
	close();
}

bool output_wave::open()
{
    if (get_state () == NOTINIT) {
	SF_INFO sfinfo;

	sfinfo.frames	  = 0;
	sfinfo.channels	  = get_info ().num_channels;
	sfinfo.samplerate = get_info ().sample_rate;
	sfinfo.format	  = (SF_FORMAT_AU | SF_FORMAT_PCM_24);

	m_file = sf_open (m_file_name.c_str(), SFM_WRITE, &sfinfo);

	if (m_file == NULL) {
	    logger::instance() ("wave", log::ERROR,
				string("Could not open file ") + m_file_name);
	    return false;
	}
	
	set_state(IDLE);
    }

    return true;
}

bool output_wave::close()
{
    if (get_state () != NOTINIT) {
	sf_close(m_file);
	set_state(NOTINIT);
	return true;
    }

    return false;
}

bool output_wave::put(const audio_buffer& in_buf, size_t nframes)
{
    bool ret = false;
    
    if (get_state () != NOTINIT) {
	float buf [nframes * in_buf.get_info().num_channels];

	in_buf.interleave(buf, nframes);
	sf_writef_float(m_file, buf, nframes);
    }

    return ret;
}

bool output_wave::start()
{
    /* This kind of device does no processing. */
    return false;
}

bool output_wave::stop()
{
    /* This kind of device does no processing. */
    return false;
}

} /* namespace psynth */
