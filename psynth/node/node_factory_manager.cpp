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
#include "node/node_factory_manager.hpp"

using namespace std;

namespace psynth
{

void node_factory_manager::register_factory (node_factory& of)
{
    m_node_fact [of.get_name ()] = &of;
}

node* node_factory_manager::create (const std::string& name,
				    const audio_info& info)
{
    node_factory_map::iterator it = m_node_fact.find (name);

    /*
    cout << " *** CREATING " << name << endl;
    if (it != m_objfact.end())
	cout << "*** SUCCESS!! \n";
    else
	cout << "*** ERROR!! \n";
    */
    
    if (it != m_node_fact.end ())
	return it->second->create (info);
    
    return NULL;
}

} /* namespace psynth */
