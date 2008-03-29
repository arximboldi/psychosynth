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

#include <iostream>

#include "object/ObjParam.h"
#include "common/Vector2D.h"

using namespace std;

namespace psynth
{

void ObjParam::updateIn()
{
    m_lock.lock();
    if (m_changed) {
	switch(m_type) {
	case INT:
	    *static_cast<int*>(m_dest) = *static_cast<int*>(m_src);
	    break;
	case FLOAT:
	    *static_cast<float*>(m_dest) = *static_cast<float*>(m_src);
	    break;
	case STRING:
	    *static_cast<std::string*>(m_dest) = *static_cast<std::string*>(m_src);
	    break;
	case VECTOR2F:
	    *static_cast<Vector2f*>(m_dest) = *static_cast<Vector2f*>(m_src);
	    break;
	default: break;
	};
	m_changed = false;
    }
    m_lock.unlock();
}

void ObjParam::updateOut()
{
    m_lock.lock();
    switch(m_type) {
	case INT:
	    *static_cast<int*>(m_src) = *static_cast<int*>(m_dest);
	    break;
	case FLOAT:
	    *static_cast<float*>(m_src) = *static_cast<float*>(m_dest);
	    break;
	case STRING:
	    *static_cast<std::string*>(m_src) = *static_cast<std::string*>(m_dest);
	    break;
	case VECTOR2F:
	    *static_cast<Vector2f*>(m_src) = *static_cast<Vector2f*>(m_dest);
	    break;
	default: break;
    };
    m_lock.unlock();
}

void ObjParam::clear()
{
    switch(m_type) {
    case INT:
	delete static_cast<int*>(m_src);
	break;
    case FLOAT:
	delete static_cast<float*>(m_src);
	break;
    case STRING:
	delete static_cast<std::string*>(m_src);
	break;
    case VECTOR2F:
	delete static_cast<Vector2f*>(m_src);
	break;
    default: break;
    }
}

void ObjParam::configure(int id, std::string name,
			 int type, void* dest,
			 ObjParam::Event ev)
{
    m_event = ev;
    configure(id, name, type, dest);
}

void ObjParam::configure(int id, std::string name,
			 int type, void* dest)
{
    m_id = id;
    m_name = name;
    m_dest = dest;
    
    if (type != m_type) {
	clear();
		
	m_type = type;

	switch(m_type) {
	case NONE:
	    m_src = NULL;
	    break;
	case INT:
	    m_src = new int;
	    *static_cast<int*>(m_src) = *static_cast<int*>(m_dest);
	    break;
	case FLOAT:
	    m_src = new float;
	    *static_cast<float*>(m_src) = *static_cast<float*>(m_dest);
	    break;
	case STRING:
	    m_src = new string;
	    *static_cast<string*>(m_src) = *static_cast<string*>(m_dest);
	    break;
	case VECTOR2F:
	    m_src = new Vector2f;
	    *static_cast<Vector2f*>(m_src) = *static_cast<Vector2f*>(m_dest);
	    break;
	default: break;
	}
    }
}

} /* namespace psynth */
