/**
 *  Time-stamp:  <2010-10-17 20:42:06 raskolnikov>
 *
 *  @file        singleton.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Apr  7 13:19:05 2009
 *
 *  Implementation of the singleton class.
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "base/singleton.hpp"

namespace psynth
{
namespace base
{

PSYNTH_DEFINE_ERROR_WHERE_WHAT (singleton_error,
				"singleton",
				"Dead reference to singleton.")

} /* namespace base */
} /* namespace psynth */

