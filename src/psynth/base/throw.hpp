/**
 *  Time-stamp:  <2011-06-07 20:43:06 raskolnikov>
 *
 *  @file        throw.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Jun  7 16:11:03 2011
 *
 *  @brief Throwing interface that better swits the logging system.
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

#ifndef PSYNTH_BASE_THROW_HPP_
#define PSYNTH_BASE_THROW_HPP_

#include <psynth/base/logger.hpp>
#include <psynth/base/exception.hpp>

namespace psynth
{
namespace base
{

namespace detail
{

template <class Base, bool IsPsynth>
class exception_wrapper : public virtual Base
{
public:
    exception_wrapper (const char* where)
        : _where (where)
        , _stream (logger::self ().path (where))
    {
        _stream.set_level (Base::level ());
    }

    exception_wrapper (const exception_wrapper&) = default;
    exception_wrapper (exception_wrapper&&) = default;
    exception_wrapper& operator= (const exception_wrapper&) = default;

    ~exception_wrapper () throw () {}
    
    const char* where () const throw ()
    { return _where; }

    const char* what () const throw ()
    { return _stream.message ().c_str (); }

    int level () const throw ()
    { return _stream.level (); }

    log_stream_adapter& stream ()
    { return _stream; }
    
private:
    const char*        _where;
    log_stream_adapter _stream;
};


template <class Base>
class exception_wrapper<Base, false> : public virtual Base
                                     , public virtual exception
{
public:
    exception_wrapper (const char* where)
        : Base ("") // HACK: This allows throwing logic_error etc, but
                    // forbidds raw std::exception and others.
        , _where (where)
        , _stream (logger::self ().path (where))
    {
        _stream.set_level (default_error_level ());
    }

    exception_wrapper (const exception_wrapper&) = default;
    exception_wrapper (exception_wrapper&&) = default;
    exception_wrapper& operator= (const exception_wrapper&) = default;

    ~exception_wrapper () throw () {}
    
    const char* what () const throw ()
    { return _stream.message ().c_str (); }

    const char* where () const throw ()
    { return _where; }
    
    int level () const throw ()
    { return _stream.level (); }

    log_stream_adapter& stream ()
    { return _stream; }
    
private:
    const char*        _where;
    log_stream_adapter _stream;
};

} /* namespace detail */

template <class Exception>
class throw_with : public boost::noncopyable
{
public:
    explicit throw_with (const char* where = "")
        : _exception (where)
        , _owner (false)
    {}

    throw_with (throw_with&& other)
        : _exception (std::move (other._exception))
        , _owner (true)
    {
        other._owner = false;
    }
    
    ~throw_with ()
    {
        if (_owner)
            throw _exception;
    }

private:
    template <class E, typename T>
    friend throw_with<E> operator<< (throw_with<E>, const T&);

    typedef detail::exception_wrapper<
        Exception,
        std::is_base_of<exception, Exception>::value >
    exception_type;

    exception_type _exception;
    bool           _owner;
};

template <class E, typename T>
throw_with<E> operator<< (throw_with<E> ts, const T& what)
{
    ts._exception.stream () << what;
    return std::move (ts);
}

#define PSYNTH_THROW(exception) ::psynth::base::throw_with<exception> (PSYNTH_MODULE_NAME)


} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_THROW_HPP_ */


