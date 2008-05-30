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

#include <libpsynth/common/AudioInfo.h>
#include <libpsynth/common/AudioBuffer.h>

namespace psynth
{

class FileReader
{
    bool m_isopen;
    AudioInfo m_info;
    
protected:
    void setIsOpen(bool isopen) {
	m_isopen = isopen;
    };

    void setInfo(const AudioInfo& info) {
	m_info = info;
    }
    
public:
    FileReader() :
	m_isopen(false),
	m_info(0,0,0) {}
    
    virtual ~FileReader() {};
    
    bool isOpen() {
	return m_isopen;
    }

    const AudioInfo& getInfo() {
	return m_info;
    }
    
    virtual void open(const char* file) = 0;
    virtual void seek(size_t pos) = 0;
    virtual int read(AudioBuffer& buf, int n_samples) = 0;
    virtual void close() = 0;
    
    int read(AudioBuffer& buf) {
	return read(buf, buf.size());
    }
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADER_H */
