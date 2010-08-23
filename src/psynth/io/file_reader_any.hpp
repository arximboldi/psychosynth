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

#ifndef PSYNTH_FILEREADERANY
#define PSYNTH_FILEREADERANY

#include <psynth/io/file_reader.hpp>

namespace psynth
{

class file_reader_any : public file_reader
{
    file_reader* m_the_reader;

public:
    file_reader_any ()
	: m_the_reader(NULL) {}

    ~file_reader_any () {
	if (is_open ())
	    close ();
    }
    
    virtual void open (const std::string& file);

    virtual void seek (size_t pos) {
	m_the_reader->seek (pos);
    }
    
    virtual size_t read (audio_buffer& buf, size_t n_samples) {
	return m_the_reader->read(buf, n_samples);
    }
    
    virtual void close () {
	if (is_open ()) {
	    m_the_reader->close();
	    delete m_the_reader;
	    m_the_reader = NULL;
	    set_is_open (false);
	}
    }
};

} /* PSYNTH_FILEREADERANY */

#endif /* PSYNTH_FILEREADERANY */
