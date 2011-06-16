/**
 *  Time-stamp:  <2011-06-13 17:02:05 raskolnikov>
 *
 *  @file        processor_fwd.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Jun 11 13:17:41 2011
 *
 *  @brief Processor forward declarations.
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

#ifndef PSYNTH_GRAPH_PROCESSOR_FWD_HPP_
#define PSYNTH_GRAPH_PROCESSOR_FWD_HPP_

#include <psynth/base/declare.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_TYPE (basic_process_context);
PSYNTH_DECLARE_TYPE (rt_process_context);
PSYNTH_DECLARE_TYPE (async_process_context);
PSYNTH_DECLARE_TYPE (user_process_context);
PSYNTH_DECLARE_TYPE (full_process_context);

PSYNTH_DECLARE_SHARED_TYPE (processor);

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_PROCESSOR_FWD_HPP_ */

