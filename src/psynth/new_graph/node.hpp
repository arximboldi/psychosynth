/**
 *  Time-stamp:  <2011-06-09 21:22:21 raskolnikov>
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

#include <psynth/new_graph/node_fwd.hpp>

namespace psynth
{
namespace graph
{

class node : public boost::noncopyable
{
public:
    input& in (const std::string& name);
    output& out (const std::string& name);
    const input& in (const std::string& name) const;
    const output& out (const std::string& name) const;
   
    control& param (const std::string& name);
    const control& param (const std::string& name) const;
    
    virtual void process (process_context& ctx) = 0;

    void register_component (input& in);
    void register_component (output& out);
    void register_component (control& ctl);

private:
    typedef std::map<std::string, input*>   in_map;
    typedef std::map<std::string, output*>  out_map;
    typedef std::map<std::string, control*> ctl_map;
};

typedef base::restricted_global_factory_manager<std::string, node_ptr>
node_factory;

#define PSYNTH_REGISTER_NODE_STATIC(node_type) \
    node_factory::registrant node_type ## _registrant (#node_type);

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_NODE_HPP_ */
