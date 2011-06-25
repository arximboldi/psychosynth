/**
 *  Time-stamp:  <2011-06-25 23:09:01 raskolnikov>
 *
 *  @file        control.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sun Jun 12 13:06:19 2011
 *
 *  @brief Control templates implementation.
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

#ifndef PSYNTH_GRAPH_CONTROL_TPP_
#define PSYNTH_GRAPH_CONTROL_TPP_

#include <boost/cast.hpp>
#include <psynth/new_graph/node_fwd.hpp>
#include <psynth/new_graph/control.hpp>
#include <psynth/new_graph/processor.hpp>

namespace psynth
{
namespace graph
{

template <typename T>
T in_control_base::get () const
{
    if (type () != base::type_value (typeid (T)))
        throw control_type_error ();
    return boost::polymorphic_downcast<
        const typed_in_control_base<T>*>(this)->get ();
}

template <typename T>
T out_control_base::get () const
{
    if (type () != base::type_value (typeid (T)))
        throw control_type_error ();
    return boost::polymorphic_downcast<
        const typed_out_control_base<T>*>(this)->get ();
}

template <typename T>
const T& in_control_base::rt_get () const
{
    assert (type () == base::type_value (typeid (T)));
    return boost::polymorphic_downcast<
        const typed_in_control_base<T>*>(this)->rt_get ();
}

template <typename T>
const T& out_control_base::rt_get () const
{
    assert (type () == base::type_value (typeid (T)));
    return boost::polymorphic_downcast<
        const typed_out_control_base<T>*>(this)->rt_get ();
}

template <typename T>
void in_control_base::set (const T& val)
{
    if (type () != base::type_value (typeid (T)))
        throw control_type_error ();
    boost::polymorphic_downcast<
        typed_in_control_base<T>*>(this)->set (val);
}
    
template <typename T>
void out_control_base::rt_set (const T& val, rt_process_context& ctx)
{
    assert (type () == base::type_value (typeid (T)));
    boost::polymorphic_downcast<
        typed_out_control_base<T>*>(this)->rt_set (val, ctx);
}

namespace detail
{

template <typename T>
void out_control_impl<T, false>::rt_set (const T& val, rt_process_context& ctx)
{
    _rt_value = val;
    ctx.push_event<async_update_event> (*this, val);
}

template <typename T>
T out_control_impl<T, false>::get () const
{
    auto g = base::make_unique_lock (_mutex);
    auto ret = _value;
    return ret;
}

template <typename T>
void out_control_impl<T, false>::async_update_event::operator () (
    async_process_context& ctx)
{
    auto g = base::make_unique_lock (_ctl.mutex);
    _ctl._value = _new_value;
}

} /* namespace detail */


template <typename T>
void in_control<T>::set (const T& val)
{
    _value  = val;
    if (this->owner ().is_attached_to_process () &&
        this->owner ().process ().is_running ())
    {
        user_process_context& ctx = this->owner ().process ().context ();
        ctx.push_rt_event<rt_update_event> (*this, val);
    }
    else
    {
        _rt_value = val;
    }
}

template <typename T>
void in_control<T>::rt_update_event::operator () (rt_process_context& ctx)
{
    std::swap (_ctl._rt_value, _new_rt_value);
    if (!_ctl._is_updated)
    {
        _ctl._is_updated = true;
        ctx.push_rt_event<rt_post_update_event> (_ctl);
    }
}

template <typename T>
void in_control<T>::rt_post_update_event::operator () (rt_process_context& ctx)
{
    _ctl._is_updated = false;
}

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CONTROL_TPP_ */
