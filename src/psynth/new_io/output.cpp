/**
 *  Time-stamp:  <2011-03-16 22:34:35 raskolnikov>
 *
 *  @file        output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 13:13:42 2011
 *
 *  @brief Implementation of some output facilities.
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

#define PSYNTH_MODULE_NAME "psynth.io.output"

#include "base/logger.hpp"
#include "output.hpp"

namespace psynth
{
namespace io
{

namespace detail
{

void dummy_output_put_impl ()
{
    PSYNTH_LOG_DEBUG << "dummy_output::put";
}

void dummy_output_start_impl ()
{
    PSYNTH_LOG_DEBUG << "dummy_output::start";
}

void dummy_output_stop_impl ()
{
    PSYNTH_LOG_DEBUG << "dummy_output::stop";
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */
