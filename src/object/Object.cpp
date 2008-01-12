/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#include "psychosynth.h"
#include "object/Object.h"

#include <cmath>

using namespace std;

void Object::Param::clear()
{
    switch(m_type) {
    case PARAM_INT:
	delete static_cast<int*>(m_src);
	break;
    case PARAM_FLOAT:
	delete static_cast<float*>(m_src);
	break;
    case PARAM_STRING:
	delete static_cast<std::string*>(m_src);
	break;
    case PARAM_VECTOR2F:
	delete static_cast<Vector2f*>(m_src);
	break;
    default: break;
    }
}

void Object::Param::configure(int type, void* dest)
{
    m_dest = dest;
    
    if (type != m_type) {
	clear();
		
	m_type = type;

	switch(m_type) {
	case PARAM_NONE:
	    m_src = NULL;
	    break;
	case PARAM_INT:
	    m_src = new int;
	    *static_cast<int*>(m_src) = *static_cast<int*>(m_dest);
	    break;
	case PARAM_FLOAT:
	    m_src = new float;
	    *static_cast<float*>(m_src) = *static_cast<float*>(m_dest);
	    break;
	case PARAM_STRING:
	    m_src = new string;
	    *static_cast<string*>(m_src) = *static_cast<string*>(m_dest);
	    break;
	case PARAM_VECTOR2F:
	    m_src = new Vector2f;
	    *static_cast<Vector2f*>(m_src) = *static_cast<Vector2f*>(m_dest);
	    break;
	default: break;
	}
    }
}


Object::Object(const AudioInfo& info, int type, int loc_params,
	       int n_in_audio, int n_in_control,
	       int n_out_audio, int n_out_control,
	       bool single_update) :
    m_audioinfo(info),
    m_outdata_audio(n_out_audio, AudioBuffer(info)),
    m_outdata_control(n_out_control, ControlBuffer(info.block_size)),
    m_id(OBJ_NULL_ID),
    m_type(type),
    m_param_position(0,0),
    m_param_radious(5.0f),
    m_param_mute(false),
    m_updated(false),
    m_single_update(single_update)
{
    m_params[PARAM_LOCAL].resize(loc_params);
    m_params[PARAM_COMMON].resize(N_COMMON_PARAMS);

    configureCommonParam(PARAM_POSITION, PARAM_VECTOR2F, &m_param_position);
    configureCommonParam(PARAM_RADIOUS, PARAM_FLOAT, &m_param_radious);
    configureCommonParam(PARAM_MUTE, PARAM_INT, &m_param_mute);
    
    m_out_sockets[LINK_AUDIO].resize(n_out_audio, OutSocket(LINK_AUDIO));
    m_out_sockets[LINK_CONTROL].resize(n_out_control, OutSocket(LINK_CONTROL));
    m_in_sockets[LINK_AUDIO].resize(n_in_audio, InSocket(LINK_AUDIO));
    m_in_sockets[LINK_CONTROL].resize(n_in_control, InSocket(LINK_CONTROL));
}

Object::~Object()
{
}

void Object::connectIn(int type, int in_socket, Object* src, int out_socket)
{
    if (m_in_sockets[type][in_socket].getSourceObject())
	m_in_sockets[type][in_socket].getSourceObject()->
	    m_out_sockets[type][out_socket].removeReference(this, in_socket);
	
    m_in_sockets[type][in_socket].set(src, out_socket);

    if (src)
	src->m_out_sockets[type][out_socket].addReference(this, in_socket);
}

inline bool Object::canUpdate(const Object* caller, int caller_port_type,
			      int caller_port)
{
    bool ret;

    if (m_single_update || !caller)
	ret = !m_updated;
    else
	ret = m_updated_links[caller_port_type].insert(std::pair<int,int>(caller->getID(),
									  caller_port)).second;
     m_updated = true;
    
    return ret;
}

void Object::updateParams()
{
    size_t j;
    
    for (j = 0; j < PARAM_SCOPES; ++j) {
	for (vector<Param>::iterator i = m_params[j].begin(); i != m_params[j].end(); ++i)
	    (*i).update();
    }
}

void Object::updateInputs()
{
    size_t i, j;
    
    for (i = 0; i < LINK_TYPES; ++i)
	for (j = 0; j < m_in_sockets[i].size(); ++j)
	    m_in_sockets[i][j].updateInput(this, i, j);
}

void Object::update(const Object* caller, int caller_port_type, int caller_port)
{
    if (canUpdate(caller, caller_port_type, caller_port)) {
	updateParams();

	if (!m_param_mute) {
	    updateInputs();
	    doUpdate(caller, caller_port_type, caller_port); 
	}
    }
}
