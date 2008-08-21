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
#include <pthread.h>

#include "output/output_alsa.h"
#include "common/logger.h"

using namespace std;

namespace psynth
{

output_alsa::output_alsa()
    : alsa_thread(*this)
{
}

output_alsa::output_alsa(const audio_info& info, const std::string& device)
    : output(info)
    , alsa_device(device)
    , alsa_thread(*this)
{
}

output_alsa::~output_alsa()
{
    if (get_state () != NOTINIT)
	close();
}

void output_alsa::run()
{
    snd_pcm_sframes_t nframes;
	
    while (get_state () == RUNNING) {
	// cout << "out_state " << out_state << '\n';
	snd_pcm_wait (alsa_pcm, 1000);  

	if ((nframes = snd_pcm_avail_update (alsa_pcm)) < 0) {
	    if (nframes == -EPIPE)
		logger::instance() ("alsa", log::WARNING, "Buffer underrun ocurred.");
	    else
		logger::instance() ("alsa", log::WARNING, "Unknown snd_pcm_avail_update() return value.");
	} else {
	    // cout << "processing " << nframes << endl; 
	    process(get_info ().block_size); //get_info ().block_size);
	}
    }
}

bool output_alsa::start()
{
    if (get_state () == IDLE) {
	set_state (RUNNING);
	alsa_thread.start();
	return true;
    } else {
	logger::instance() ("alsa", log::WARNING, "Thread already started or subsystem not initialized.");
	return false;
    }
}

bool output_alsa::stop()
{
    if (get_state () == RUNNING) {
	set_state (IDLE);
	alsa_thread.join();
	return true;
    } else {
	logger::instance() ("alsa", log::WARNING, "Thread not running.");
	return false;
    }
}

bool output_alsa::open()
{
    int err;
    int dir = 0;
    unsigned int uirate = get_info ().sample_rate;
	
    if (get_state () == NOTINIT) {
	cout << "sample_rate " << get_info ().sample_rate << endl;
	cout << "block_size " << get_info ().block_size << endl;
	cout << "num_channels " << get_info ().num_channels << endl;
	
	if ((err = snd_pcm_open (&alsa_pcm, alsa_device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
	    logger::instance() ("alsa", log::WARNING,
				string("Could not open device. (")
				+ snd_strerror(err) + ")");
	    return false;
	}
	
	snd_pcm_hw_params_malloc (&alsa_hwparams);
	snd_pcm_hw_params_any (alsa_pcm, alsa_hwparams);

	snd_pcm_hw_params_set_access (alsa_pcm, alsa_hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
		
	alsa_format = SND_PCM_FORMAT_S16;

	snd_pcm_hw_params_set_format (alsa_pcm, alsa_hwparams, alsa_format);
	snd_pcm_hw_params_set_rate_near (alsa_pcm, alsa_hwparams, &uirate, &dir);
	snd_pcm_hw_params_set_channels (alsa_pcm, alsa_hwparams, get_info ().num_channels);

	/* FIXME */
	snd_pcm_hw_params_set_periods(alsa_pcm, alsa_hwparams, 2, 0);
	snd_pcm_hw_params_set_buffer_size (alsa_pcm, alsa_hwparams, get_info ().block_size);

	snd_pcm_hw_params (alsa_pcm, alsa_hwparams);
	//snd_pcm_prepare (alsa_pcm);
		
	snd_pcm_sw_params_malloc (&alsa_swparams);
	snd_pcm_sw_params_set_avail_min(alsa_pcm, alsa_swparams, get_info ().block_size);
	snd_pcm_sw_params_set_start_threshold (alsa_pcm, alsa_swparams, 0U);
	snd_pcm_sw_params (alsa_pcm, alsa_swparams);

	if ((err = snd_pcm_prepare (alsa_pcm)) < 0) {
	    logger::instance() ("alsa", log::WARNING,
				string("Could not prepare device. (")
				+ snd_strerror(err) + ")");	    
	} else {
	    m_buf = new short int[get_info ().block_size * get_info ().num_channels * sizeof(short int)];
	    
	    set_state (IDLE);
	}
	
	return true;
    } else {
	logger::instance() ("alsa", log::WARNING, "Can not initialize twice.");
	return false;
    }
}

bool output_alsa::put(const audio_buffer& in_buf, size_t nframes)
{
    int err;
    bool ret = true;

    //cout << "writting: " << nframes << endl;
    if (in_buf.get_info().num_channels != get_info ().num_channels ||
	in_buf.get_info().sample_rate != get_info ().sample_rate) {
	/* TODO: Adapt the audio signal to fit our requeriments. */
	logger::instance() ("alsa", log::WARNING,
			    "Cant send data to the device: data and output system properties missmatch.");
	return false;
    }

    if (get_state () != NOTINIT) {
	size_t copyframes = get_info ().block_size;
		
	while (nframes > 0) {
	    if (nframes < copyframes)
		copyframes = nframes;

	    in_buf.interleave_s16(m_buf, copyframes);

	    if ((err = snd_pcm_writei (alsa_pcm, m_buf, copyframes)) != (int)copyframes) {
		logger::instance() ("alsa", log::WARNING,
				    string("Could not write to device. (")
				    + snd_strerror(err) + ")");
		//close(); /* WTF! */
		snd_pcm_recover(alsa_pcm, err, 1);
		snd_pcm_prepare(alsa_pcm);
		ret = false;
	    }
	    
	    nframes -= copyframes;
	}
		
    } else {
	logger::instance() ("alsa", log::WARNING, "Cannot write to an unitialized device");
	ret = false;
    }
	
    return ret;
}

bool output_alsa::close()
{
    if (get_state () != NOTINIT) {
	if (get_state () == RUNNING)
	    stop();
	
	snd_pcm_close(alsa_pcm);
	snd_pcm_hw_params_free(alsa_hwparams);
	snd_pcm_sw_params_free(alsa_swparams);
	set_state (NOTINIT);

	delete [] m_buf;
	return true;
    } else {
	logger::instance() ("alsa", log::WARNING, "Cannot close a device which is not opened.");
	return false;
    }
}

} /* namespace psynth */
