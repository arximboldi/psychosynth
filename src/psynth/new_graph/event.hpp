/**
 *  Time-stamp:  <2011-06-27 20:34:53 raskolnikov>
 *
 *  @file        event.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:45:27 2011
 *
 *  @brief Events that are passed around.
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

#ifndef PSYNTH_GRAPH_EVENT_HPP_
#define PSYNTH_GRAPH_EVENT_HPP_

#include <psynth/base/functor.hpp>
#include <psynth/new_graph/processor_fwd.hpp>

namespace psynth
{
namespace graph
{

typedef base::dynamic_functor<void, rt_process_context&> rt_event;
typedef base::dynamic_functor<void, async_process_context&> async_event;

/**
 *  @todo Change when inheriting constructors or template aliases are added. 
 */
template <typename Fn>
struct fn_rt_event : public base::fn_dynamic_functor <
    Fn, void, rt_process_context&>
{
    typedef base::fn_dynamic_functor <Fn, void, rt_process_context&> base;
    
    fn_rt_event (const Fn& f)
        : base (f) {}
    fn_rt_event (Fn&& f)
        : base (std::move (f)) {}
};

template <typename Fn>
struct fn_async_event : base::fn_dynamic_functor <
    Fn, void, async_process_context&>
{
    typedef base::fn_dynamic_functor <Fn, void, async_process_context&> base;
    
    fn_async_event (const Fn& f)
        : base (f) {}
    fn_async_event (Fn&& f)
        : base (std::move (f)) {}
};

template <class Fn>
fn_rt_event<Fn> make_rt_event (Fn&& f)
{
    return fn_rt_event<Fn> (f);
}

template <class Fn>
fn_async_event<Fn> make_async_event (Fn&& f)
{
    return fn_async_event<Fn> (f);
}

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_EVENT_HPP_ */
