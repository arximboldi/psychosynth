/**
 *  Time-stamp:  <2011-03-07 12:26:36 raskolnikov>
 *
 *  @file        alsa_raw_output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 19:06:57 2011
 *
 *  Alsa raw output implementation.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define PSYNTH_MODULE_NAME "psynth.io.alsa"

#include "base/scope_guard.hpp"
#include "alsa_raw_output.hpp"

namespace psynth
{
namespace io
{

PSYNTH_DEFINE_ERROR (alsa_error);
PSYNTH_DEFINE_ERROR_WHAT (alsa_open_error, "Can not open device.");
PSYNTH_DEFINE_ERROR_WHAT (alsa_param_error, "Invalid parameter.");

#define PSYNTH_ALSA_CHECK(fun, except)                          \
    do {                                                        \
        int err = fun;                                          \
        if (err < 0)                                            \
            PSYNTH_LOG << log::warning << snd_strerror (err);   \
    } while (0)

alsa_raw_output::alsa_raw_output (const char*      device,
                                  snd_pcm_format_t format,
                                  snd_uframes_t    buffer_size,
                                  snd_pcm_access_t access,
                                  unsigned int     rate,
                                  unsigned int     channels)
{

    PSYNTH_ALSA_CHECK (snd_pcm_open (&_handle), alsa_open_error);
    auto grd_handle = base::make_guard ([&] { snd_pcm_close (_handle); });
    
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_malloc (&_hw_params),
                       alsa_param_error);
    auto grd_hw_params = base::make_guard (
        [&] { snd_pcm_hw_params_free (&_hw_params); });

    
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_any (_handle, _hwparams),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_access (
                           _handle, _hwparams, access),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_format (
                           _handle, _hw_params, format),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_rate (
                           _handle, _hw_params, rate, 0),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_channels (
                           _handle, _hw_params, channels),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_buffer_size (
                           _hw_params, buffer_size),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params (_handle, _hw_params),
                       alsa_param_error);
		
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_malloc (&_sw_params),
                       alsa_param_error);

    auto grd_sw_params = base::make_guard (
        [&] { snd_pcm_hw_params_free (_sw_params); });
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_avail_min (
                           _handle, _sw_params, buffer_size),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_start_threshold (
                           _handle, _sw_params, buffer_size),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_stop_threshold (
                           _handle, _sw_params, -1),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params (_handle, _sw_params),
                       alsa_param_error);
    
    grd_handle.dismiss ();
    grd_hw_params.dismiss ();
    grd_sw_params.dismiss ();
}
    
alsa_raw_output::~alsa_raw_output ()
{
    soft_stop ();
    snd_pcm_close (_handle);
    snd_pcm_hw_params_free (_hw_params);
    snd_pcm_sw_params_free (_sw_params);
}

std::size_t alsa_raw_output::put_i (void*  data, std::size_t frames)
{
    snd_pcm_sframes_t res = snd_pcm_writei (_handle, data, frames);

    if (res < 0)
    {
        PSYNTH_LOG << "snd_pcm_writei () error: " << snd_strerror (res);
        return 0;
    }
    
    return res;
}

std::size_t alsa_raw_output::put_n (void** data, std::size_t frames)
{
    snd_pcm_sframes_t res = snd_pcm_writen (_handle, data, frames);

    if (res < 0)
    {
        PSYNTH_LOG << "snd_pcm_writen () error: " << snd_strerror (res);
        return 0;
    }
    
    return res;
}

void alsa_raw_output::iterate ()
{
    pcm_sframes_t nframes_or_err = 0;

    nframes_or_err = snd_pcm_wait (_handle, 1000);  
    if (nframes_or_err < 0)
        PSYNTH_LOG << "snd_pcm_wait () error: "
                   << snd_strerror (nframes_or_err);
    
    if ((nframes_or_err = snd_pcm_avail_update (alsa_pcm)) < 0)
    {
        PSYNTH_LOG << log::warning
                   << (nframes == -EPIPE ?
                       "Buffer underrun ocurred: " :
                       "snd_pcm_avail_update () error: ")
                   << snd_strerror (nframes_or_err);
    }
    else
        process (nframes_or_err);
}

} /* namespace io */
} /* namespace psynth */
