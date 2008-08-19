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
#include "object/ObjectFactoryManager.h"

using namespace std;

namespace psynth
{

void ObjectFactoryManager::registerFactory(ObjectFactory& of)
{
    m_objfact[of.getName()] = &of;
}

Object* ObjectFactoryManager::create(const std::string& name,
				     const audio_info& info)
{
    ObjectFactoryMap::iterator it = m_objfact.find(name);

    /*
    cout << " *** CREATING " << name << endl;
    if (it != m_objfact.end())
	cout << "*** SUCCESS!! \n";
    else
	cout << "*** ERROR!! \n";
    */
    
    if (it != m_objfact.end())
	return it->second->create(info);
    
    return NULL;
}

} /* namespace psynth */
