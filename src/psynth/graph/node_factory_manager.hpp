/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef PSYNTH_NODE_FACTORY_MANAGER_H
#define PSYNTH_NODE_FACTORY_MANAGER_H

#include <map>
#include <psynth/graph/node_factory.hpp>

namespace psynth
{
namespace graph
{

class node_factory_manager
{
    typedef std::map<std::string, node_factory*> node_factory_map;
    node_factory_map m_node_fact;
    
public:
    void register_factory (node_factory& of);
    node* create (const std::string& name, const audio_info& info);
};

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJECTFACTORYMANAGER_H */
