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

#ifndef PSYNTH_FILEREADERWAVE_H
#define PSYNTH_FILEREADERWAVE_H

#include <sndfile.h>
#include <libpsynth/input/FileReader.h>

namespace psynth
{

class FileReaderWave : public FileReader
{
    SNDFILE* m_file;
    
public:
    ~FileReaderWave() {
	if (isOpen())
	    close();
    }
    
    void open(const char* file);
    void seek(size_t pos);
    int read(audio_buffer& buf, int n_samples);
    void close();
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADERWAVE_H */
