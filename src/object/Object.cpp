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

#include "psychosynth.h"
#include "object/Object.h"

#include <cmath>

using namespace std;

void Object::Param::configure(int type, void* dest)
{
    if (type != m_type) {
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
	default: break;
	}
		
	m_type = type;

	switch(m_type) {
	case PARAM_NONE:
	    m_src = NULL;
	    break;
	case PARAM_INT:
	    m_src = new int;
	    break;
	case PARAM_FLOAT:
	    m_src = new float;
	    break;
	case PARAM_STRING:
	    m_src = new string;
	    break;
	default: break;
	}
    }
    m_dest = dest;
}


Object::Object(const AudioInfo& info, int type, int params,
	       int n_in_audio, int n_in_control,
	       int n_out_audio, int n_out_control) :
    m_audioinfo(info),
    m_outdata_audio(n_out_audio, AudioBuffer(info)),
    m_outdata_control(n_out_control, ControlBuffer(info.block_size)),
    m_params(params),
    m_id(OBJ_NULL_ID),
    m_type(type),
    m_x(0), m_y(0),
    m_updated(false)
{
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

void Object::update()
{
    if (!m_updated) {
	size_t i, j;
	
	m_updated = true;

	m_paramlock.lock();
	for (vector<Param>::iterator i = m_params.begin(); i != m_params.end(); ++i)
	    (*i).update();
	m_paramlock.unlock();
	
	for (i = 0; i < LINK_TYPES; ++i)
	    for (j = 0; j < m_in_sockets[i].size(); ++j)
		m_in_sockets[i][j].updateInput();

	doUpdate(); 
    }
}
