/**
 *  Time-stamp:  <2011-06-16 17:21:11 raskolnikov>
 *
 *  @file        passive_output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun 15 17:30:34 2011
 *
 *  @brief Passive output implementation.
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

#include "io/output.hpp"
#include "passive_output.hpp"

namespace psynth
{
namespace graph
{
namespace core
{

passive_output::passive_output (device_ptr out)
    : _in_input ("input", this)
    , _output (out)
{
}

void passive_output::rt_do_process (rt_process_context& ctx)
{
    if (_output)
        _output->put (_in_input.rt_range ());
}

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
