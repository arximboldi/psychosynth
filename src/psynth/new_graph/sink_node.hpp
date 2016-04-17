/**
 *  Time-stamp:  <2011-06-11 20:38:05 raskolnikov>
 *
 *  @file        sink.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Jun 11 20:34:23 2011
 *
 *  @brief Base class for sink nodes.
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

#ifndef PSYNTH_GRAPH_SINK_HPP_
#define PSYNTH_GRAPH_SINK_HPP_

#include <psynth/new_graph/node.hpp>

namespace psynth
{
namespace graph
{

class sink_node : public virtual node
{
};

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_SINK_HPP_ */
