/**
 *  Time-stamp:  <2011-03-16 23:47:37 raskolnikov>
 *
 *  @file        file_output.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 22:04:03 2011
 *
 *  File output template implementation.
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

#ifndef PSYNTH_IO_FILE_OUTPUT_TPP_
#define PSYNTH_IO_FILE_OUTPUT_TPP_

#include <psynth/io/file_output.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

std::size_t
file_output_put_impl (SNDFILE* file, const sound::bits8s* ptr, std::size_t frames);
std::size_t
file_output_put_impl (SNDFILE* file, const sound::bits16s* ptr, std::size_t frames);
std::size_t
file_output_put_impl (SNDFILE* file, const sound::bits32s* ptr, std::size_t frames);
std::size_t
file_output_put_impl (SNDFILE* file, const sound::bits32sf* ptr, std::size_t frames);

} /* namespace detail */

template <class Range>
file_output<Range>::file_output (const std::string& fname,
                                 file_fmt           format,
                                 std::size_t        rate)
{
    _info.channels   = sound::num_samples<Range>::value;
    _info.samplerate = rate;
    _info.format     = detail::file_format_impl (
        format, file_support<Range>::format::value);
    _file = detail::file_open_impl (fname.c_str (), SFM_WRITE, &_info);
}

template <class Range>
file_output<Range>::~file_output ()
{
    detail::file_close_impl (_file);
}

/**
 * @todo
 *
 * The implementation of this function relies in a bug in the
 * sndfile library. The sndfile library gets the frames to output in
 * int*, short* or float* variables. That is an issue for us as the
 * sizeof and representation of int, short and float are platform
 * dependant. However, we checked the sndfile library and it assumes
 * that sizeof (int) = 4, sizeof (short) = 2 and sizeof (char) = 1,
 * etc. Actually, it converts from int to char by doing:
 *
 *                <code>intvar >> 24</code>
 *
 * Yes. What the fuck. That is what I thought. But untill they fix
 * that we can exploit this bug to simplify our implementation an
 * assume that it is safe to cast bits32s* to int* when passing it to
 * libsndfile. Actually that is the only way to make a portable usage
 * of this library :D
 */
template <class Range>
std::size_t file_output<Range>::put (const range& data)
{
    return detail::file_output_put_impl (
        _file,
        reinterpret_cast<const typename sound::sample_type<Range>::type*> (
            data.frames ()),
        data.size ());
}

template <class Range>
std::size_t file_output<Range>::seek (std::ptrdiff_t offset, seek_dir dir)
{
    return detail::file_seek_impl (_file, offset, dir);
}

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_FILE_OUTPUT_TPP_ */

