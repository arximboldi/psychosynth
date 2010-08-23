/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef PSYNTH_OUTPUTOSS_H
#define PSYNTH_OUTPUTOSS_H

#include <boost/thread/thread.hpp>

#include <psynth/io/output.hpp>

namespace psynth
{

class output_oss : public output
{
    int m_fd;
    int m_format;
    int m_stereo;
  int m_fragments;
  int m_fragmentsize;
    short int* m_buf;
    std::string m_device;
	
    boost::thread m_thread;
    
public:
    output_oss ();
    output_oss (const audio_info& info, const std::string& device);
    ~output_oss ();

  bool set_buffer_size (int fragmentsize, int fragments = -1) {
	if (get_state () == NOTINIT) {
	  if (m_fragments > 0)
	    m_fragments = fragments;
	  m_fragmentsize = fragmentsize;
	    return true;
	}
	
	return false;
  }
  
    bool set_device (const std::string& device) {
	if (get_state () == NOTINIT) {
	    m_device = device;
	    return true;
	}
	
	return false;
    }

    const std::string& get_device() const {
	return m_device;
    }
    
    bool open ();
    bool close ();
    bool put (const audio_buffer& buf, size_t nframes);
    void run ();
    bool start ();
    bool stop ();
};

} /* namespace psynth */

#endif /* OUTPUTOSS_H */
