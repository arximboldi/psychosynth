/**
 *  Time-stamp:  <2011-06-18 12:51:47 raskolnikov>
 *
 *  @file        node.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 19:03:22 2011
 *
 *  @brief A node of the synthesis graph.
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

#ifndef PSYNTH_GRAPH_NODE_HPP_
#define PSYNTH_GRAPH_NODE_HPP_

#include <map>

#include <boost/range/iterator_range.hpp>
#include <boost/intrusive/list_hook.hpp>

#include <psynth/base/util.hpp>
#include <psynth/base/factory_manager.hpp>
#include <psynth/new_graph/exception.hpp>

#include <psynth/new_graph/core/patch_fwd.hpp>
#include <psynth/new_graph/processor_fwd.hpp>
#include <psynth/new_graph/node_fwd.hpp>
#include <psynth/new_graph/control_fwd.hpp>
#include <psynth/new_graph/port_fwd.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_ERROR (error, node_error)
PSYNTH_DECLARE_ERROR (node_error, node_component_error);
PSYNTH_DECLARE_ERROR (node_error, node_attachment_error);

struct patch_child_hook_tag;
typedef boost::intrusive::list_member_hook<
    boost::intrusive::tag<patch_child_hook_tag>
    >
patch_child_hook;

class node : private boost::noncopyable
{
    typedef std::map<std::string, in_port_base*>     in_map;
    typedef std::map<std::string, out_port_base*>    out_map;
    typedef std::map<std::string, in_control_base*>  param_map;
    typedef std::map<std::string, out_control_base*> state_map;

public:
    typedef base::ptr_iterator<
    base::map_iterator<in_map::key_type,
                       in_map::mapped_type> >
    input_iterator;
    typedef base::ptr_iterator<
        base::map_iterator<out_map::key_type,
                           out_map::mapped_type> >
    output_iterator;
    typedef base::ptr_iterator<
        base::map_iterator<param_map::key_type,
                           param_map::mapped_type> >
    param_iterator;
    typedef base::ptr_iterator<
        base::map_iterator<state_map::key_type,
                           state_map::mapped_type> >
    state_iterator;
    
    typedef boost::iterator_range<input_iterator> input_range;
    typedef boost::iterator_range<output_iterator> output_range;
    typedef boost::iterator_range<param_iterator> param_range;
    typedef boost::iterator_range<state_iterator> state_range;
    
    patch_child_hook _patch_child_hook;
    
    node ();
    
    virtual void rt_process (rt_process_context& ctx);
    virtual void rt_context_update (rt_process_context& ctx);
    virtual void rt_advance ();
    
    in_port_base& in (const std::string& name);
    const in_port_base& in (const std::string& name) const;
    input_range inputs ();
    
    out_port_base& out (const std::string& name);
    const out_port_base& out (const std::string& name) const;
    output_range outputs ();
        
    in_control_base& param (const std::string& name);
    const in_control_base& param (const std::string& name) const;
    param_range params ();
    
    out_control_base& state (const std::string& name);
    const out_control_base& state (const std::string& name) const;
    state_range states ();
        
    void register_component (in_port_base& in);
    void register_component (out_port_base& out);
    void register_component (in_control_base& param);
    void register_component (out_control_base& state);

    void unregister_component (in_port_base& in);
    void unregister_component (out_port_base& out);
    
    void attach_to_process (processor& p);
    void detach_from_process ();

    void attach_to_patch (core::patch& p);
    void detach_from_patch ();

    processor& process ()
    {
        check_attached_to_process ();
        return *_process;
    }
    
    const processor& process () const
    {
        check_attached_to_process ();
        return *_process;
    }
    
    core::patch& patch ()
    {
        check_attached_to_patch ();
        return *_patch;
    }
    
    const core::patch& patch () const
    {
        check_attached_to_patch ();
        return *_patch;
    }

    bool is_attached_to_process () const
    { return _process != 0; }
    bool is_attached_to_patch () const
    { return _patch != 0; }
    
    void check_attached_to_process (bool attached = true) const
    {
        if (is_attached_to_process () != attached)
            throw node_attachment_error ();
    }
    
    void check_attached_to_patch (bool attached = true) const
    {
        if (is_attached_to_patch () != attached)
            throw node_attachment_error ();
    }

private:
    virtual void rt_on_context_update (rt_process_context& ctx) {}
    virtual void rt_do_process (rt_process_context& ctx) {}
    
    core::patch* _patch;
    processor*   _process;
    
    in_map    _inputs;
    out_map   _outputs;
    param_map _params;
    state_map _states;

    bool _rt_processed;
    bool _rt_post_processed;
};

typedef
base::restricted_global_factory_manager<std::string, node_ptr>
node_factory;

#define PSYNTH_REGISTER_NODE_STATIC(node_type)                          \
    node_factory::registrant<node_type> node_type ## _registrant (#node_type);

#define PSYNTH_REGISTER_NODE_STATIC_AS(node_type, node_name)            \
    node_factory::registrant<node_type> node_type ## _registrant (node_name);

} /* namespace graph */

    namespace base
    {

    extern template class
    base::restricted_global_factory_manager<std::string, graph::node_ptr>;

    } /* namespace base */

} /* namespace psynth */

#endif /* PSYNTH_GRAPH_NODE_HPP_ */
