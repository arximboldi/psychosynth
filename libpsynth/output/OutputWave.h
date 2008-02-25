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

#ifndef OUTPUTWAVE_H
#define OUTPUTWAVE_H

#include <string>

#include <audiofile.h>
#include <libpsynth/output/Output.h>

class OutputWave : public Output
{
    AFfilehandle m_af_file;
    AFfilesetup m_af_setup;

    short int* m_buf;
    
    std::string m_file_name;
    
public:
    OutputWave(const AudioInfo& info);
    OutputWave(const AudioInfo& info, const std::string& fname);

    ~OutputWave();

    void setFileName(const std::string& fname) {
	m_file_name = fname;
    }
    
    bool open();
    bool close();
    bool put(const AudioBuffer& buf, size_t nframes);
    bool start();
    bool stop();
};

#endif /* OUTPUTWAVE_H */
