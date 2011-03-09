/**
 *  Time-stamp:  <2011-03-09 01:50:43 raskolnikov>
 *
 *  @file        wave_raw_output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 21:47:43 2011
 *
 *  Raw output onto a wave file implementation.
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

#include "file_output.hpp"

namespace psynth
{
namespace io
{

namespace detail
{

/**
 * @todo Not supported by libsndfile.
 */
std::size_t
file_output_put_impl (SNDFILE* file, sound::bits8s* ptr, std::size_t frames)
{
    return 0;
}

std::size_t
file_output_put_impl (SNDFILE* file, sound::bits16s* ptr, std::size_t frames)
{
    // TODO: Error checking?
    return sf_writef_short (file, ptr, frames);
}

std::size_t
file_output_put_impl (SNDFILE* file, sound::bits32s* ptr, std::size_t frames)
{
    return sf_writef_int (file, ptr, frames);
}

std::size_t
file_output_put_impl (SNDFILE* file, sound::bits32sf* ptr, std::size_t frames)
{
    return sf_writef_float (file, reinterpret_cast<float*>(ptr), frames);
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */
