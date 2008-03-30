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

#include "common/Logger.h"
#include "input/FileReaderOgg.h"

using namespace std;

namespace psynth
{

void FileReaderOgg::open(const char* file)
{
    if (!isOpen()) {
	/* const_cast cause vorbis code sucks a little bit... */
	if (ov_fopen(const_cast<char*>(file), &m_file)) {
	    Logger::instance().log("ogg", Log::ERROR, string("Could not open file: ") + file);
	    return;
	}

	AudioInfo info;
	vorbis_info* oi;
	oi = ov_info(&m_file, -1);
	info.num_channels = oi->channels;
	info.sample_rate = oi->rate;
	info.block_size = ov_pcm_total(&m_file, -1);
	setInfo(info);

	setIsOpen(true);
    }
}

void FileReaderOgg::seek(size_t pos)
{
    if (ov_seekable(&m_file))
	ov_pcm_seek(&m_file, pos); //*getInfo().num_channels);
    else
	Logger::instance().log("ogg", Log::ERROR, "Stream not seekable.");
}

int FileReaderOgg::read(AudioBuffer& buf, int n_samples)
{
    float** pcm;
    int bitstream;
    int n_read;
    int index = 0;
	
    while(n_samples > 0) {
	n_read = ov_read_float(&m_file, &pcm, n_samples, &bitstream);
	
	if (n_read) {
	    int n_chan = min(getInfo().num_channels,
			     buf.getInfo().num_channels);
	
	    int i;
	
	    for (i = 0; i < n_chan; ++i)
		memcpy(buf.getData()[i] + index, pcm[i], sizeof(float) * n_read);
	    for (; i < buf.getInfo().num_channels; ++i)
		memcpy(buf.getData()[i] + index, pcm[i-n_chan], sizeof(float) * n_read);

	    index += n_read;
	    n_samples -= n_read;
	} else {
	    n_samples = 0;
	}
    }
    
    return index;
}

void FileReaderOgg::close()
{
    if (isOpen()) {
	ov_clear(&m_file);
	setIsOpen(false);
    }
}

} /* namespace psynth */
