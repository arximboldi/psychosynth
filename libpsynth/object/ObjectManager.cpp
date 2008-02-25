/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#include "object/ObjectManager.h"

using namespace std;

namespace psynth
{

ObjectManager::ObjectManager() :
    m_objmap(),
    m_outputs()
{
}

ObjectManager::~ObjectManager()
{
    map<int, Object*>::iterator map_iter;
    for (map_iter = m_objmap.begin(); map_iter != m_objmap.end(); ++map_iter)
	delete (*map_iter).second;
}

bool ObjectManager::attachObject(Object* obj, int id)
{
    if (!m_objmap.insert(pair<int, Object*>(id, obj)).second)
	return false;

    obj->setID(id);
    
    ObjectOutput* out = dynamic_cast<ObjectOutput*>(obj);

    if (out != NULL) {
	if (!out->setManager(this)) {
	    m_objmap.erase(id);
	    return false; /* Already attached to an ObjectManager!! */
	}
		
	m_outputs.push_back(out);
    }
	
    return true;
}

bool ObjectManager::detachObject(int id)
{
    map<int, Object*>::iterator iter;
    iter = m_objmap.find(id);
	
    if (iter == m_objmap.end())
	return false;

    detachObject(iter);
    
    return true;
}

void ObjectManager::detachObject(Iterator it)
{
    map<int, Object*>::iterator& iter = it;
    
    ObjectOutput* out = dynamic_cast<ObjectOutput*>((*iter).second);
    if (out != NULL) {
	m_outputs.remove(out);
	out->setManager(NULL);
    }

    (*iter).second->setID(OBJ_NULL_ID);
    m_objmap.erase(iter);
}

void ObjectManager::remove(Iterator it)
{
    Object* obj = it->second;
    detachObject(it);
    delete obj;
}

void ObjectManager::update()
{
    map<int, Object*>::iterator map_iter;
    for (map_iter = m_objmap.begin(); map_iter != m_objmap.end(); ++map_iter) {
	(*map_iter).second->advance();
    }
	
    list<ObjectOutput*>::iterator out_iter;
    for (out_iter = m_outputs.begin(); out_iter != m_outputs.end(); ++out_iter) {
	(*out_iter)->update(NULL, -1, -1);
    }
}

} /* namespace psynth */
