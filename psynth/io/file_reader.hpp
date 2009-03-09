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

#ifndef PSYNTH_FILEREADER_H
#define PSYNTH_FILEREADER_H

#include <string>

#include <psynth/synth/audio_info.hpp>
#include <psynth/synth/audio_buffer.hpp>

namespace psynth
{

class file_reader
{
    bool m_isopen;
    audio_info m_info;
    
protected:
    void set_is_open (bool isopen) {
	m_isopen = isopen;
    };

    void set_info(const audio_info& info) {
	m_info = info;
    }
    
public:
    file_reader()
	: m_isopen(false)
	, m_info(0,0,0)
	{}
    
    virtual ~file_reader() {};
    
    bool is_open () {
	return m_isopen;
    }

    const audio_info& get_info () {
	return m_info;
    }
    
    virtual void open (const std::string& file) = 0;
    virtual void seek (size_t pos) = 0;
    virtual int read (audio_buffer& buf, int n_samples) = 0;
    virtual void close () = 0;
    
    int read (audio_buffer& buf) {
	return read(buf, buf.size());
    }
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADER_H */
