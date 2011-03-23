/**
 *  Time-stamp:  <2011-03-18 19:53:17 raskolnikov>
 *
 *  @file        oss_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 16:44:50 2011
 *
 *  OSS output system.
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

#ifndef PSYNTH_IO_OSS_OUTPUT_H_
#define PSYNTH_IO_OSS_OUTPUT_H_

#include <psynth/io/oss_raw_output.hpp>
#include <psynth/io/output.hpp>

namespace psynth
{
namespace io
{

template <typename Range>
struct oss_support;

template <typename Range>
struct oss_is_supported
{
    typedef typename oss_support<Range>::is_supported type;
};

template <typename Range>
class oss_output : public oss_raw_output,
                   public async_output<Range>
{
public:
    static_assert (oss_support<Range>::is_supported::value,
                   "Range output format not supported by OSS.");
    
    typedef Range range;
    
    oss_output (const std::string& device,
                std::size_t        buffer_size,
                std::size_t        rate,
                callback_type      cb = callback_type ());

    std::size_t put (const range& data);

    std::size_t buffer_size () const
    { return oss_raw_output::buffer_size (); }
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/io/oss_output.tpp>

#endif /* PSYNTH_IO_OSS_OUTPUT_H_ */

