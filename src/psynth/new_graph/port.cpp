/**
 *  Time-stamp:  <2011-06-16 20:53:02 raskolnikov>
 *
 *  @file        port.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Jun 13 11:05:26 2011
 *
 *  @brief Ports implementation.
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

#define PSYNTH_MODULE_NAME "psynth.graph.port"

#include "processor.hpp"
#include "node.hpp"
#include "port.hpp"

namespace psynth
{
namespace graph
{

PSYNTH_DEFINE_ERROR (port_error);
PSYNTH_DEFINE_ERROR_WHAT (
    port_type_error,
    "Can not connect ports of different types.");
PSYNTH_DEFINE_ERROR_WHAT (
    port_patch_error,
    "Can not connect ports of different patches.");

const port_meta default_port_meta;

port_base::port_base (std::string name, node* owner)
  : _name (name)
  , _owner (owner)
{
    assert (owner); // FIXME
}

void out_port_base::_add_reference (in_port_base* ref)
{
    _refs.push_back (ref);

    if (owner ().is_attached_to_process () &&
        owner ().process ().is_running ())
    {
        auto& ctx = owner ().process ().context ();
        ctx.push_rt_event (make_rt_event ([=] (rt_process_context&) {
                    this->_rt_refs.push_back (*ref);
                }));
    }
    else
        _rt_refs.push_back (*ref);
}

void out_port_base::_del_reference (in_port_base* ref)
{
    _refs.remove (ref);
    
    if (owner ().is_attached_to_process () &&
        owner ().process ().is_running ())
    {
        auto& ctx = owner ().process ().context ();
        ctx.push_rt_event (make_rt_event ([=] (rt_process_context&) {
                    this->_rt_refs.remove_if (
                        base::make_equal_id (*ref));
                }));
    }
    else
        _rt_refs.remove_if (base::make_equal_id (*ref));   
}

void in_port_base::connect (out_port_base& port)
{
    // FIXME: Re-think thread safety of having the same variable for
    // all threads. Probably we should again split in rt and non-rt
    // versions of the state.
    
    port.owner ().check_attached_to_patch ();
    owner ().check_attached_to_patch ();
    if (&port.owner ().patch () != &owner ().patch ())
        throw port_patch_error ();
    if (port.type () != type ())
        throw port_type_error ();
    
    if (_source_port)
        detail::out_port_access::del_reference (*_source_port, this);
    _source_port = &port;
    detail::out_port_access::add_reference (*_source_port, this);
}

void in_port_base::disconnect ()
{
    if (_source_port)
        detail::out_port_access::del_reference (*_source_port, this);
    _source_port = 0;
}

void out_port_base::disconnect ()
{
    for (auto& r : _refs)
        r->disconnect ();
}

} /* namespace graph */
} /* namespace psynth */

