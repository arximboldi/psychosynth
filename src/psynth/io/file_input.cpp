/**
 *  Time-stamp:  <2011-03-22 18:17:42 raskolnikov>
 *
 *  @file        file_input.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 17:50:47 2011
 *
 *  Implementation of file input facilities.
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

#include "file_input.hpp"

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
file_input_take_impl (SNDFILE* file, sound::bits8s* ptr, std::size_t frames)
{
    assert (false);
    return 0;
}

std::size_t
file_input_take_impl (SNDFILE* file, sound::bits16s* ptr, std::size_t frames)
{
    sf_count_t res = sf_readf_short (file, ptr, frames);
    return res;
}

std::size_t
file_input_take_impl (SNDFILE* file, sound::bits32s* ptr, std::size_t frames)
{
    sf_count_t res = sf_readf_int (file, ptr, frames);
    return res;
}

std::size_t
file_input_take_impl (SNDFILE* file, sound::bits32sf* ptr, std::size_t frames)
{
    sf_count_t res = sf_readf_float (file, reinterpret_cast<float*>(ptr), frames);
    return res;
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */
