/**
 *  Time-stamp:  <2011-06-18 13:16:39 raskolnikov>
 *
 *  @file        control.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 16:19:28 2011
 *
 *  @brief Graph node parameter.
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

#ifndef PSYNTH_GRAPH_CONTROL_HPP_
#define PSYNTH_GRAPH_CONTROL_HPP_

#include <atomic>
#include <map>
#include <mutex>

#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>

#include <psynth/base/type_value.hpp>
#include <psynth/new_graph/node_fwd.hpp>
#include <psynth/new_graph/event.hpp>
#include <psynth/new_graph/exception.hpp>

namespace psynth
{
namespace graph
{

PSYNTH_DECLARE_ERROR (error, control_type_error);

typedef std::map<std::string, boost::any> control_meta;
extern const control_meta default_control_meta;

/**
 *  Base class providing runtime polymorphic access to controls.
 *
 *  @note The implementation of the get and set methods relies on
 *  static_cast<control<T>*> (this) workin when it matches.
 */
class control_base : private boost::noncopyable
{
public:
    virtual ~control_base () {}

    virtual base::type_value type () const = 0;
    virtual const control_meta& meta () const = 0;

    virtual std::string str () const = 0;    

    const node& owner () const
    { return *_owner; }

    node& owner ()
    { return *_owner; }

    std::string name ()
    { return _name; }
    
protected:    
    control_base (const std::string& name, node* owner);

protected:
    std::string _name;
    node*       _owner;
};


/**
 *  Base class providing runtime polymorphic access to input controls.
 *
 *  @note The implementation of the get and set methods relies on
 *  boost::polymorphic_downcast<control<T>*> (this) workin when it matches.
 */
class in_control_base : public control_base
{
public:
    virtual void str (const std::string& s) = 0;

    template <typename T>
    T get () const;

    template <typename T>
    const T& rt_get () const;

    template <typename T>
    void set (const T&);
        
protected:
    in_control_base (const std::string& name, node* owner);
};


/**
 *  Base class providing runtime polymorphic access to output controls.
 *
 *  @note The implementation of the get and set methods relies on
 *  boost::polymorphic_downcast<control<T>*> (this) workin when it matches.
 */
class out_control_base : public control_base
{
public:
    template <typename T>
    T get () const;

    template <typename T>
    const T& rt_get () const;

    template <typename T>
    void rt_set (const T&, rt_process_context& ctx);

protected:
    out_control_base (const std::string& name, node* owner);
};

template <typename T, bool IsFundamental>
class out_control_impl : public out_control_base
{
public:
    T get () const
    { return _value; }
    
    T rt_get () const
    { return _value; }

    void rt_set (const T& val, rt_process_context& ctx)
    { _value = val; }

protected:
    out_control_impl (const std::string& name, node* owner, T val)
        : out_control_base (name, owner), _value (val) {}

private:
    T _value; // FIXME: Substitute with std::atomic<T> whenever GCC
              // properly implements it ;)
};

template <typename T>
class out_control_impl<T, false> : public out_control_base
{
public:
    virtual const T& rt_get () const
    { return _rt_value; }

    virtual T get () const;
    
    virtual void rt_set (const T& val, rt_process_context& ctx);
    
protected:
    out_control_impl (const std::string& name, node* owner, T val)
        : out_control_base (name, owner), _value (val), _rt_value (val) {}

private:
    struct async_update_event : public async_event
    {
        async_update_event (out_control_impl& ctl, const T& val)
            : _ctl (ctl), _new_value (val) {}
        void operator () (async_process_context& ctx);
    private:
        out_control_impl& _ctl;
        T _new_value;
    };

    friend class async_update_event;
    
    T _value;
    T _rt_value;
    std::mutex _mutex;
};


/**
 *  A control for notifying the state of the node from the real time
 *  process out to the user thread.
 */
template <typename T>
class out_control : public out_control_impl<T, std::is_fundamental<T>::value>
{
    typedef out_control_impl<T, std::is_fundamental<T>::value> impl_base;

public:
    out_control (const std::string& name, const T& value=T(), node* owner=0)
        : impl_base (name, owner, value) {}
    
    base::type_value type () const
    { return typeid (T); }

    const control_meta& meta () const 
    { return default_control_meta; } // FIXME !!!!
    
    std::string str () const
    { return boost::lexical_cast<std::string> (this->get ()); }

};


/**
 *  A control for sending parameter values from the user thread to the
 *  node internal state.
 */
template <typename T>
class in_control : public in_control_base
{
public:
    in_control (const std::string& name, const T& value=T(), node* owner=0)
        : in_control_base (name, owner)
        , _value (value)
        , _rt_value (value)
        , _is_updated (false)
    {}

    base::type_value type () const
    { return typeid (T); }

    const control_meta& meta () const 
    { return default_control_meta; } // FIXME !!!!
    
    virtual T get () const
    { return _value; }
    
    virtual T rt_get () const
    { return _rt_value; }

    virtual void set (const T&);
    
    void str (const std::string& s)
    { set (boost::lexical_cast<T> (s)); }

    std::string str () const
    { return boost::lexical_cast<std::string> (this->get ()); }
    

    bool rt_is_updated ()
    { return _is_updated; }
    
private:
    struct rt_update_event : public rt_event
    {
        rt_update_event (in_control& ctl, const T& val)
            : _ctl (ctl), _new_rt_value (val) {}
        void operator () (rt_process_context& ctx);
    private:
        in_control& _ctl;
        T _new_rt_value;
    };
    
    struct rt_post_update_event : public rt_event
    {
        rt_post_update_event (in_control& ctl)
            : _ctl (ctl) {}
        void operator () (rt_process_context& ctx);        
    private:
        in_control& _ctl;
    };
    
    friend class rt_update_event;
    friend class rt_post_update_event;

    T _value;
    T _rt_value;
    bool _is_updated;
};

extern template class in_control<float>;
extern template class in_control<int>;
extern template class in_control<bool>;

extern template class out_control<float>;
extern template class out_control<int>;
extern template class out_control<bool>;

} /* namespace graph */
} /* namespace psynth */

#include <psynth/new_graph/control.tpp>

#endif /* PSYNTH_GRAPH_CONTROL_HPP_ */
