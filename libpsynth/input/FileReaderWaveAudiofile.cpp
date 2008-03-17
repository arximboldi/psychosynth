/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007, 2008                    *
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

#include <audiofile.h>

#include "input/FileReaderWave.h"
#include "common/Logger.h"

using namespace std;

namespace psynth
{

void FileReaderWave::open(const char* file)
{
    if (!isOpen()) {
	m_file = afOpenFile(file, "r", NULL);
    
	if (m_file == AF_NULL_FILEHANDLE) {
	    Logger::instance().log("wave", Log::ERROR, string("Could not open file: ") + file);
	    return;
	}

	AudioInfo info;
    
	info.sample_rate = afGetRate(m_file, AF_DEFAULT_TRACK);
	info.num_channels = afGetVirtualChannels(m_file, AF_DEFAULT_TRACK);
	info.block_size = afGetFrameCount(m_file, AF_DEFAULT_TRACK);
	setInfo(info);
    
	afGetVirtualSampleFormat(m_file, AF_DEFAULT_TRACK,
				 &m_sample_format, &m_sample_width);

	m_buffer = operator new (getBufferSize() * (m_sample_width >> 3) * info.num_channels);

	cout << "channels: " << info.num_channels << endl;
	cout << "sample_rate: " << info.sample_rate << endl;
	cout << "sample_format: " << m_sample_format << endl;
	cout << "sample_width: " << m_sample_width << endl;
	
	setIsOpen(true);
    }
}

void FileReaderWave::seek(size_t pos)
{
    afSeekFrame (m_file, AF_DEFAULT_TRACK, pos);
}

int FileReaderWave::read(AudioBuffer& buf, int n_samples)
{
    int n_read;

    if (n_samples > getBufferSize())
	n_samples = getBufferSize();
    
    n_read = afReadFrames(m_file, AF_DEFAULT_TRACK,
			  m_buffer, n_samples);

    if (m_sample_format == AF_SAMPFMT_FLOAT) {
	buf.deinterleave((Sample*)m_buffer, n_read);
    } else if (m_sample_format == AF_SAMPFMT_TWOSCOMP ||
	       m_sample_format == AF_SAMPFMT_UNSIGNED) {
	if (m_sample_width == 8)
	    buf.deinterleaveC8((char*)m_buffer, n_read);
	else if (m_sample_width == 16)
	    buf.deinterleaveS16((short*)m_buffer, n_read);
	else if (m_sample_width == 32)
	    buf.deinterleaveI32((int*)m_buffer, n_read);
    }
    
    return n_read;
}

void FileReaderWave::close()
{
    if (isOpen()) {
	afCloseFile(m_file);
	operator delete (m_buffer);
	setIsOpen(false);
    }
}

} /* namespace psynth */
