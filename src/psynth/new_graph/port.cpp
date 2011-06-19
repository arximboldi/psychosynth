/**
 *  Time-stamp:  <2011-06-18 23:26:14 raskolnikov>
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
}

void port_base::_set_owner (node* new_owner)
{
    _owner = new_owner;
}

void port_base::_set_name (std::string new_name)
{
    _name = new_name;
}

in_port_base::in_port_base (std::string name, graph::node* owner)
    : port_base (name, owner)
    , _source_port (0)
    , _rt_source_port (0)
{
    if (owner)
        owner->register_component (*this);
}

out_port_base::out_port_base (std::string name, node* owner)
    : port_base (name, owner)
{
    if (owner)
        owner->register_component (*this);
}

void out_port_base::_add_reference (in_port_base* ref)
{
    _refs.push_back (ref);

    if (_has_owner () &&
        owner ().is_attached_to_process () &&
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
    
    if (_has_owner () &&
        owner ().is_attached_to_process () &&
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

bool in_port_base::rt_in_available () const
{
    out_port_base* src = _rt_source_port;
    return src && src->rt_out_available ();
}

void check_port_compatibility (in_port_base& in, out_port_base& out)
{
    if (!in._has_owner () || !out._has_owner ())
        throw port_patch_error ();
    out.owner ().check_attached_to_patch ();
    in.owner ().check_attached_to_patch ();
    if (&out.owner ().patch () != &in.owner ().patch ())
        throw port_patch_error ();
    if (out.type () != in.type ())
        throw port_type_error ();
}

void in_port_base::connect (out_port_base& port)
{
    _connect (&port);
}

void in_port_base::disconnect ()
{
    _connect (0);
}

void in_port_base::_connect (out_port_base* source)
{
    _user_connect (source);

    // FIXME: I am using this pattern so much we should consider
    // abstracting it as a method of node.
    
    if (_has_owner () &&  // always true here
        owner ().is_attached_to_process () &&
        owner ().process ().is_running ())
    {
        auto& ctx = owner ().process ().context ();
        ctx.push_rt_event (make_rt_event ([=] (rt_process_context&) {
                    this->_rt_connect (source);
                }));
    }
    else
        this->_rt_connect (source);
}

void in_port_base::_user_connect (out_port_base* source)
{
    if (source)
        check_port_compatibility (*this, *source);
    if (_source_port)
        detail::out_port_access::del_reference (*_source_port, this);
    _source_port = source;
    if (source)
        detail::out_port_access::add_reference (*_source_port, this);
}

void in_port_base::_rt_connect (out_port_base* source)
{
    _rt_source_port = source;
}

void in_port_base::rt_process (rt_process_context& ctx)
{
    if (rt_connected ())
        rt_source ().owner ().rt_process (ctx);
}

void out_port_base::disconnect ()
{
    for (auto& r : _refs)
        r->disconnect ();
}

} /* namespace graph */
} /* namespace psynth */

