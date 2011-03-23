/**
 *  Time-stamp:  <2011-03-21 18:06:55 raskolnikov>
 *
 *  @file        util.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar 21 18:04:40 2011
 *
 *  @brief Synthesis utilities of various kinds.
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

#ifndef PSYNTH_SYNTH_UTIL_H_
#define PSYNTH_SYNTH_UTIL_H_

#include <psynth/sound/forwards.hpp>

namespace psynth
{
namespace synth
{

/**
 * @todo Fix and probably remove.
 */
template <class Range>
std::size_t find_hill (const Range& data, std::size_t start)
{
    const auto zero = sound::sample_traits<
        typename sound::sample_type<Range>::type
        >::zero_value ();
   ++start;
   
   while (start < (size_t) data.size () &&
          (data [start-1] != zero ||
           data [start] == zero))
       ++start;
    
    return start;
}

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_UTIL_H_ */

