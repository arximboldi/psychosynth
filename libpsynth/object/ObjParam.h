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

#ifndef PSYNTH_OBJPARAM
#define PSYNTH_OBJPARAM

#include <libpsynth/common/mutex.h>
#include <string>

namespace psynth
{

#include <libpsynth/common/FastDelegate.h>
using namespace fastdelegate;

class ObjParam
{
public:
    typedef FastDelegate1<ObjParam&, void> Event;

private:
    friend class Object;
	
    mutex m_lock;
    std::string m_name;
    int m_id;
    int m_type;
    bool m_changed;
    Event m_event;
    void* m_src;
    void* m_dest;
    
    void clear();
    void configure(int id, std::string name, int type, void* dest);
    void configure(int id, std::string name, int type, void* dest, Event ev);

    void updateIn();
    void updateOut();
    
public:
    enum Type {
	NONE = -1,
	INT,       /* int */
	FLOAT,     /* float */
	STRING,    /* std::string */
	VECTOR2F,  /* Vector2f */
	TYPES
    };

    ObjParam() :
	m_type(NONE),
	m_changed(false),
	m_src(NULL),
	m_dest(NULL)
	{}

    ObjParam(const ObjParam& obj) :
	m_type(NONE),
	m_changed(false) {
	configure(obj.m_id, obj.m_name, obj.m_type, obj.m_dest, obj.m_event);
    }
    
    ~ObjParam() {
	clear();
    }

    ObjParam& operator= (const ObjParam& obj) {
	if (this != &obj)
	    configure(obj.m_id, obj.m_name, obj.m_type, obj.m_dest, obj.m_event);

	return *this;
    }
    
    int getID() const {
	return m_id;
    }

    const std::string& getName() const {
	return m_name;
    }
	
    int type() const {
	return m_type;
    };
	
    template <typename T>
    void set(const T& d) {
	m_lock.lock();
	m_changed = true;
	*static_cast<T*>(m_src) = d;
	m_lock.unlock();
	if (!m_event.empty()) m_event(*this);
    }

    template <typename T>
    void get(T& d) const {
	m_lock.lock();
	d = *static_cast<T*>(m_src);
	m_lock.unlock();
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OBJPARAM */
