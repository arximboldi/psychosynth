/**
 *  Time-stamp:  <2012-02-02 00:37:07 raskolnikov>
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
#include <cstdio>

#include "base/logger.hpp"
#include "base/throw.hpp"
#include "base/scope_guard.hpp"
#include "alsa_raw_output.hpp"

#define PSYNTH_ALSA_REPORT_XRUN 0

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
                                  unsigned int      nperiods,
                                  snd_pcm_uframes_t period_size,
                                  snd_pcm_access_t  access,
                                  unsigned int      rate,
                                  unsigned int      channels,
                                  callback_type     cb)
    : thread_async (cb, true)
    , _buffer_size (nperiods * period_size)
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

    PSYNTH_ALSA_CHECK (snd_pcm_open (&_handle, device,
                                     SND_PCM_STREAM_PLAYBACK,
                                     SND_PCM_NONBLOCK),
                       alsa_open_error);
    auto grd_handle = base::make_guard ([&] { snd_pcm_close (_handle); });

    // Hardware paremeters
    
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

    int dir = 0;
    unsigned int actual_nperiods = nperiods;
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_periods_near (
                           _handle, _hw_params, &actual_nperiods, &dir),
                       alsa_param_error); 
    if (actual_nperiods != nperiods)
        PSYNTH_LOG << base::log::warning
                   << "ALSA could not set the requested periods. "
                   << "Actual periods is: " << actual_nperiods;

    dir = 0;
    snd_pcm_uframes_t actual_period_size = period_size;
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_period_size_near (
                           _handle, _hw_params, &actual_period_size, &dir),
                       alsa_param_error); 
    if (actual_period_size != period_size)
        PSYNTH_LOG << base::log::warning
                   << "ALSA could not set the requested buffer size. "
                   << "Actual period size is: " << actual_period_size;

    _buffer_size = nperiods * period_size;
    PSYNTH_ALSA_CHECK (snd_pcm_hw_params_set_buffer_size_near (
                           _handle, _hw_params, &_buffer_size),
                       alsa_param_error); 
    if (_buffer_size != nperiods * period_size)
        PSYNTH_LOG << base::log::warning
                   << "ALSA could not set the requested buffer size. "
                   << "Actual buffer size is: " << _buffer_size;

    PSYNTH_ALSA_CHECK (snd_pcm_hw_params (_handle, _hw_params),
                       alsa_param_error);
    
    // Software parameters
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_malloc (&_sw_params),
                       alsa_param_error);

    auto grd_sw_params = base::make_guard (
        [&] { snd_pcm_sw_params_free (_sw_params); });

    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_current (_handle, _sw_params),
                       alsa_param_error);
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_start_threshold (
                           _handle, _sw_params, 0),
                       alsa_param_error);

    bool soft_mode = true;
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_stop_threshold (
                           _handle, _sw_params,
                           soft_mode ? -1 : _buffer_size),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_silence_threshold (
                           _handle, _sw_params, 0),
                       alsa_param_error);

#if PSYNTH_ALSA_SET_SILENCE_SIZE
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_silence_size (
                           _handle, _sw_params, 0),
                       alsa_param_error);
#endif
    
    PSYNTH_ALSA_CHECK (snd_pcm_sw_params_set_avail_min (
                           _handle, _sw_params, actual_period_size),
                       alsa_param_error);

    PSYNTH_ALSA_CHECK (snd_pcm_sw_params (_handle, _sw_params),
                       alsa_param_error);
    
    grd_handle.dismiss ();
    grd_hw_params.dismiss ();
    grd_sw_params.dismiss ();

#undef PSYNTH_ALSA_CHECK
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

int alsa_xrun_recovery (snd_pcm_t* handle)
{
    snd_pcm_status_t *status;
    int res;
    
    snd_pcm_status_alloca(&status);
    
    if ((res = snd_pcm_status(handle, status)) < 0)
    {
        PSYNTH_LOG << base::log::error <<
            "Status error: " << snd_strerror(res);
    }
    
    if (snd_pcm_status_get_state(status) == SND_PCM_STATE_SUSPENDED)
    {
        PSYNTH_LOG << "ALSA output is in suspended state.";
        
        if ((res = snd_pcm_prepare (handle)) < 0)
            PSYNTH_LOG << base::log::error <<
                "Error preparing after suspend: " << snd_strerror(res);
    }
    
    if (snd_pcm_status_get_state(status) == SND_PCM_STATE_XRUN) {
#if PSYNTH_ALSA_REPORT_XRUN
        struct timeval now, diff, tstamp;
        snd_pcm_status_get_tstamp (status,&now);
        snd_pcm_status_get_trigger_tstamp (status, &tstamp);
        timersub(&now, &tstamp, &diff);
        float delayed_usecs = diff.tv_sec * 1000000.0 + diff.tv_usec;
        
        PSYNTH_LOG << base::log::error <<
            "Buffer underrun of at least " <<
            delayed_usecs / 1000.0 << " msecs.";
#endif
    }

    return res;
}

} /* namespace anonymous */

std::size_t alsa_raw_output::put_i (const void* data, std::size_t frames)
{
    snd_pcm_sframes_t res;

    res = snd_pcm_writei (_handle, data, frames);
    
    if (res < 0 && alsa_xrun_recovery (_handle) < 0)
    {
        PSYNTH_LOG << "Write error: " << snd_strerror (res);
        return 0;
    }
    
    return res;
}

std::size_t alsa_raw_output::put_n (const void* const* data, std::size_t frames)
{
    snd_pcm_sframes_t res;

    res = snd_pcm_writen (_handle, (void**) data, frames);

    if (res < 0 && alsa_xrun_recovery (_handle) < 0)
    {
        PSYNTH_LOG << "Write error: " << snd_strerror (res);
        return 0;
    }
    
    return res;
}

void alsa_raw_output::start ()
{
    thread_async::start ();
}

void alsa_raw_output::stop ()
{
    thread_async::stop ();
    snd_pcm_drop(_handle);
}   

void alsa_raw_output::prepare()
{
#define PSYNTH_ALSA_CHECK(fun, except)                                  \
    do {                                                                \
        int err = fun;                                                  \
        if (err < 0) {                                                  \
            PSYNTH_THROW (except) << "Problem starting playback. "      \
                                  << snd_strerror (err);                \
        }                                                               \
    } while (0)

    PSYNTH_ALSA_CHECK (snd_pcm_prepare (_handle), alsa_start_error);
    PSYNTH_ALSA_CHECK (snd_pcm_start (_handle), alsa_start_error);

#undef PSYNTH_ALSA_CHECK
}

void alsa_raw_output::iterate ()
{
    snd_pcm_sframes_t nframes_or_err = 0;

    nframes_or_err = snd_pcm_wait (_handle, 1000);
    if (nframes_or_err < 0)
        PSYNTH_LOG << "snd_pcm_wait () error: "
                   << snd_strerror (nframes_or_err);

    nframes_or_err = snd_pcm_avail (_handle);

    if (nframes_or_err < 0)
    {
        alsa_xrun_recovery(_handle);
    }
    else
    {
        process (std::min<std::size_t> (nframes_or_err, _buffer_size));
    }
}

} /* namespace io */
} /* namespace psynth */
