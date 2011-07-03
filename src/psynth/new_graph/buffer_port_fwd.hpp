/**
 *  Time-stamp:  <2011-07-03 10:42:05 raskolnikov>
 *
 *  @file        buffer_port_fwd.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sun Jul  3 10:39:18 2011
 *
 *  @brief Buffer port forwards.
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

#ifndef PSYNTH_GRAPH_BUFFER_PORT_FWD_HPP_
#define PSYNTH_GRAPH_BUFFER_PORT_FWD_HPP_

#include <psynth/base/declare.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_TEMPLATE (buffer_in_port, typename);
PSYNTH_DECLARE_TEMPLATE (buffer_out_port, typename)
PSYNTH_DECLARE_TEMPLATE (buffer_forward_port, typename);
PSYNTH_DECLARE_TEMPLATE (soft_buffer_in_port, typename);
PSYNTH_DECLARE_TEMPLATE (soft_buffer_forward_port, typename);

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_BUFFER_PORT_FWD_HPP_ */
