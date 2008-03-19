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

#include "common/Logger.h"
#include "output/OutputWave.h"

using namespace std;

namespace psynth
{

OutputWave::OutputWave(const AudioInfo& info) :
    Output(info),
    m_file_name("")
{
}

OutputWave::OutputWave(const AudioInfo& info, const std::string& fname) :
    Output(info),
    m_file_name(fname)
{
}

OutputWave::~OutputWave()
{
    if (getState() != NOTINIT)
	close();
}

bool OutputWave::open()
{
    if (getState() == NOTINIT) {
	SF_INFO sfinfo;

	sfinfo.frames	  = 0;
	sfinfo.channels	  = getInfo().num_channels;
	sfinfo.samplerate = getInfo().sample_rate;
	sfinfo.format	  = (SF_FORMAT_AU | SF_FORMAT_PCM_24);

	m_file = sf_open (m_file_name.c_str(), SFM_WRITE, &sfinfo);

	if (m_file == NULL) {
	    Logger::instance().log("wave", Log::ERROR,
				   string("Could not open file ") + m_file_name);
	    return false;
	}
	
	setState(IDLE);
    }

    return true;
}

bool OutputWave::close()
{
    if (getState() != NOTINIT) {
	sf_close(m_file);
	setState(NOTINIT);
	return true;
    }

    return false;
}

bool OutputWave::put(const AudioBuffer& in_buf, size_t nframes)
{
    bool ret = false;
    
    if (getState() != NOTINIT) {
	float buf [nframes * in_buf.getInfo().num_channels];

	in_buf.interleave(buf, nframes);
	sf_writef_float(m_file, buf, nframes);
    }

    return ret;
}

bool OutputWave::start()
{
    /* This kind of device does no processing. */
    return false;
}

bool OutputWave::stop()
{
    /* This kind of device does no processing. */
    return false;
}

} /* namespace psynth */
