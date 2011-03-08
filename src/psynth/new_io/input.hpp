/**
 *  Time-stamp:  <2011-03-08 16:26:42 raskolnikov>
 *
 *  @file        input.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 16:20:55 2011
 *
 *  Input base class.
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

#ifndef PSYNTH_IO_INPUT_H_
#define PSYNTH_IO_INPUT_H_

namespace psynth
{
namespace io
{

/**
 *
 * @todo All the IO interface (actually the output class and derivates
 * too) assume that buffers do not model ranges. This is why we can
 * safely assume a <i>const Range&</i> in this interface. 
 */
template <class Range>
class input
{
public:
    typedef Range range;

    virtual std::size_t take (const Range& r) = 0;
};

/**
 * @todo Not used yet.
 */
template <typename Range>
class async_input : public output<Range>,
                    public virtual async_base
{
public:
    virtual std::size_t buffer_size () const = 0;
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_INPUT_H_ */

