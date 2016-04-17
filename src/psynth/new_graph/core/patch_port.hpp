/**
 *  Time-stamp:  <2011-06-28 17:52:48 raskolnikov>
 *
 *  @file        patch_port.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 17 14:38:16 2011
 *
 *  @brief Patch port node.
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

#ifndef PSYNTH_GRAPH_CORE_PATCH_PORT_HPP_
#define PSYNTH_GRAPH_CORE_PATCH_PORT_HPP_

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/port.hpp>
#include <psynth/new_graph/buffer_port.hpp>
#include <psynth/new_graph/soft_buffer_port.hpp>
#include <psynth/new_graph/control.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

class patch_in_port_base : public node
{
public:
    virtual in_port_base& patch_port () = 0;

protected:
    void rt_do_process (rt_process_context& ctx);
};

typedef std::shared_ptr<patch_in_port_base> patch_in_port_base_ptr;

struct patch_output_hook_tag;
typedef boost::intrusive::list_member_hook<
    boost::intrusive::tag<patch_output_hook_tag>
    >
patch_output_hook;

class patch_out_port_base : public node
{
public:
    patch_output_hook _patch_output_hook;

    virtual out_port_base& patch_port () = 0;

protected:
    void rt_do_process (rt_process_context& ctx);
};

typedef std::shared_ptr<patch_out_port_base> patch_out_port_base_ptr;

namespace detail
{

template <class Base>
class port_name_control : in_control<std::string>
{
    typedef in_control<std::string> base_type;

public:
    port_name_control (std::string name, node* owner, const std::string val)
        : base_type (name, owner, val) {}

    void set (const std::string& name);
};

extern template class port_name_control<patch_in_port_base>;
extern template class port_name_control<patch_out_port_base>;

} /* namespace detail */

template <class ForwardPort>
class patch_in_port_impl : public patch_in_port_base
{
public:
    typedef typename ForwardPort::port_type port_type;

    patch_in_port_impl ()
        : _ctl_port_name ("port-name", this, "input")
        , _forward_port ("input", "output", 0, this)
    {}

    in_port<port_type>& patch_port ()
    { return _forward_port; }

private:
    detail::port_name_control<patch_in_port_base> _ctl_port_name;
    ForwardPort _forward_port;
};


template <class T>
struct patch_in_port
    : public patch_in_port_impl <forward_port<T> >
{
};

template <class T>
struct patch_buffer_in_port
    : public patch_in_port_impl <buffer_forward_port<T> >
{
};

template <class T>
struct patch_soft_buffer_in_port
    : public patch_in_port_impl <soft_buffer_forward_port<T> >
{
};

template <class ForwardPort>
class patch_out_port_impl : public patch_out_port_base
{
public:
    typedef typename ForwardPort::port_type port_type;

    patch_out_port_impl ()
        : _ctl_port_name ("port-name", this, "output")
        , _forward_port ("input", "output", this, 0)
    {}

    out_port<port_type>& patch_port ()
    { return _forward_port; }

private:
    detail::port_name_control<patch_out_port_base> _ctl_port_name;
    ForwardPort _forward_port;
};

template <class T>
struct patch_out_port
    : public patch_out_port_impl <forward_port<T> >
{
};

template <class T>
struct patch_buffer_out_port
    : public patch_out_port_impl <buffer_forward_port<T> >
{
};

template <class T>
struct patch_soft_buffer_out_port
    : public patch_out_port_impl <soft_buffer_forward_port<T> >
{
};

typedef patch_buffer_out_port<audio_buffer> audio_patch_out_port;
typedef patch_buffer_in_port<audio_buffer> audio_patch_in_port;
typedef patch_soft_buffer_out_port<audio_buffer> audio_patch_soft_out_port;
typedef patch_soft_buffer_in_port<audio_buffer> audio_patch_soft_in_port;
typedef patch_buffer_out_port<sample_buffer> sample_patch_out_port;
typedef patch_buffer_in_port<sample_buffer> sample_patch_in_port;
typedef patch_soft_buffer_out_port<sample_buffer> sample_patch_soft_out_port;
typedef patch_soft_buffer_in_port<sample_buffer> sample_patch_soft_in_port;

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_CORE_PATCH_PORT_HPP_ */
