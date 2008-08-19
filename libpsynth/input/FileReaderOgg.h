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

#ifndef PSYNTH_FILEREADEROGG
#define PSYNTH_FILEREADEROGG

#include <libpsynth/input/FileReader.h>
#include <vorbis/vorbisfile.h>

namespace psynth
{

class FileReaderOgg : public FileReader
{
    OggVorbis_File m_file;
    int m_bitstream;
    
public:
    FileReaderOgg() :
	m_bitstream(0) {}

    ~FileReaderOgg() {
	if (isOpen())
	    close();
    }
    
    void open(const char* file);
    void seek(size_t pos);
    int read(audio_buffer& buf, int n_samples);
    void close();
};
    
} /* namespace psynth */

#endif /* PSYNTH_FILEREADEROGG */
