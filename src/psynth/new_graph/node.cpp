/**
 *  Time-stamp:  <2011-06-16 20:13:31 raskolnikov>
 *
 *  @file        node.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 21:24:29 2011
 *
 *  @brief New node implementation.
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

#define PSYNTH_MODULE_NAME "psynth.graph.node"

#include "base/throw.hpp"
#include "node.hpp"
#include "port.hpp"
#include "control.hpp"

namespace psynth
{

namespace base
{
template class
base::restricted_global_factory_manager<std::string, graph::node_ptr>;
} /* namespace base */

namespace graph
{

PSYNTH_DEFINE_ERROR (node_error);
PSYNTH_DEFINE_ERROR (node_attachment_error);
PSYNTH_DEFINE_ERROR_WHAT (node_component_error, "Unknown component.");

node::node ()
    : _patch (0)
    , _process (0)
    , _rt_processed (false)
{
}

void node::rt_process (rt_process_context& ctx)
{
    if (!_rt_processed)
    {
        _rt_processed = true;
        this->rt_do_process (ctx);
    }
}

void node::rt_advance ()
{
    _rt_processed = false;
}

in_port_base& node::in (const std::string& name)
{
    auto it = _inputs.find (name);
    if (it == _inputs.end ())
        PSYNTH_THROW (node_component_error)
            << "Unknown input port: " << name;
    return *it->second;
}

const in_port_base& node::in (const std::string& name) const
{
    return const_cast<node*> (this)-> in (name);
}

node::input_range node::inputs ()
{
    return input_range (_inputs.begin (), _inputs.end ());
}

out_port_base& node::out (const std::string& name)
{
    auto it = _outputs.find (name);
    if (it == _outputs.end ())
        PSYNTH_THROW (node_component_error)
            << "Unknown output port: " << name;
    return *it->second;
}

const out_port_base& node::out (const std::string& name) const
{
    return const_cast<node*> (this)-> out (name);
}

node::output_range node::outputs ()
{
    return output_range (_outputs.begin (), _outputs.end ());
}

in_control_base& node::param (const std::string& name)
{
    auto it = _params.find (name);
    if (it == _params.end ())
        PSYNTH_THROW (node_component_error)
            << "Unknown node parameter: " << name;
    return *it->second;
}

const in_control_base& node::param (const std::string& name) const
{
    return const_cast<node*> (this)-> param (name);
}

node::param_range node::params ()
{
    return param_range (_params.begin (), _params.end ());
}

out_control_base& node::state (const std::string& name)
{
    auto it = _states.find (name);
    if (it == _states.end ())
        PSYNTH_THROW (node_component_error)
            << "Unknown node parameter: " << name;
    return *it->second;
}

const out_control_base& node::state (const std::string& name) const
{
    return const_cast<node*> (this)-> state (name);
}

node::state_range node::states ()
{
    return state_range (_states.begin (), _states.end ());
}

void node::register_component (in_port_base& in)
{
    auto name = in.name ();
    auto it = _inputs.find (name);
    if (it != _inputs.end ())
        PSYNTH_THROW (node_component_error)
            << "Duplicate input port name: " << name;
    _inputs.insert (it, std::make_pair (name, &in));
}

void node::register_component (out_port_base& out)
{
    auto name = out.name ();
    auto it = _outputs.find (name);
    if (it != _outputs.end ())
        PSYNTH_THROW (node_component_error)
            << "Duplicate output port name: " << name;
    _outputs.insert (it, std::make_pair (name, &out));
}

void node::register_component (in_control_base& param)
{
    auto name = param.name ();
    auto it = _params.find (name);
    if (it != _params.end ())
        PSYNTH_THROW (node_component_error)
            << "Duplicate param control name: " << name;
    _params.insert (it, std::make_pair (name, &param));
}

void node::register_component (out_control_base& state)
{
    auto name = state.name ();
    auto it = _states.find (name);
    if (it != _states.end ())
        PSYNTH_THROW (node_component_error)
            << "Duplicate state control name: " << name;
    _states.insert (it, std::make_pair (name, &state));    
}

void node::attach_to_process (processor& p)
{
    check_attached_to_process (false);
    _process = &p;
}

void node::detach_from_process ()
{
    check_attached_to_process ();
    _process = 0;
}

void node::attach_to_patch (core::patch& p)
{
    check_attached_to_patch (false);
    _patch = &p;
}

void node::detach_from_patch ()
{
    check_attached_to_patch ();
    _patch = 0;
}

} /* namespace graph */
} /* namespace psynth */
