/**
 *  Time-stamp:  <2011-03-07 21:13:15 raskolnikov>
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

#include <psynth/new_io/output.hpp>

namespace psynth
{
namespace io
{

template <typename Range>
struct oss_support;

template <typename Range>
class oss_output : public async_output<Range>,
                   public oss_raw_output
{
public:
    static_assert (oss_support<Range>::is_suported::value,
                   "Range output format not supported by OSS.");
    
    typedef Range range;
    
    oss_output (const std::string& device,
                std::size_t        buffer_size,
                std::size_t        rate);

    std::size_t put (const range& data);
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/new_io/oss_output.tpp>

#endif /* PSYNTH_IO_OSS_OUTPUT_H_ */

