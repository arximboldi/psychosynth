/**
 *  Time-stamp:  <2011-03-22 20:26:26 raskolnikov>
 *
 *  @file        file_common.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 16:50:40 2011
 *
 *  File common definitions.
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

#define PSYNTH_MODULE_NAME "psynth.io.file"

#include "base/logger.hpp"
#include "base/scope_guard.hpp"
#include "file_common.hpp"

namespace psynth
{
namespace io
{

PSYNTH_DEFINE_ERROR (file_error);
PSYNTH_DEFINE_ERROR_WHAT (file_open_error, "Error while opening file.");
PSYNTH_DEFINE_ERROR_WHAT (file_param_error, "Invalid filefile parameter.");
PSYNTH_DEFINE_ERROR_WHAT (file_seek_error, "Error while seeking file.");

namespace
{

int sndfile_format [] =
{
    SF_FORMAT_WAV,
    SF_FORMAT_AIFF,
    SF_FORMAT_AU,
    SF_FORMAT_FLAC,
    SF_FORMAT_OGG
};

int sndfile_seek_dir [] =
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

} /* anonymous namespace */


namespace detail
{

int file_format_impl (file_fmt format, int sample_format)
{
    return sndfile_format [static_cast<int> (format)] |
        sample_format | SF_ENDIAN_FILE;
}

SNDFILE* file_open_impl (const char* fname, int mode, SF_INFO* info)
{
    int orig_channels = info->channels;
    SNDFILE* file = sf_open (fname, mode, info);
    auto file_grd = base::make_guard ([&] { sf_close (file); });

    if (file == 0)
    {
        PSYNTH_LOG << base::log::warning
                   << "Problem while opening audio file: "
                   << sf_strerror (file);
        throw file_open_error ();
    }

    if (orig_channels != info->channels)
        throw file_open_error ("Number of channels expected mismatch.");

    file_grd.dismiss ();
    return file;
}

std::size_t file_seek_impl (SNDFILE* file, std::ptrdiff_t offset, seek_dir dir)
{
    sf_count_t res = sf_seek (file, offset,
                              sndfile_seek_dir [static_cast<int> (dir)]);

    if (res < 0)
    {
        PSYNTH_LOG << base::log::warning
                   << "Error while seeking audio file: "
                   << sf_strerror (file);
        throw file_seek_error ();
    }

    return res;
}

void file_close_impl (SNDFILE* file)
{
    sf_close (file);
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */
