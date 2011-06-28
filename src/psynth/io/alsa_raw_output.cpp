/**
 *  Time-stamp:  <2011-06-28 15:39:08 raskolnikov>
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

#include <limits>
#include <algorithm>

#include "base/logger.hpp"
#include "base/throw.hpp"
#include "base/scope_guard.hpp"
#include "alsa_raw_output.hpp"

namespace psynth
{
namespace io
{

PSYNTH_DEFINE_ERROR (alsa_error);
PSYNTH_DEFINE_ERROR_WHAT (alsa_open_error, "Can not open device.");
PSYNTH_DEFINE_ERROR_WHAT (alsa_start_error, "Can not start device.");
PSYNTH_DEFINE_ERROR_WHAT (alsa_param_error, "Invalid parameter.");

alsa_raw_output::alsa_raw_output (const char*       device,
                                  snd_pcm_format_t  format,
                                  snd_pcm_uframes_t buffer_size,
                                  snd_pcm_access_t  access,
                                  unsigned int      rate,
                                  unsigned int      channels,
                                  callback_type     cb)
    : thread_async (cb, true)
    , _buffer_size (buffer_size)
{
    
#define PSYNTH_ALSA_CHECK(fun, except)                                  \
    do {                                                                \
        int err = fun;                                                  \
        if (err < 0) {                                                  \
            PSYNTH_THROW (except) << "Problem opening ("                \
                                  << device << "). "                    \
                                  << snd_strerror (err);                \
        }                                                               \
    } while (0)
    
    int dir;
    
    PSYNTH_ALSA_CHECK (snd_pcm_open (&_handle, device,
                                     SND_PCM_STREAM_PLAYBACK, 0),
                       alsa_open_error);
    auto grd_handle = base::make_guard ([&] { snd_pcm_close (_handle); });

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_malloc (&_hw_params),
                       alsa_param_error);
    auto grd_hw_params = base::make_guard (
        [&] { snd_pcm_hw_params_free (_hw_params); });

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_any (_handle, _hw_params),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_access (
                           _handle, _hw_params, access),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_format (
                           _handle, _hw_params, format),
                       alsa_param_error);

    unsigned int actual_rate = rate;
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_rate_near (
                           _handle, _hw_params, &actual_rate, 0),
                       alsa_param_error);
    if (actual_rate != rate)
        PSYNTH_LOG << base::log::warning
                   << "ALSA does not like the selected frame rate and instead "
                   << "it chose: " << actual_rate;

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_channels (
                           _handle, _hw_params, channels),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_buffer_size_min (
                           _handle, _hw_params, &_buffer_size),
                       alsa_param_error); 
    if (_buffer_size != buffer_size)
        PSYNTH_LOG << base::log::warning
                   << "ALSA could not set the requested buffer size. "
                   << "Actual buffer size is: " << _buffer_size;
    
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params (_handle, _hw_params),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_malloc (&_sw_params),
                       alsa_param_error);

    auto grd_sw_params = base::make_guard (
        [&] { snd_pcm_sw_params_free (_sw_params); });

    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_current (_handle, _sw_params),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_avail_min (
                           _handle, _sw_params, buffer_size),
                       alsa_param_error);

    snd_pcm_uframes_t period_size;
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_get_period_size (
                           _hw_params, &period_size, &dir),
                       alsa_param_error);
        
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_start_threshold (
                           _handle, _sw_params,
                           (_buffer_size / period_size) * period_size),
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

namespace
{

static int alsa_xrun_recovery (snd_pcm_t *handle, int err)
{
    if (err == -EPIPE)
    {
        /* under-run */
        err = snd_pcm_prepare (handle);
        if (err < 0)
            PSYNTH_LOG << base::log::error
                       << "Can't recovery from underrun, prepare failed: "
                       << snd_strerror (err);
        return 0;
    }
    else if (err == -ESTRPIPE)
    {
        while ((err = snd_pcm_resume(handle)) == -EAGAIN)
            sleep (1); /* wait until the suspend flag is released */

        if (err < 0)
        {
            err = snd_pcm_prepare (handle);
            if (err < 0)
            PSYNTH_LOG << base::log::error
                       << "Can't recovery from suspend, prepare failed: "
                       << snd_strerror (err);         
        }
        return 0;
    }
    
    return err;
}

} /* namespace anonymous */

std::size_t alsa_raw_output::put_i (const void* data, std::size_t frames)
{
    snd_pcm_sframes_t res;
    
    while ((res = snd_pcm_writei (_handle, data, frames)) == -EAGAIN);
    
    if (res < 0 && alsa_xrun_recovery (_handle, res) < 0)
    {
        PSYNTH_LOG << "Write error: " << snd_strerror (res);
        return 0;
    }
    
    return res;
}

std::size_t alsa_raw_output::put_n (const void* const* data, std::size_t frames)
{
    
    snd_pcm_sframes_t res;
    
    while ((res = snd_pcm_writen (_handle, (void**) data, frames)) == -EAGAIN);

    if (res < 0 && alsa_xrun_recovery (_handle, res) < 0)
    {
        PSYNTH_LOG << "Write error: " << snd_strerror (res);
        return 0;
    }
    
    return res;
}

void alsa_raw_output::start ()
{
    //check_idle ();
    //PSYNTH_ALSA_CHECK (snd_pcm_prepare (_handle), alsa_start_error);
    //PSYNTH_ALSA_CHECK (snd_pcm_start (_handle), alsa_start_error);
    thread_async::start ();
}

void alsa_raw_output::stop ()
{
    thread_async::stop ();
}   

void alsa_raw_output::iterate ()
{
    // snd_pcm_sframes_t nframes_or_err = 0;

    // nframes_or_err = snd_pcm_wait (_handle, 1000);  
    // if (nframes_or_err < 0)
    //     PSYNTH_LOG << "snd_pcm_wait () error: "
    //                << snd_strerror (nframes_or_err);
    
    // if ((nframes_or_err = snd_pcm_avail_update (_handle)) < 0)
    // {
    //     PSYNTH_LOG << base::log::warning
    //                << (nframes_or_err == -EPIPE ?
    //                    "Buffer underrun ocurred: " :
    //                    "snd_pcm_avail_update () error: ")
    //                << snd_strerror (nframes_or_err);
    // }
    // else
    // {
    //     process (std::min<std::size_t> (nframes_or_err, _buffer_size));
    // }

    process (_buffer_size);
}

} /* namespace io */
} /* namespace psynth */
