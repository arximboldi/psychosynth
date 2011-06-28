/**
 *  Time-stamp:  <2011-06-28 18:08:12 raskolnikov>
 *
 *  @file        patch.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:30:22 2011
 *
 *  @brief Patch implementation.
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

#define PSYNTH_MODULE_NAME "psynth.graph.core.patch"

#include <iostream>

#include "new_graph/core/patch_port.hpp"
#include "new_graph/processor.hpp"
#include "new_graph/port.hpp"
#include "patch.hpp"

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_REGISTER_NODE_STATIC (patch);

PSYNTH_DEFINE_ERROR (patch_child_error);

void patch::rt_process (rt_process_context& ctx)
{
    node::rt_process (ctx);
    for (auto& n : _rt_outputs)
        n.rt_process (ctx);
}

void patch::rt_context_update (rt_process_context& ctx)
{
    node::rt_context_update (ctx);

#if 0
    // Actually not needed by now.
    for (auto& n : _rt_childs)
        n->rt_context_update (ctx);
#endif
}

void patch::rt_advance ()
{
    node::rt_advance ();
    for (auto& n : _rt_childs)
        n.rt_advance ();
}
    
node_ptr patch::add (node_ptr child)
{
    if (child->is_attached_to_patch () &&
        &child->patch () == this)
        return child;
    child->check_attached_to_patch (false);
    
    child->attach_to_patch (*this);
    _childs.push_back (child);
    execute_rt ([=] {
            this->_rt_childs.push_back (*child);
        });
    
    if (is_attached_to_process ())
        process ().notify_add_node (child);

    if (patch_in_port_base_ptr port =
        std::dynamic_pointer_cast<patch_in_port_base> (child))
    {
        auto& p = port->patch_port ();
        this->register_component (p);
        p._set_owner (this);
    }

    if (patch_out_port_base_ptr port =
        std::dynamic_pointer_cast<patch_out_port_base> (child))
    {
        auto& p = port->patch_port ();
        this->register_component (p);
        p._set_owner (this);
        execute_rt ([=] {
                this->_rt_outputs.push_back (*port);
            });
    }
    
    return child;
}

void patch::remove (node_ptr child)
{
    if (!child->is_attached_to_patch () ||
        &child->patch () != this)
        throw patch_child_error ();

    for (auto& p : child->inputs ())
        p.disconnect ();
    for (auto& p : child->outputs ())
        p.disconnect ();

    if (patch_in_port_base_ptr port =
        std::dynamic_pointer_cast<patch_in_port_base> (child))
    {
        auto& p = port->patch_port ();
        p.disconnect ();
        this->unregister_component (p);
        p._set_owner (0);
    }

    if (patch_out_port_base_ptr port =
        std::dynamic_pointer_cast<patch_out_port_base> (child))
    {
        // TODO: Maybe we would run the port without being attached at
        // all. We can do a check before running.
        execute_rt ([=] {
                this->_rt_outputs.remove_if (base::make_equal_id (*port));
            });
        auto& p = port->patch_port ();
        p.disconnect ();
        this->unregister_component (p);
        p._set_owner (0);
    }
    
    child->detach_from_patch ();
    _childs.remove (child);

    execute_rt ([=] {
            this->_rt_childs.remove_if (
                base::make_equal_id (*child));
        });

    if (is_attached_to_process ())
        process ().notify_remove_node (child);
}

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
