/**
 *  Time-stamp:  <2011-06-18 13:40:07 raskolnikov>
 *
 *  @file        patch_port.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 17 16:00:01 2011
 *
 *  @brief Patch port implementation.
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

#include "patch.hpp"
#include "patch_port.hpp"

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_REGISTER_NODE_STATIC (audio_patch_out_port);
PSYNTH_REGISTER_NODE_STATIC (sample_patch_out_port);
PSYNTH_REGISTER_NODE_STATIC (audio_patch_in_port);
PSYNTH_REGISTER_NODE_STATIC (sample_patch_in_port);

void patch_in_port_base::rt_do_process (rt_process_context& ctx)
{
    node::rt_process (ctx);
    auto& p = patch_port ();
    if (p.rt_connected ())
        p.rt_source ().owner ().rt_process (ctx);
}

void patch_out_port_base::rt_do_process (rt_process_context& ctx)
{
    node::rt_process (ctx);
}

namespace detail
{

template <class Base>
void port_name_control<Base>::set (const std::string& name)
{
    auto& node = static_cast<Base&> (owner ()); // Safe!

    if (node.is_attached_to_patch ())
    {
        auto& patch = node.patch ();
        auto& port  = node.patch_port ();
        auto old    = port.name ();
            
        patch.unregister_component (port); // Thus must not fail
        auto guard = base::make_guard ([&] {
                port._set_name (old);
                patch.register_component (port);
            });
        port._set_name (name);
        patch.register_component (port);
        guard.dismiss ();
    }
    else
    {
        auto& port = node.patch_port ();
        port._set_name (name);
    }
    
    base_type::set (name);
}

template class port_name_control<patch_in_port_base>;
template class port_name_control<patch_out_port_base>;

} /*namespace detail */

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */
