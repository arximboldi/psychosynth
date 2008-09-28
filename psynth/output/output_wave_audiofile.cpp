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

#include "common/logger.h"
#include "output/output_wave.h"

using namespace std;

namespace psynth
{

output_wave::output_wave(const audio_info& info)
    : output (info)
    , m_file_name ("")
{
}

output_wave::output_wave(const audio_info& info, const std::string& fname)
    : output (info)
    , m_file_name (fname)
{
}

output_wave::~output_wave()
{
    if (getState() != NOTINIT)
	close();
}

bool output_wave::open()
{
    if (getState() == NOTINIT) {
	m_af_setup = afNewFileSetup();
	afInitFileFormat(m_af_setup, AF_FILE_WAVE);
	afInitByteOrder(m_af_setup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
	afInitChannels(m_af_setup, AF_DEFAULT_TRACK, get_info ().num_channels);
	afInitRate(m_af_setup, AF_DEFAULT_TRACK, get_info ().sample_rate);
	afInitSampleFormat(m_af_setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);

	m_af_file = afOpenFile(m_file_name.c_str(), "w", m_af_setup);

	if (m_af_file == NULL) {
	    logger::instance() ("wav_out", log::ERROR, "Could not open file.");

	    afFreeFileSetup(m_af_setup);
	    return false;
	}

	m_buf = new short int[get_info ().block_size * get_info ().num_channels];
	setState(IDLE);
    }

    return true;
}

bool output_wave::close()
{
    if (getState() != NOTINIT) {
	afCloseFile(m_af_file);
	afFreeFileSetup(m_af_setup);

	delete [] m_buf;
	return true;
    }

    return false;
}

bool output_wave::put(const audio_buffer& in_buf, size_t nframes)
{
    bool ret = false;
    
    if (getState() != NOTINIT) {
	size_t copyframes = get_info ().block_size;

	ret = true;
	while (nframes > 0) {
	    if (nframes < copyframes)
		copyframes = nframes;

	    in_buf.interleave_s16 (m_buf, copyframes);
	    afWriteFrames(m_af_file, AF_DEFAULT_TRACK, m_buf, copyframes);

	    nframes -= copyframes;
	}
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
