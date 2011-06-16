/**
 *  Time-stamp:  <2011-06-10 16:21:44 raskolnikov>
 *
 *  @file        exception.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 22:59:25 2011
 *
 *  Common exception base clases for the IO layer.
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

#ifndef PSYNTH_IO_EXCEPTION_H_
#define PSYNTH_IO_EXCEPTION_H_

#include <psynth/base/exception.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_ERROR (base::error_base, error);

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_EXCEPTION_H_ */
