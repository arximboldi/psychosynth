/**
 *  Time-stamp:  <2011-06-10 16:21:28 raskolnikov>
 *
 *  @file        exception.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:20:50 2011
 *
 *  @brief Graph module common exceptions.
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

#ifndef PSYNTH_GRAPH_EXCEPTION_HPP_
#define PSYNTH_GRAPH_EXCEPTION_HPP_

#include <psynth/base/exception.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_ERROR (base::error_base, error);

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_EXCEPTION_HPP_ */
