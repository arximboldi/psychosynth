/**
 *  Time-stamp:  <2011-03-07 19:15:17 raskolnikov>
 *
 *  @file        jack_output.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 17:44:25 2011
 *
 *  Jack output implementation.
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

#ifndef PSYNTH_IO_JACK_OUTPUT_TPP_
#define PSYNTH_IO_JACK_OUTPUT_TPP_

#include <psynth/new_io/jack_output.hpp>

namespace psynth
{
namespace io
{

template <typename Range>
struct jack_support
{
    typedef sound::num_samples<Range>::type channels;    
    typedef mpl::and_<mpl::eq_<sound::sample_type<Range>, sound::bits32sf>,
                      sound::is_planar<Range>::type>::type
    is_supported;
};

template <typename Range>
jack_output<Range>::jack_output (const std::string& client,
                                 const std::string& server,
                                 std::size_t        rate)
    : jack_raw_output (client.c_str (),
                       server.c_str (),
                       rate,
                       jack_support<Range>::channels::value)
{
}

template <typename Range>
jack_output<Range>::jack_output (const std::string& client,
                                 std::size_t        rate)
    : jack_raw_output (client.c_str (),
                       0,
                       rate,
                       jack_support<Range>::channels::value)
{
}

template <typename Range>
std::size_t jack_output<Range>::put (const range& data)
{
    return put_on_raw (*this, data);
}

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_JACK_OUTPUT_TPP_ */


