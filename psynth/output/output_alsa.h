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

#ifndef PSYNTH_OUTPUTALSA_H
#define PSYNTH_OUTPUTALSA_H

#include <boost/thread/thread.hpp>

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <pthread.h>

#include <libpsynth/output/output.h>

namespace psynth
{

class output_alsa : public output
{
    snd_pcm_t *alsa_pcm;
    snd_pcm_hw_params_t *alsa_hwparams;
    snd_pcm_sw_params_t *alsa_swparams;
    snd_pcm_format_t alsa_format;
    short int* m_buf;
    std::string alsa_device;

    boost::thread alsa_thread;
    
public:
    output_alsa ();
    output_alsa (const audio_info& info, const std::string& device);
    ~output_alsa ();

    bool set_device (const std::string& device) {
	if (get_state () == NOTINIT) {
	    alsa_device = device;
	    return true;
	}
	
	return false;
    }

    const std::string& get_device () const {
	return alsa_device;
    }
	
    void run();
    bool open();
    bool close();
    bool put (const audio_buffer& buf, size_t nframes);
    bool start();
    bool stop();
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUTALSA_H */
