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

#include "input/FileReaderWave.h"
#include "common/Logger.h"

using namespace std;

namespace psynth
{

void FileReaderWave::open(const char* file)
{
    if (!isOpen()) {
	SF_INFO sfinfo;
	
	m_file = sf_open(file, SFM_READ, &sfinfo);
    
	if (m_file == NULL) {
	    Logger::instance().log("wave", Log::ERROR, string("Could not open file: ") + file);
	    return;
	}

	AudioInfo info;
	info.sample_rate  = sfinfo.samplerate;
	info.num_channels = sfinfo.channels;
	info.block_size   = sfinfo.frames;
	setInfo(info);
    
	cout << "channels: " << info.num_channels << endl;
	cout << "sample_rate: " << info.sample_rate << endl;
		
	setIsOpen(true);
    }
}

void FileReaderWave::seek(size_t pos)
{
    sf_seek(m_file, pos, SEEK_SET);
}

int FileReaderWave::read(AudioBuffer& outbuf, int n_samples)
{
    int n_read;

    float buf [n_samples * getInfo().num_channels];
    
    n_read = sf_readf_float(m_file, buf, n_samples);

    if (n_read)
	outbuf.deinterleave(buf, n_read, getInfo().num_channels);
    
    return n_read;
}

void FileReaderWave::close()
{
    if (isOpen()) {
	sf_close(m_file);
	setIsOpen(false);
    }
}

} /* namespace psynth */
