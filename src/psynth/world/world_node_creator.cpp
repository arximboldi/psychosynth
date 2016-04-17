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

#include <algorithm>
#include "world/world_node_creator.hpp"
#include "world/world.hpp"

using namespace std;

namespace psynth
{

void world_node_creator::destroy()
{
    for (param_maker_list::iterator it = m_param.begin();
	 it != m_param.end();
	 ++it)
	delete it->second;
    m_param.clear();
}

void world_node_creator::copy(const world_node_creator& obj)
{
    m_name = obj.m_name;
    for (param_maker_list::const_iterator it = obj.m_param.begin();
	 it != obj.m_param.end();
	 ++it)
	m_param.push_back(make_pair(it->first, it->second->clone()));
}

world_node world_node_creator::create (world& table)
{
    world_node obj;

    obj = table.add_node (m_name);

    if (!obj.is_null ()) {
	for (param_maker_list::iterator it = m_param.begin();
	     it != m_param.end();
	     ++it)
	    it->second->apply(obj, it->first);
    }

    return obj;
}

world_node_creator::param_maker_base* world_node_creator::find (const std::string& name)
{
    for (param_maker_list::iterator it = m_param.begin();
	 it != m_param.end();
	 ++it)
	if (it->first == name)
	    return it->second;

    return NULL;
}

} /* namespace psynth */
