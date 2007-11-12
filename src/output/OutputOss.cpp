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

#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "psychosynth.h"
#include "output/OutputOss.h"

using namespace std;

OutputOss::OutputOss(const AudioInfo& info, const std::string& device) : 
	Output(info),
	buf(NULL),
	oss_device(device),
	oss_thread(this)
{
	out_state = OUT_NOTINIT;
}

OutputOss::~OutputOss()
{
	if (out_state != OUT_NOTINIT)
		close();
}

void OutputOss::run()
{
	while(out_state == OUT_RUNNING) {
		out_callback (out_info.block_size, out_cbdata);
	}
}

void OutputOss::start()
{
	if (out_state == OUT_IDLE) {
		out_state = OUT_RUNNING;
		oss_thread.start();
	} else {
		cout << _("ERROR: OSS output thread already started or OSS subsystem not initialized.") << endl;
	}
}

void OutputOss::stop()
{
	if (out_state == OUT_RUNNING) {
		out_state = OUT_IDLE;
		oss_thread.join();
	} else {
		cout << _("ERROR: Alsa output thread not running.") << endl;
	}
}

bool OutputOss::open()
{
	if (out_state == OUT_NOTINIT) {
		
		if ((oss_fd = ::open(oss_device.c_str(), O_WRONLY, 0)) < 0) {
			cerr << _("ERROR: Could not open OSS device: ") << oss_device << endl;
			return false;
		}
		
		oss_format = AFMT_S16_LE;
		oss_stereo = out_info.num_channels==2?1:0;
		
		ioctl(oss_fd, SNDCTL_DSP_SETFMT, &oss_format);
		ioctl(oss_fd, SNDCTL_DSP_STEREO, &oss_stereo);
		ioctl(oss_fd, SNDCTL_DSP_SPEED,  &out_info.sample_rate);
		
		buf = new short int[out_info.block_size * out_info.num_channels * sizeof(short int)];
		
		out_state = OUT_IDLE;
		return true;
	} else {
		cerr << _("WARNING: OSS output object already initialized.") << endl;
		return false;
	}
}

bool OutputOss::put(const Real* rbuf, int nframes)
{
	int i;
	Real r;
	short int* bufp;

	if (out_state != OUT_NOTINIT) {
		int copyframes = out_info.block_size;
		while (nframes > 0) {
			if (nframes < copyframes)
				copyframes = nframes;
			
			bufp = buf;
			for (i = 0; i < copyframes*out_info.num_channels; i++) {
				r = *(rbuf++);
				if (r < -1) r = -1; 
				else if (r > 1) r = 1;	
				*(bufp++) = (short int)(r*32767.0);
			}
			write(oss_fd, buf, copyframes * (out_info.num_channels) * sizeof(short int));
			nframes -= copyframes;
		}

	} else {
		cerr << _("ERROR: OSS output device not initialized. Cannot write.") << endl;
		return false;
	}
	
	return true;
}

bool OutputOss::put(const AudioBuffer& in_buf)
{
	int i, j;
	int nframes = in_buf.getInfo().block_size;
	short int* bufp;
	bool ret = true;
	Real r;
	
	if (in_buf.getInfo().num_channels != out_info.num_channels 
		|| in_buf.getInfo().sample_rate != out_info.sample_rate) {
		/* TODO: Adapt the audio signal to fit our requeriments. */
		WARNING("Cant send data to the device: data and output system properties missmatch.");
		return false;
	}

	if (out_state != OUT_NOTINIT) {
		int copyframes = out_info.block_size;
		while (nframes > 0) {
			if (nframes < copyframes)
				copyframes = nframes;
			
			bufp = buf;
			for (i = 0; i < copyframes; i++) {
				for (j = 0; j < out_info.num_channels; j++) {
					r = in_buf[j][i]; /* TODO: Optimize interleaving */
					if (r < -1) r = -1; 
					else if (r > 1) r = 1;	
					*(bufp++) = (short int)(r*32766.0);
				}
			}
			
			write(oss_fd, buf, copyframes * (out_info.num_channels) * sizeof(short int));
			nframes -= copyframes;
		}
		
	} else {
		cerr << _("ERROR: OSS output device not initialized. Cannot write.") << endl;
		ret = false;
	}
	
	return ret;	
}

bool OutputOss::close()
{
	if (out_state != OUT_NOTINIT) {
		if (out_state == OUT_RUNNING) stop();
		delete [] buf;
		::close(oss_fd);
		return true;
	} else {
		cerr << _("ERROR: OSS output device not initialized. Cannot end") << endl;
		return false;
	}
}
