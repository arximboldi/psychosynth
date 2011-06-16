/**
 *  Time-stamp:  <2011-06-16 19:36:35 raskolnikov>
 *
 *  @file        port.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sun Jun 12 20:23:04 2011
 *
 *  @brief Ports that interconnect nodes.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_PORT_HPP_
#define PSYNTH_PORT_HPP_

#include <map>
#include <list>
#include <atomic>

#include <boost/any.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/intrusive/list.hpp>

#include <psynth/base/type_value.hpp>
#include <psynth/base/iterator.hpp>
#include <psynth/new_graph/exception.hpp>
#include <psynth/new_graph/node_fwd.hpp>
#include <psynth/new_graph/processor_fwd.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_ERROR (error, port_error);
PSYNTH_DECLARE_ERROR (port_error, port_type_error);
PSYNTH_DECLARE_ERROR (port_error, port_patch_error);

typedef std::map<std::string, boost::any> port_meta;
extern const port_meta default_port_meta;

class out_port_base;
class in_port_base;

class port_base : private boost::noncopyable
{
public:
    virtual base::type_value type () const = 0;
    virtual const port_meta& meta () const = 0;
    virtual void rt_context_update (const rt_process_context&) {}

    std::string name ()
    { return _name; }
    
    node& owner ()
    { return *_owner; }
    const node& owner () const
    { return *_owner; }
    
protected:
    port_base (std::string name, node* owner);
    
private:
    std::string _name;
    node* _owner;
};

struct out_port_referee_hook_tag;
typedef boost::intrusive::list_member_hook<
    boost::intrusive::tag<out_port_referee_hook_tag>
    >
out_port_referee_hook;

class in_port_base : public port_base
{   
public:
    out_port_referee_hook _out_port_referee_hook;
    
    void connect (out_port_base& port);
    void disconnect ();
    
    bool connected ()
    { return _source_port == 0; }
    bool rt_connected ()
    { return _source_port == 0; }

    out_port_base& source ()
    { return *_source_port; }
    out_port_base& rt_source ()
    { return *_source_port; }
    const out_port_base& source () const
    { return *_source_port; }
    const out_port_base& rt_source () const
    { return *_source_port; }
    
protected:
    in_port_base (std::string name, graph::node* owner)
        : port_base (name, owner)
        , _source_port (0)
    {}
    
    // FIXME: Duplicate?
    std::atomic<out_port_base*> _source_port;
};


namespace detail { class out_port_access; }

class out_port_base : public port_base
{
    typedef std::list<in_port_base*> reference_list;
    typedef boost::intrusive::list<
        in_port_base,
        boost::intrusive::member_hook<
            in_port_base,
            out_port_referee_hook,
            &in_port_base::_out_port_referee_hook> >
    rt_reference_list;

public:
    typedef base::ptr_iterator<reference_list::iterator>
    reference_iterator;
    typedef base::ptr_iterator<reference_list::const_iterator>
    reference_const_iterator;
    typedef boost::iterator_range<reference_iterator>
    reference_range;
    typedef boost::iterator_range<reference_const_iterator>
    const_reference_range;

    void disconnect ();
    
    bool connected ()
    { return _refs.empty (); }
    
    bool rt_connected ()
    { return _rt_refs.empty (); } // FIXME: Add concurrent access to refs policy

    reference_range references ()
    { return reference_range (_refs.begin (), _refs.end ()); }
    const_reference_range references () const
    { return const_reference_range (_refs.begin (), _refs.end ()); }
    
protected:
    out_port_base (std::string name, node* owner)
        : port_base (name, owner) {}

private:
    void _add_reference (in_port_base*);
    void _del_reference (in_port_base*);
    
    friend class detail::out_port_access;
    
    reference_list    _refs;
    rt_reference_list _rt_refs;
};

namespace detail
{

struct out_port_access
{
    static void add_reference (out_port_base& self, in_port_base* param)
    { self._add_reference (param); }

    static void del_reference (out_port_base& self, in_port_base* param)
    { self._del_reference (param); }
};

} /* namespace detail */


template <typename T>
class out_port : public out_port_base
{
public:
    out_port (std::string name, node* owner, const T& value = T())
        : out_port_base (name, owner)
        , _data (value){}
    
    T& rt_get ()
    { return _data; }
    
    const T& rt_get () const
    { return _data; }

    base::type_value type () const
    { return typeid (T); }

    const port_meta& meta () const 
    { return default_port_meta; }  // FIXME !!!
    
private:
    T _data;
};

template <typename T>
class in_port : public in_port_base
{
public:
    in_port (std::string name, node* owner)
        : in_port_base (name, owner) {}
    
    base::type_value type () const
    { return typeid (T); }

    const port_meta& meta () const 
    { return default_port_meta; } // FIXME !!!

    const T& rt_get () const
    {
        // Relies on the connection being made right!
        return static_cast<const out_port<T>&> (this->source ()).rt_get ();
    }
    
private:
};

} /* namespace graph */
} /* namespace psynth */

#include <psynth/new_graph/port.tpp>

#endif /* PSYNTH_PORT_HPP_ */
