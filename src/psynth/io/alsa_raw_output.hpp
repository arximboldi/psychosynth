/**
 *  Time-stamp:  <2011-03-18 19:59:13 raskolnikov>
 *
 *  @file        alsa_raw_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 18:05:31 2011
 *
 *  Alsa raw output interface.
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

#ifndef PSYNTH_IO_ALSA_RAW_OUTPUT_H_
#define PSYNTH_IO_ALSA_RAW_OUTPUT_H_

#define ALSA_PCM_NEW_HW_PARAMS_API
#include <alsa/asoundlib.h>

#include <psynth/io/thread_async.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_ERROR (error, alsa_error);
PSYNTH_DECLARE_ERROR (alsa_error, alsa_open_error);
PSYNTH_DECLARE_ERROR (alsa_error, alsa_param_error);

class alsa_raw_output : public thread_async,
                        public boost::noncopyable
{
public:
    typedef thread_async::callback_type callback_type;
    
    alsa_raw_output (const char*       device,
                     snd_pcm_format_t  format,
                     snd_pcm_uframes_t buffer_size,
                     snd_pcm_access_t  access,
                     unsigned int      rate,
                     unsigned int      channels,
                     callback_type     cb = callback_type ());
    
    ~alsa_raw_output ();

    std::size_t put_i (const void*        data, std::size_t frames);
    std::size_t put_n (const void* const* data, std::size_t frames);

    std::size_t buffer_size () const
    { return _buffer_size; }
    
protected:
    void iterate ();
    
private:
    snd_pcm_uframes_t    _buffer_size;
    snd_pcm_t*           _handle;
    snd_pcm_hw_params_t* _hw_params;
    snd_pcm_sw_params_t* _sw_params;
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_ALSA_RAW_OUTPUT_H_ */

