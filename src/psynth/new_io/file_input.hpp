/**
 *  Time-stamp:  <2011-03-08 17:50:11 raskolnikov>
 *
 *  @file        file_input.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 17:15:12 2011
 *
 *  Input from a file.
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

#ifndef PSYNTH_IO_FILE_INPUT_H_
#define PSYNTH_IO_FILE_INPUT_H_

#include <psynth/new_io/file_common.hpp>

namespace psynth
{
namespace io
{

template <class Range>
class file_input : public file_input_base<Range>
{
public:
    static_assert (file_support<Range>::is_supported::value,
                   "Audio file format not supported.");
    
    file_input (const char* fname);

    ~file_input ();

    std::size_t take (const range& data);

    std::size_t seek (std::ptrdiff_t offset, seek_dir dir);

    std::size_t frame_rate () const
    { return _info.samplerate; }
    
private:
    SNDFILE* _file;
    SF_INFO  _info,
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/new_io/file_input.tpp>

#endif /* PSYNTH_IO_FILE_INPUT_H_ */


