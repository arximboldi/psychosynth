/**
 *  Time-stamp:  <2011-06-13 19:21:08 raskolnikov>
 *
 *  @file        functor.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:55:01 2011
 *
 *  @brief Functor utilities.
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

#ifndef PSYNTH_GRAPH_FUNCTOR_HPP_
#define PSYNTH_GRAPH_FUNCTOR_HPP_

namespace psynth
{
namespace base
{

/**
 *  Useful when we need polymorphic functors.
 *  @todo Move elsewhere?
 */
template <typename Ret, typename... Args>
struct dynamic_functor
{
    virtual ~dynamic_functor () {};
    virtual Ret operator () (Args... args) = 0;
};

template <typename Fn, typename Ret, typename... Args>
struct fn_dynamic_functor : dynamic_functor <Ret, Args...>
{
    fn_dynamic_functor (const Fn& f)
        : _fn (f) {}

    fn_dynamic_functor (Fn&& f)
        : _fn (std::move (f)) {}

    fn_dynamic_functor (const fn_dynamic_functor&) = default;
    fn_dynamic_functor (fn_dynamic_functor&&) = default;

    fn_dynamic_functor& operator= (const fn_dynamic_functor&) = default;

    Ret operator () (Args... args)
    {
        return _fn (std::forward<Args> (args)...);
    }

private:
    Fn _fn;
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_FUNCTOR_HPP_ */
