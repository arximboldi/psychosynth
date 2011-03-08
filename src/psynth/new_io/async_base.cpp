/**
 *  Time-stamp:  <2011-03-04 17:55:48 raskolnikov>
 *
 *  @file        async_base.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 17:51:08 2011
 *
 *  Asynchronous infrastructure implementation.
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

#define PSYNTH_MODULE_NAME "psynth.io.async"

#include "async_base.hpp"

namespace psynth
{
namespace io
{

PSYNTH_DEFINE_ERROR (async_error);
PSYNTH_DEFINE_ERROR_WHAT (async_not_running_error,
                          "Asynchronous I/O device is not running.");
PSYNTH_DEFINE_ERROR_WHAT (async_not_idle_error,
                          "Asynchronous I/O device is not idle.");

} /* namespace io */
} /* namespace psynth */
