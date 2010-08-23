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
#include <psynth/io/file_reader.hpp>

namespace psynth
{

class file_reader_wave : public file_reader
{
    SNDFILE* m_file;
    
public:
    ~file_reader_wave() {
	if (is_open())
	    close();
    }
    
    void open (const std::string& file);
    void seek (size_t pos);
    size_t read (audio_buffer& buf, size_t n_samples);
    void close ();
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADERWAVE_H */
