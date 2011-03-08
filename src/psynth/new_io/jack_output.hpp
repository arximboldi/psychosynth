/**
 *  Time-stamp:  <2011-03-07 21:13:40 raskolnikov>
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

namespace psynth
{
namespace io
{

template <typename Range>
struct jack_support;

template <typename Range>
class jack_output : public async_output<Range>,
                    public jack_raw_output
{
public:
    static_assert (jack_support<Range>::is_suported::value,
                   "Range output format not supported by JACK.");
    
    typedef Range range;
    
    jack_output (const std::string& client,
                 const std::string& server,
                 std::size_t        rate);

    jack_output (const std::string& device,
                 std::size_t        rate);

    std::size_t put (const range& data);
};

} /* namespace io */
} /* namespace psynth */

#include <pysnth/io/jack_output.tpp>

#endif /* PSYNTH_IO_JACK_OUTPUT_H_ */

