/**
 *  Time-stamp:  <2011-03-08 17:49:52 raskolnikov>
 *
 *  @file        file_input.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 17:35:02 2011
 *
 *  Input file reader template implementation.
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

#ifndef PSYNTH_IO_FILE_INPUT_TPP_
#define PSYNTH_IO_FILE_INPUT_TPP_

#include <psynth/new_io/file_input.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

std::size_t
file_input_take_impl (SNDFILE* file, sound::bits8s* ptr, std::size_t frames);
std::size_t
file_input_take_impl (SNDFILE* file, sound::bits16s* ptr, std::size_t frames);
std::size_t
file_input_take_impl (SNDFILE* file, sound::bits32s* ptr, std::size_t frames);
std::size_t
file_input_take_impl (SNDFILE* file, sound::bits32sf* ptr, std::size_t frames);

} /* namespace detail */

template <class Range>
file_input<Range>::file_input (const char* fname)
{
    _file = file_open_impl (fname, SF_READ, &info);
}

template <class Range>
file_input<Range>::~file_input ()
{
    file_close_impl (_file);
}

/**
 * @todo Read note in file_input::put
 */
template <class Range>
std::size_t file_input<Range>::put (const range& data)
{
    return file_input_take_impl (_file,
                                 static_cast<sample_type<Range>::type*> (
                                     range.frames ()),
                                 data.size ());
}

template <class Range>
std::size_t file_input<Range>::seek (std::ptrdiff_t offset, seek_dir dir)
{
    return file_seek_impl (_file, offset, dir);
}

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_FILE_INPUT_TPP_ */

