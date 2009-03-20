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

#include <iostream>

#include <cmath>

#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <boost/bind.hpp>

#include "base/logger.hpp"
#include "io/output_oss.hpp"

using namespace std;

namespace psynth
{

output_oss::output_oss()
    : m_buf(NULL)
{
}

output_oss::output_oss(const audio_info& info, const std::string& device)
    : output(info)
    , m_buf(NULL)
    , m_device(device)
    , m_fragments (4)
    , m_fragmentsize (4096)
{
}

output_oss::~output_oss()
{
    if (get_state () != NOTINIT)
	close();
}

void output_oss::run()
{
    while(get_state () == RUNNING) {
	process(get_info ().block_size);
    }
}

bool output_oss::start()
{
    if (get_state () == IDLE) {
	set_state (RUNNING);
	m_thread = boost::thread (boost::bind (&output_oss::run, this));
	return true;
    } else {
	logger::self () ("oss", log::error,
			    "Output thread already started or OSS subsystem not initialized.");
	return false;
    }
}

bool output_oss::stop()
{
    if (get_state () == RUNNING) {
	set_state (IDLE);
	m_thread.join();
	return true;
    } else {
	logger::self () ("oss", log::error,
			    "Output thread not running.");
	return false;
    }
}

bool output_oss::open()
{
    if (get_state () == NOTINIT) {
	if ((m_fd = ::open(m_device.c_str(), O_WRONLY, 0)) < 0) {
	    logger::self () ("oss", log::error, "Could not open OSS device.");
	    return false;
	}
		
	m_format = AFMT_S16_LE;
	m_stereo = get_info ().num_channels == 2 ? 1 : 0;

	int tmp = (m_fragments << 16) | ((int) log2 (m_fragmentsize));
	
	ioctl (m_fd, SNDCTL_DSP_SETFRAGMENT, &tmp);
	ioctl(m_fd, SNDCTL_DSP_SETFMT, &m_format);
	ioctl(m_fd, SNDCTL_DSP_STEREO, &m_stereo);
	ioctl(m_fd, SNDCTL_DSP_SPEED,  &get_info ().sample_rate);
		
	m_buf = new short int [get_info ().block_size *
			       get_info ().num_channels *
			       sizeof(short int)];
		
	set_state (IDLE);
	return true;
    } else {
	logger::self () ("oss", log::error, "Device already initialized.");
	return false;
    }
}

bool output_oss::put(const audio_buffer& in_buf, size_t nframes)
{  
    bool ret = true;
	
    if (in_buf.get_info().num_channels != get_info ().num_channels ||
	in_buf.get_info().sample_rate != get_info ().sample_rate) {
	/* TODO: Adapt the audio signal to fit our requeriments. */
	return false;
    }

    if (get_state () != NOTINIT) {
	int copyframes = get_info ().block_size;

	while (nframes > 0) {
	    if ((int)nframes < copyframes)
		copyframes = nframes;
			
	    in_buf.interleave_s16(m_buf, copyframes);
			
	    write(m_fd, m_buf, copyframes * (get_info ().num_channels) * sizeof(short int));
	    nframes -= copyframes;
	}
		
    } else {
	logger::self () ("oss", log::error, "Cannot write to uninitialized device.");
	ret = false;
    }
	
    return ret;	
}

bool output_oss::close()
{
    if (get_state () != NOTINIT) {
	if (get_state () == RUNNING)
	    stop();
	delete [] m_buf;
	m_buf = NULL;
	::close(m_fd);
	set_state (NOTINIT);
	return true;
    } else {
	logger::self () ("oss", log::error, "Cannot close uninitialized device.");
	return false;
    }
}

} /* namespace psynth */
