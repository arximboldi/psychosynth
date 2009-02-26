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

#ifndef PSYNTH_OUTPUTWAVE_H
#define PSYNTH_OUTPUTWAVE_H

#include <string>

#include <sndfile.h>
#include <psynth/output/output.hpp>

namespace psynth
{

class output_wave : public output
{
    SNDFILE* m_file;
    
    std::string m_file_name;
    
public:
    output_wave(const audio_info& info);
    output_wave(const audio_info& info, const std::string& fname);

    ~output_wave();

    void set_file_name (const std::string& fname) {
	m_file_name = fname;
    }
    
    bool open();
    bool close();
    bool put(const audio_buffer& buf, size_t nframes);
    bool start();
    bool stop();
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUTWAVE_H */
