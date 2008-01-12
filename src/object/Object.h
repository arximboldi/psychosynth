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

#ifndef OBJECT_H
#define OBJECT_H

#include <list>
#include <vector>
#include <cmath>
#include <set>
#include <iostream>

#include "psychosynth.h"
#include "common/ControlBuffer.h"
#include "common/AudioBuffer.h"
#include "common/Mutex.h"
#include "common/Vector2D.h"

const int OBJ_NULL_ID = -1;

class Object
{
public:
    enum LinkType {
	LINK_NONE = -1,
	LINK_AUDIO,
	LINK_CONTROL,
	LINK_TYPES
    };

    enum ParamScope {
	PARAM_LOCAL,
	PARAM_COMMON,
	PARAM_SCOPES
    };
    
    enum ParamType {
	PARAM_NONE = -1,
	PARAM_INT,       /* int */
	PARAM_FLOAT,     /* float */
	PARAM_STRING,    /* std::string */
	PARAM_VECTOR2F,  /* Vector2f */
	PARAM_TYPES
    };

    enum CommonParams {
	PARAM_POSITION = 0,
	PARAM_RADIOUS,
	PARAM_MUTE,
	N_COMMON_PARAMS
    };

    class ParamID {
    public:
	ParamScope scope;
	int id;

	ParamID(ParamScope _scope = PARAM_LOCAL, int _id = 0) :
	    scope(_scope), id(_id) {}

	bool operator== (const ParamID& param) const {
	    return scope == param.scope && id == param.id;
	}

	bool operator< (const ParamID& param) const {
	    return
		scope < param.scope ||
		(scope == param.scope && id < param.id);
	}
    };
    
    class OutSocket {
	friend class Object;

	int m_type;
	std::list<std::pair<Object*, int> > m_ref; /* Objetos y puertos destino */

    protected:
	OutSocket(int type) : m_type(type) {}
	
	void addReference(Object* obj, int port) {
	    m_ref.push_back(std::pair<Object*, int>(obj, port));
	}

	void removeReference(Object* obj, int port) {
	    m_ref.remove(std::pair<Object*, int>(obj, port));
	}
		
	void clearReferences() {
	    std::list<std::pair<Object*, int> >::iterator i;
	    for (i = m_ref.begin(); i != m_ref.end(); i++)
		(*i).first->m_in_sockets[ m_type ][ (*i).second ].set(NULL, 0);
	}

    public:
	const std::list<std::pair<Object*, int> >& getReferences() const {
	    return m_ref;
	}
    };
	
    class InSocket {
	friend class Object;

	int m_type;
	Object* m_srcobj;
	int m_srcport;

    protected:
	InSocket(int type) :
	    m_type(type), m_srcobj(NULL), m_srcport(0) {}
			
	void set(Object* srcobj, int port) {
	    m_srcobj = srcobj;
	    m_srcport = port;
	}
		
	void updateInput(const Object* caller, int caller_port_type, int caller_port) {
	    if (m_srcobj)
		m_srcobj->update(caller, caller_port_type, caller_port);
	}

	template <typename SocketDataType>
	const SocketDataType* getData(int type) const {
	    if (m_srcobj)
		return m_srcobj->getOutput<SocketDataType>(type, m_srcport);
	    else
		return NULL;
	}
	
    public:
	Object* getSourceObject() const {
	    return m_srcobj;
	}

	int getSourceSocket() const {
	    return m_srcport;
	}
    };

private:

    class Param {
	Mutex m_lock;
	int m_type;
	bool m_changed;
	void* m_src;
	void* m_dest;

    public:
	Param() :
	    m_type(PARAM_NONE), m_changed(false), m_src(NULL), m_dest(NULL) {}

	~Param() {
	    clear();
	}

	void clear();
	
	void configure(int type, void* dest);

	int type() const {
	    return m_type;
	};
	
	template <typename T>
	void set(const T& d) {
	    m_lock.lock();
	    m_changed = true;
	    *static_cast<T*>(m_src) = d;
	    m_lock.unlock();
	}

	template <typename T>
	void get(T& d) const {
	    m_lock.lock();
	    d = *static_cast<T*>(m_src);
	    m_lock.unlock();
	}
	
	void update() {
	    m_lock.lock();
	    if (m_changed) {
		switch(m_type) {
		case PARAM_INT:
		    *static_cast<int*>(m_dest) = *static_cast<int*>(m_src);
		    break;
		case PARAM_FLOAT:
		    *static_cast<float*>(m_dest) = *static_cast<float*>(m_src);
		    break;
		case PARAM_STRING:
		    *static_cast<std::string*>(m_dest) = *static_cast<std::string*>(m_src);
		    break;
		case PARAM_VECTOR2F:
		    *static_cast<Vector2f*>(m_dest) = *static_cast<Vector2f*>(m_src);
		    break;
		default: break;
		};
		m_changed = false;
	    }
	    m_lock.unlock();
	}
    };

    AudioInfo m_audioinfo;
    std::vector<AudioBuffer>   m_outdata_audio;
    std::vector<ControlBuffer> m_outdata_control;
    std::vector<OutSocket>     m_out_sockets[LINK_TYPES];
    std::vector<InSocket>      m_in_sockets[LINK_TYPES];
    std::vector<Param>         m_params[PARAM_SCOPES];
    int m_id;
    int m_type;
    
    Vector2f m_param_position;
    float m_param_radious;
    int m_param_mute;
    
    /* For !m_single_update, contains the objects that has
     * been updated (<obj_id, port_id>) */
    std::set<std::pair<int,int> > m_updated_links[LINK_TYPES];
    bool m_updated;
    bool m_single_update;
    
    Mutex m_paramlock;

    
    bool canUpdate(const Object* caller, int caller_port_type,
		   int caller_port);

    void configureCommonParam(int id, int type, void* val) {
	m_params[PARAM_COMMON][id].configure(type, val);
    }
    
protected:
    template <typename SocketDataType>
    SocketDataType* getOutput(int type, int socket) {
	if (m_param_mute)
	    return NULL;
	
	/* TODO: Find a way to do type checking */
	switch(type) {
	case LINK_AUDIO:
	    return reinterpret_cast<SocketDataType*>(&(m_outdata_audio[socket]));
	    break;
	case LINK_CONTROL:
	    return reinterpret_cast<SocketDataType*>(&(m_outdata_control[socket]));
	    break;
	default:
	    break;
	}
	
	return NULL;
    }
	
    virtual void doUpdate(const Object* caller, int caller_port_type, int caller_port) = 0;
    virtual void doAdvance() = 0;
    
    void configureLocalParam(int id, int type, void* val) {
	m_params[PARAM_LOCAL][id].configure(type, val);
    }
	
public:
    Object(const AudioInfo& prop, int type, int params,
	   int inaudiosocks, int incontrolsocks,
	   int outaudiosocks, int outcontrolsocks,
	   bool single_update = true);
	
    virtual ~Object();
    
    int getType() const {
	return m_type;
    };

    /* Only to be used by ObjectManager */
    void setID(int id) {
	m_id = id;
    }
	
    int getID() const {
	return m_id;
    }

    void updateParams();

    void updateInputs();
    
    void update(const Object* caller, int caller_port_type, int caller_port);
	
    void advance() {
	m_updated = false;
	if (!m_single_update) {
	    m_updated_links[LINK_AUDIO].clear();
	    m_updated_links[LINK_CONTROL].clear();
	}

	doAdvance();
    }
	
    const AudioInfo& getAudioInfo() const {
	return m_audioinfo;
    }

    const AudioInfo& getInfo() const {
	return m_audioinfo;
    }
    
    void connectIn(int type, int in_socket, Object* src, int out_socket);

    int getParamType(ParamID id) {
	return m_params[id.scope][id.id].type();
    };
    
    template <typename T>
    void setParam(ParamID id, const T& val) {
	m_params[id.scope][id.id].set(val);
    }

    template <typename T>
    void getParam(ParamID id, T& val) const {
	m_params[id.scope][id.id].get(val);
    }

    template <typename SocketDataType>
    const SocketDataType* getOutput(int type, int socket) const {
	return getOutput<SocketDataType>(type, socket);
    }

    template <typename SocketDataType>
    const SocketDataType* getInput(int type, int socket) const {
	return m_in_sockets[type][socket].getData<SocketDataType>(type);
    }

    const OutSocket& getOutSocket(int type, int socket) const {
	return m_out_sockets[type][socket];
    }
    
    const InSocket& getInSocket(int type, int socket) const {
	return m_in_sockets[type][socket];
    }
    
    int getNumOutput(int type) const {
	return m_out_sockets[type].size();
    }
	
    int getNumInput(int type) const {
	return m_in_sockets[type].size();
    }

    int getId() const {
	return m_id;
    };
	
    void setId(int id) {
	m_id = id;
    };
        
    float distanceTo(const Object &obj) const {
	return m_param_position.distance(obj.m_param_position);
    };
	
    float sqrDistanceTo(const Object &obj) const {
	return m_param_position.sqrDistance(obj.m_param_position);
    };

    float distanceToCenter() const {
	return m_param_position.length();
    };
	
    float sqrDistanceToCenter() const {
	return m_param_position.sqrLength();
    };
    
    float distanceTo(const Vector2f& obj) const {
	return m_param_position.distance(obj);
    };
	
    float sqrDistanceTo(const Vector2f &obj) const {
	return m_param_position.sqrDistance(obj);
    };
};

#endif /* OBJECT_H */
