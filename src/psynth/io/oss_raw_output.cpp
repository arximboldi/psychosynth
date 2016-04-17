/**
 *  Time-stamp:  <2012-02-01 23:09:50 raskolnikov>
 *
 *  @file        oss_raw_output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 11:32:04 2011
 *
 *  OSS raw output implementation.
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

#define PSYNTH_MODULE_NAME "psynth.io.oss"

#include <cassert>

#include <fcntl.h>
#include <sys/soundcard.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>

#include "base/logger.hpp"
#include "base/throw.hpp"
#include "base/scope_guard.hpp"
#include "oss_raw_output.hpp"

namespace psynth
{
namespace io
{

PSYNTH_DEFINE_ERROR (oss_error);
PSYNTH_DEFINE_ERROR_WHAT (oss_open_error, "Error while opening OSS device.");
PSYNTH_DEFINE_ERROR_WHAT (oss_param_error, "Invalid parameter for OSS device.");

oss_raw_output::oss_raw_output (const char* device,
                                int         format,
                                int         sample_size,
                                int         buffer_size,
                                bool        interleaved,
                                int         rate,
                                int         channels,
                                callback_type cb)
    : thread_async (cb, true)
    , _frame_size (sample_size * channels)
    , _buffer_size (buffer_size)
{

#define PSYNTH_OSS_CHECK(fun, except)                                   \
    do {                                                                \
        int err = fun;                                                  \
        if (err == -1) {                                                \
            PSYNTH_THROW (except) <<                                    \
                "Problem opening (" << device << ") " << ::strerror (errno); \
        }                                                               \
    } while (0)

    int stereo;
    switch (channels)
    {
    case 1:
        stereo = 0;
        break;
    case 2:
        stereo = 1;
        break;
    default:
        PSYNTH_LOG << base::log::warning
                   << "Only stereo and mono sound is supported.";
        throw oss_param_error ();
    };

    int fragment = (2 << 16) | ((int) std::log2 (buffer_size));

    if (!interleaved)
    {
        PSYNTH_LOG << base::log::warning
                   << "Planar audio data is not supported.";
        throw oss_param_error ();
    }

    _handle = ::open (device, O_WRONLY, 0);
    PSYNTH_OSS_CHECK (_handle, oss_open_error);

    auto grd_handle = base::make_guard ([&] { ::close (_handle); });

    PSYNTH_OSS_CHECK (::ioctl (_handle, SNDCTL_DSP_SETFRAGMENT, &fragment),
                      oss_param_error);
    PSYNTH_OSS_CHECK (::ioctl (_handle, SNDCTL_DSP_SETFMT,      &format),
                      oss_param_error);
    PSYNTH_OSS_CHECK (::ioctl (_handle, SNDCTL_DSP_STEREO,      &stereo),
                      oss_param_error);
    PSYNTH_OSS_CHECK (::ioctl (_handle, SNDCTL_DSP_SPEED,       &rate),
                      oss_param_error);

    grd_handle.dismiss ();
}

oss_raw_output::~oss_raw_output ()
{
    soft_stop ();
    ::close (_handle);
}

std::size_t oss_raw_output::put_i (const void* data, std::size_t frames)
{
    ::ssize_t bytes_or_err = ::write (_handle, data, _frame_size * frames);
    if (bytes_or_err == -1)
    {
        PSYNTH_LOG  << base::log::warning
                    << "Error while writing to OSS device: "
                    << strerror (errno);
        return 0;
    }
    return bytes_or_err / _frame_size; // TODO: Performance?
}

std::size_t oss_raw_output::put_n (const void* const* data, std::size_t frames)
{
    assert (false);
    return 0;
}

void oss_raw_output::iterate ()
{
    process (_buffer_size);
}

} /* namespace io */
} /* namespace psynth */
