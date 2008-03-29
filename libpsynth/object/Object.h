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

#ifndef PSYNTH_OBJECT_H
#define PSYNTH_OBJECT_H

#include <list>
#include <vector>
#include <cmath>
#include <set>
#include <iostream>

#include <libpsynth/common/ControlBuffer.h>
#include <libpsynth/common/AudioBuffer.h>
#include <libpsynth/common/Mutex.h>
#include <libpsynth/common/Vector2D.h>
#include <libpsynth/common/Deleter.h>

#include <libpsynth/object/ObjParam.h>
#include <libpsynth/object/EnvelopeSimple.h>
#include <libpsynth/object/Watch.h>

namespace psynth
{

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
    
    enum CommonParams {
	PARAM_POSITION = 0,
	PARAM_RADIOUS,
	PARAM_MUTE,
	N_COMMON_PARAMS
    };
    
    class OutSocket {
	friend class Object;

	int m_type;
	std::list<std::pair<Object*, int> > m_ref; /* Objetos y puertos destino */

    protected:
	OutSocket(int type) : m_type(type) {}

	bool isEmpty() {
	    return m_ref.empty();
	}
	
	void addReference(Object* obj, int port) {
	    m_ref.push_back(std::pair<Object*, int>(obj, port));
	}

	void removeReference(Object* obj, int port) {
	    m_ref.remove(std::pair<Object*, int>(obj, port));
	}
		
	void clearReferences() {
	    std::list<std::pair<Object*, int> >::iterator i, r;
	    for (i = m_ref.begin(); i != m_ref.end(); ) {
		r = i++;
		r->first->connectIn(m_type, r->second, NULL, 0);
	    }
	}

    public:
	const std::list<std::pair<Object*, int> >& getReferences() const {
	    return m_ref;
	}
    };
	
    class InSocket
    {
	friend class Object;

	void attachWatch(Watch* watch) {
	    m_watchs.push_back(watch);
	}

	void detachWatch(Watch* watch) {
	    m_watchs.remove(watch);
	};
	
    protected:
	int m_type;
	Object* m_srcobj;
	int m_srcport;
	std::list<Watch*> m_watchs;
	
	InSocket(int type) :
	    m_type(type), m_srcobj(NULL), m_srcport(0) {}
	
	bool isEmpty() {
	    return m_srcobj == NULL;
	}
	
	void set(Object* srcobj, int port) {
	    m_srcobj = srcobj;
	    m_srcport = port;
	}
		
	void updateInput(const Object* caller, int caller_port_type, int caller_port);

	template <typename SocketDataType>
	const SocketDataType* getData(int type) const {
	    if (m_srcobj)
		return m_srcobj->getOutput<SocketDataType>(type, m_srcport);
	    else
		return NULL;
	}
	
    public:
	virtual ~InSocket() {
	    for_each(m_watchs.begin(), m_watchs.end(), Deleter<Watch*>());
	}
	
	virtual Object* getSourceObject() const {
	    return m_srcobj;
	}

	virtual int getSourceSocket() const {
	    return m_srcport;
	}
    };

    class InSocketManual : public InSocket
    {
	friend class Object;
	
	bool must_update;
	Object* src_obj;
	int src_sock;

    public:
	InSocketManual(int type) :
	    InSocket(type),
	    must_update(false),
	    src_obj(NULL),
	    src_sock(-1) {}

	virtual Object* getSourceObject() const {
	    return src_obj;
	}

	virtual int getSourceSocket() const {
	    return src_sock;
	}	
    };

private:
    AudioInfo m_audioinfo;

    std::vector<AudioBuffer> m_outdata_audio;
    std::vector<ControlBuffer> m_outdata_control;

    std::vector<OutSocket> m_out_sockets[LINK_TYPES];
    std::vector<InSocketManual> m_in_sockets[LINK_TYPES];
    std::vector<EnvelopeSimple> m_in_envelope[LINK_TYPES];
    EnvelopeSimple m_out_envelope;
    
    std::vector<ObjParam*> m_params;
    ObjParam m_null_param;
    int m_nparam;
    
    int m_id;
    int m_type;
    std::string m_name;
    
    Vector2f m_param_position;
    float m_param_radious;
    int m_param_mute;
    
    /* For !m_single_update, contains the objects that has
     * been updated (<obj_id, port_id>) */
    std::set<std::pair<int,int> > m_updated_links[LINK_TYPES];
    bool m_updated;
    bool m_single_update;
    
    Mutex m_paramlock;

    void updateParamsOut();
    void updateInputs();
    void updateInSockets();
    void setEnvelopesDeltas();
    void updateEnvelopes();
    bool canUpdate(const Object* caller, int caller_port_type,
		   int caller_port);

protected:
    template <typename SocketDataType>
    SocketDataType* getOutput(int type, int socket) {
	if (m_param_mute && m_out_envelope.finished())
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
    virtual void onInfoChange() = 0;

    void delParam(int index);
    void addParam(const std::string&, int type, void* val);
    void addParam(const std::string&, int type, void* val, ObjParam::Event ev);

    EnvelopeSimple getInEnvelope(int type, int sock) {
	return m_in_envelope[type][sock];
    }

public:
    Object(const AudioInfo& prop, int type,
	   const std::string& name,
	   int inaudiosocks, int incontrolsocks,
	   int outaudiosocks, int outcontrolsocks,
	   bool single_update = true);
	
    virtual ~Object();
    
    int getType() const {
	return m_type;
    };

    const std::string& getName() const {
	return m_name;
    }

    /* Only to be used by ObjectManager */
    void setID(int id) {
	m_id = id;
    }
	
    int getID() const {
	return m_id;
    }

    void updateParamsIn();
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

    void setInfo(const AudioInfo& info);

    void attachWatch(int type, int in_socket, Watch* watch) {
	watch->setInfo(m_audioinfo);
	m_in_sockets[type][in_socket].attachWatch(watch);
    }

    void detachWatch(int type, int in_socket, Watch* watch) {
	m_in_sockets[type][in_socket].detachWatch(watch);
    }
    
    void connectIn(int type, int in_socket, Object* src, int out_socket);

    void forceConnectIn(int type, int in_socket, Object* src, int out_socket);

    void clearConnections();

    bool hasConnections();
    
    ObjParam& param(int id) {
	return *m_params[id];
    }

    const ObjParam& param(int id) const {
	return *m_params[id];
    }

    ObjParam& param(const std::string& name);

    const ObjParam& param(const std::string& name) const;
    
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

} /* namespace psynth */

#endif /* PSYNTH_OBJECT_H */
