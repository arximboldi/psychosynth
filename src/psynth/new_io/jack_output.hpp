/**
 *  Time-stamp:  <2011-03-09 19:23:51 raskolnikov>
 *
 *  @file        jack_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 17:40:14 2011
 *
 *  Jack output system.
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

#ifndef PSYNTH_IO_JACK_OUTPUT_H_
#define PSYNTH_IO_JACK_OUTPUT_H_

#include <psynth/new_io/output.hpp>
#include <psynth/new_io/jack_raw_output.hpp>

namespace psynth
{
namespace io
{

template <typename Range>
struct jack_support;

template <typename Range>
struct jack_is_supported
{
    typedef typename jack_support<Range>::is_supported type;
};

template <typename Range>
class jack_output : public async_output<Range>,
                    public jack_raw_output
{
public:
    static_assert (jack_support<Range>::is_supported::value,
                   "Range output format not supported by JACK.");
    
    typedef Range range;
    
    jack_output (const std::string& client,
                 const std::string& server,
                 std::size_t        rate,
                 callback_type      cb = callback_type ());

    jack_output (const std::string& client,
                 std::size_t        rate,
                 callback_type      cb = callback_type ());

    std::size_t put (const range& data);

    std::size_t buffer_size () const
    { return jack_raw_output::buffer_size (); }
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/new_io/jack_output.tpp>

#endif /* PSYNTH_IO_JACK_OUTPUT_H_ */

