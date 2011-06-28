/**
 *  Time-stamp:  <2011-06-27 22:55:28 raskolnikov>
 *
 *  @file        control.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:19:59 2011
 *
 *  @brief Control parameter implementation.
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

#define PSYNTH_MODULE_NAME "psynth.graph.control"

#include <cassert>

#include "node.hpp"
#include "control.hpp"

namespace psynth
{
namespace graph
{

template class in_control<std::string>;
template class in_control<float>;
template class in_control<int>;
template class in_control<bool>;

template class out_control<std::string>;
template class out_control<float>;
template class out_control<int>;
template class out_control<bool>;

PSYNTH_DEFINE_ERROR_WHAT (control_type_error, "Wrong control type.");

const control_meta default_control_meta;

control_base::control_base (const std::string& name, node* owner)
    : _name (name)
    , _owner (owner)
{
    // assert (owner); // FIXME
}

in_control_base::in_control_base (const std::string& name, node* owner)
    : control_base (name, owner)
{
    // assert (owner); // fixme
    if (_owner)
        owner->register_component (*this);
}

out_control_base::out_control_base (const std::string& name, node* owner)
    : control_base (name, owner)
{
    // assert (owner); // fixme
    if (_owner)
        owner->register_component (*this);
}

} /* namespace graph */
} /* namespace psynth */
