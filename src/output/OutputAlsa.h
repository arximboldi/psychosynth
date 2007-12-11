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

#ifndef __OUTPUTALSA_H__
#define __OUTPUTALSA_H__

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>
#include <pthread.h>

#include "psychosynth.h"
#include "output/Output.h"
#include "common/Thread.h"

class OutputAlsa : public Output, Runnable {
    snd_pcm_t *alsa_pcm;
    snd_pcm_hw_params_t *alsa_hwparams;
    snd_pcm_sw_params_t *alsa_swparams;
    snd_pcm_format_t alsa_format;
    short int* m_buf;
    std::string alsa_device;
    Thread alsa_thread;
public:
    OutputAlsa(const AudioInfo& info, const std::string& device);
    ~OutputAlsa();
	
    void run();
    bool open();
    bool close();
    bool put(const AudioBuffer& buf, size_t nframes);
    void start();
    void stop();
};


#endif
