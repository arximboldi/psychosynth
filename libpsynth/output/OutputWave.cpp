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
	m_af_setup = afNewFileSetup();
	afInitFileFormat(m_af_setup, AF_FILE_WAVE);
	afInitByteOrder(m_af_setup, AF_DEFAULT_TRACK, AF_BYTEORDER_LITTLEENDIAN);
	afInitChannels(m_af_setup, AF_DEFAULT_TRACK, getInfo().num_channels);
	afInitRate(m_af_setup, AF_DEFAULT_TRACK, getInfo().sample_rate);
	afInitSampleFormat(m_af_setup, AF_DEFAULT_TRACK, AF_SAMPFMT_TWOSCOMP, 16);

	m_af_file = afOpenFile(m_file_name.c_str(), "w", m_af_setup);

	if (m_af_file == NULL) {
	    Logger::instance().log("wav_out", Log::ERROR, "Could not open file.");

	    afFreeFileSetup(m_af_setup);
	    return false;
	}

	m_buf = new short int[getInfo().block_size * getInfo().num_channels];
	setState(IDLE);
    }

    return true;
}

bool OutputWave::close()
{
    if (getState() != NOTINIT) {
	afCloseFile(m_af_file);
	afFreeFileSetup(m_af_setup);

	delete [] m_buf;
	return true;
    }

    return false;
}

bool OutputWave::put(const AudioBuffer& in_buf, size_t nframes)
{
    bool ret = false;
    
    if (getState() != NOTINIT) {
	size_t copyframes = getInfo().block_size;

	ret = true;
	while (nframes > 0) {
	    if (nframes < copyframes)
		copyframes = nframes;

	    in_buf.interleaveS16(m_buf, copyframes);
	    afWriteFrames(m_af_file, AF_DEFAULT_TRACK, m_buf, copyframes);

	    nframes -= copyframes;
	}
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
