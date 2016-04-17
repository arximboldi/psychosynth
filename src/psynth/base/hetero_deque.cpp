/**
 *  Time-stamp:  <2011-06-08 16:33:25 raskolnikov>
 *
 *  @file        hetero_deque.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun  8 14:59:10 2011
 *
 *  @brief Heterogeneous deque module implementation.
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

#include "hetero_deque.hpp"

#define PSYNTH_MODULE_NAME "psynth.base.hetero_deque"

namespace psynth
{
namespace base
{

PSYNTH_DEFINE_ERROR_WHAT(hetero_deque_empty,
                         "Can not extract from empty deque.");

} /* namespace base */
} /* namespace psynth */
