/**
 *  Time-stamp:  <2011-03-09 13:29:44 raskolnikov>
 *
 *  @file        input.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 13:22:48 2011
 *
 *  Input base class translation unit.
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

#define PSYNTH_MODULE_NAME "psynth.io.input"

#include "base/logger.hpp"
#include "input.hpp"

namespace psynth
{
namespace io
{

namespace detail
{

void dummy_input_take_impl ()
{
    PSYNTH_LOG_DEBUG << "dummy_input::take";
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */
