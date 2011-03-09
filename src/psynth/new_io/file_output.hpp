/**
 *  Time-stamp:  <2011-03-09 01:45:16 raskolnikov>
 *
 *  @file        file_raw_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 21:17:02 2011
 *
 *  File raw output.
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

#ifndef PSYNTH_IO_FILE_OUTPUT_H_
#define PSYNTH_IO_FILE_OUTPUT_H_

#include <string>
#include <sndfile.h>

#include <psynth/new_io/file_common.hpp>

namespace psynth
{
namespace io
{

template <class Range>
class file_output : public file_output_base<Range>
{
    typedef file_output_base<Range> base;
public:
    typedef typename base::range range;
    
    static_assert (file_support<Range>::is_supported::value,
                   "Audio file format not supported.");
    
    file_output (const char*  fname,
                 file_fmt     format,
                 std::size_t  rate);

    ~file_output ();

    std::size_t put (const range& data);

    std::size_t seek (std::ptrdiff_t offset, seek_dir dir);
    
private:
    SNDFILE* _file;
    SF_INFO  _info;
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/new_io/file_output.tpp>

#endif /* PSYNTH_IO_FILE_OUTPUT_H_ */

