/**
 *  Time-stamp:  <2011-03-22 17:34:17 raskolnikov>
 *
 *  @file        scaler.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar 21 18:54:10 2011
 *
 *  @brief Scaler template implementation.
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

#ifndef PSYNTH_SYNTH_SCALER_TPP_
#define PSYNTH_SYNTH_SCALER_TPP_

#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/forwards.hpp>
#include <boost/mpl/and.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits.hpp>

namespace psynth
{

namespace mpl = boost::mpl;

namespace synth
{

template <class Range>
struct scaler_support
{
    typedef mpl::and_ <boost::is_same<typename sound::sample_type<Range>::type,
                                     sound::bits32sf>,
                       mpl::not_<sound::is_planar<Range> > >
    is_supported;
};

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_SCALER_TPP_ */
