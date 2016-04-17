/**
 *  Time-stamp:  <2011-06-09 20:53:19 raskolnikov>
 *
 *  @file        factory_manager.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 20:51:58 2011
 *
 *  @brief Translation unit for factory_manager.
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

#include "factory_manager.hpp"

namespace psynth
{
namespace base
{

#define PSYNTH_MODULE_NAME "psynth.base.factory"
PSYNTH_DEFINE_ERROR_WHAT (factory_error, "Unknown factory key.");

} /* namespace base */
} /* namespace psynth */
