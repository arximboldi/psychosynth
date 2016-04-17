/**
 *  Time-stamp:  <2011-06-17 17:18:52 raskolnikov>
 *
 *  @file        buffer_port.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 13 20:16:47 2011
 *
 *  @brief Buffer ports instatiations.
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

#include "buffer_port.hpp"

namespace psynth
{
namespace graph
{

template class buffer_out_port<audio_buffer>;
template class buffer_in_port<audio_buffer>;
template class defaulting_buffer_in_port<audio_buffer>;
template class buffer_forward_port<audio_buffer>;

template class buffer_out_port<sample_buffer>;
template class buffer_in_port<sample_buffer>;
template class defaulting_buffer_in_port<sample_buffer>;
template class buffer_forward_port<sample_buffer>;

} /* namespace graph */
} /* namespace psynth */
