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

#include <mutex>

#include <boost/function.hpp>
#include <string>

namespace psynth
{
namespace graph
{

class node_param
{
public:
    typedef boost::function <void (node_param&)> event;

    enum type
    {
	NONE = -1,
	INT,       /* int */
	FLOAT,     /* float */
	STRING,    /* std::string */
	VECTOR2F,  /* Vector2f */
	TYPES
    };

    node_param () :
	m_type(NONE),
	m_changed(false),
	m_src(NULL),
	m_dest(NULL)
	{}

    node_param (const node_param& obj) :
	m_type(NONE),
	m_changed(false) {
	configure(obj.m_id, obj.m_name, obj.m_type, obj.m_dest, obj.m_event);
    }

    ~node_param () {
	clear();
    }

    node_param& operator= (const node_param& obj) {
	if (this != &obj)
	    configure(obj.m_id, obj.m_name, obj.m_type, obj.m_dest, obj.m_event);

	return *this;
    }

    int get_id () const {
	return m_id;
    }

    const std::string& get_name () const {
	return m_name;
    }

    int type() const {
	return m_type;
    };

    template <typename T>
    void set (const T& d) {
	{
	    std::unique_lock<std::mutex> lock (m_mutex);
	    m_changed = true;
	    *static_cast<T*>(m_src) = d;
	}
	if (!m_event.empty()) m_event(*this);
    }

    template <typename T>
    void get (T& d) const {
	m_mutex.lock ();
	d = *static_cast<T*>(m_src);
	m_mutex.unlock ();
    }

private:
    friend class node0;

    mutable std::mutex m_mutex;

    std::string m_name;
    int m_id;
    int m_type;
    bool m_changed;
    event m_event;
    void* m_src;
    void* m_dest;

    void clear ();
    void configure (int id, std::string name, int type, void* dest);
    void configure (int id, std::string name, int type, void* dest, event ev);

    void update_in ();
    void update_out ();
};

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_OBJPARAM */
