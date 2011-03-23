/**
 *  Time-stamp:  <2011-03-23 12:22:30 raskolnikov>
 *
 *  @file        file_common.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 16:45:28 2011
 *
 *  Common utilities for both file input and output.
 *
 *  @todo Think of a refactor that avoids having to import indirectly
 *  async_base in this file and all the output and input interfaces.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psyhcosynth.
 *   
 *  Psyhcosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psyhcosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_IO_FILE_COMMON_H_
#define PSYNTH_IO_FILE_COMMON_H_

#include <psynth/io/output.hpp>
#include <psynth/io/input.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_ERROR (error, file_error);
PSYNTH_DECLARE_ERROR (file_error, file_open_error);
PSYNTH_DECLARE_ERROR (file_error, file_param_error);
PSYNTH_DECLARE_ERROR (file_error, file_seek_error);

template <class Range>
class file_support;

template <class Range>
struct file_is_supported
{
    typedef typename file_support<Range>::is_supported type;
};


/**
 * Supported file formats.
 * 
 * @todo Fill with the rest of sndfile formats.
 * @todo Implement MP3 support. That would require some refactoring.
 */
enum class file_fmt
{
    wav = 0,
    aiff,
    au,
    ogg,
    flac,
    num_formats
};

/**
 * Seek position.
 */
enum class seek_dir
{
    beg = 0,
    cur,
    end
};

template <typename Range>
class file_output_base : public output<Range>
{
public:
    virtual std::size_t seek (std::ptrdiff_t offset,
                              seek_dir dir = seek_dir::beg) = 0;
};


template <typename Range>
class file_input_base : public input<Range>
{
public:
    virtual std::size_t frame_rate () const = 0;
    
    virtual std::size_t seek (std::ptrdiff_t offset,
                              seek_dir dir = seek_dir::beg) = 0;

    virtual std::size_t length () const = 0;
};

template <typename Range>
class dummy_file_input : public file_input_base<Range>
{
public:
    typedef Range range;
    
    std::size_t frame_rate () const
    { return 44100; }

    std::size_t length () const
    { return 0; }
    
    std::size_t seek (std::ptrdiff_t offset,
                      seek_dir dir = seek_dir::beg)
    { return 0; }
    
    virtual std::size_t take (const range& data)
    {
        detail::dummy_input_take_impl ();
        typename range::value_type zero (
            sound::sample_traits<
                typename sound::sample_type<range>::type
                >::zero_value ());
        fill_frames (data, zero);
        return data.size ();
    }
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/io/file_common.tpp>

#endif /* PSYNTH_IO_FILE_COMMON_H_ */

