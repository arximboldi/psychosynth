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

#include "common/Logger.h"
#include "output/OutputOss.h"

using namespace std;

namespace psynth
{

OutputOss::OutputOss() :
    m_buf(NULL),
    m_thread(*
	     this)
{
}

OutputOss::OutputOss(const AudioInfo& info, const std::string& device) : 
    Output(info),
    m_buf(NULL),
    m_device(device),
    m_thread(*this)
{
}

OutputOss::~OutputOss()
{
    if (getState() != NOTINIT)
	close();
}

void OutputOss::run()
{
    while(getState() == RUNNING) {
	process(getInfo().block_size);
    }
}

bool OutputOss::start()
{
    if (getState() == IDLE) {
	setState(RUNNING);
	m_thread.start();
	return true;
    } else {
	Logger::instance().log("oss", Log::ERROR,
			       "Output thread already started or OSS subsystem not initialized.");
	return false;
    }
}

bool OutputOss::stop()
{
    if (getState() == RUNNING) {
	setState(IDLE);
	m_thread.join();
	return true;
    } else {
	Logger::instance().log("oss", Log::ERROR,
			       "Output thread not running.");
	return false;
    }
}

bool OutputOss::open()
{
    if (getState() == NOTINIT) {
	if ((m_fd = ::open(m_device.c_str(), O_WRONLY, 0)) < 0) {
	    Logger::instance().log("oss", Log::ERROR, "Could not open OSS device.");
	    return false;
	}
		
	m_format = AFMT_S16_LE;
	m_stereo = getInfo().num_channels == 2 ? 1 : 0;

	
	//int tmp = (0x0004 << 16) | ((int) log2(getInfo().block_size));
	int tmp = (0x0004 << 16) | ((int) log2(4096));
	
	ioctl (m_fd, SNDCTL_DSP_SETFRAGMENT, &tmp);
	ioctl(m_fd, SNDCTL_DSP_SETFMT, &m_format);
	ioctl(m_fd, SNDCTL_DSP_STEREO, &m_stereo);
	ioctl(m_fd, SNDCTL_DSP_SPEED,  &getInfo().sample_rate);
		
	m_buf = new short int[getInfo().block_size * getInfo().num_channels * sizeof(short int)];
		
	setState(IDLE);
	return true;
    } else {
	Logger::instance().log("oss", Log::ERROR, "Device already initialized.");
	return false;
    }
}

bool OutputOss::put(const AudioBuffer& in_buf, size_t nframes)
{  
    bool ret = true;
	
    if (in_buf.getInfo().num_channels != getInfo().num_channels 
	|| in_buf.getInfo().sample_rate != getInfo().sample_rate) {
	/* TODO: Adapt the audio signal to fit our requeriments. */
	return false;
    }

    if (getState() != NOTINIT) {
	int copyframes = getInfo().block_size;

	while (nframes > 0) {
	    if ((int)nframes < copyframes)
		copyframes = nframes;
			
	    in_buf.interleaveS16(m_buf, copyframes);
			
	    write(m_fd, m_buf, copyframes * (getInfo().num_channels) * sizeof(short int));
	    nframes -= copyframes;
	}
		
    } else {
	Logger::instance().log("oss", Log::ERROR, "Cannot write to uninitialized device.");
	ret = false;
    }
	
    return ret;	
}

bool OutputOss::close()
{
    if (getState() != NOTINIT) {
	if (getState() == RUNNING)
	    stop();
	delete [] m_buf;
	m_buf = NULL;
	::close(m_fd);
	setState(NOTINIT);
	return true;
    } else {
	Logger::instance().log("oss", Log::ERROR, "Cannot close uninitialized device.");
	return false;
    }
}

} /* namespace psynth */
