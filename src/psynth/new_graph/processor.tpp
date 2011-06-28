/**
 *  Time-stamp:  <2011-06-28 18:47:39 raskolnikov>
 *
 *  @file        processor.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sun Jun 12 17:07:24 2011
 *
 *  @brief Processor templates implementations.
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

#ifndef PSYNTH_GRAPH_PROCESSOR_TPP_
#define PSYNTH_GRAPH_PROCESSOR_TPP_

#include <psynth/new_graph/processor.hpp>

namespace psynth
{
namespace graph
{

template <class Event, typename... Args>
bool rt_process_context::push_rt_event (Args&&... args)
{
    return _rt_buffers.local ().push_back<Event> (
        std::forward<Args> (args) ...);
}

template <class Event, typename... Args>
bool user_process_context::push_rt_event (Args&&... args)
{
    auto g = _rt_buffers.back_policy ().guard ();
    return _rt_buffers.back ().push_back<Event> (
        std::forward<Args> (args) ...);   
}

template <class Event, typename... Args>
bool async_process_context::push_rt_event (Args&&... args)
{
    auto g = _rt_buffers.back_policy ().guard ();
    return _rt_buffers.back ().push_back<Event> (
        std::forward<Args> (args) ...);
}

template <class Event, typename... Args>
bool rt_process_context::push_async_event (Args&&... args)
{
    return _async_buffers.back ().push_back<Event> (
        std::forward<Args> (args) ...);
}

template <class Event, typename... Args>
bool user_process_context::push_async_event (Args&&... args)
{
    auto g = _async_buffers.local_policy ().guard ();
    return _async_buffers.local ().push_back<Event> (
        std::forward<Args> (args) ...);
    _async_cond.notify_all ();
}

template <class Event, typename... Args>
bool async_process_context::push_async_event (Args&&... args)
{
    auto g = _async_buffers.local_policy ().guard ();
    return _async_buffers.local ().push_back<Event> (
        std::forward<Args> (args) ...);
}

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_PROCESSOR_TPP_ */
