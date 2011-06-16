/**
 *  Time-stamp:  <2011-06-11 23:09:37 raskolnikov>
 *
 *  @file        async_base.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 17:44:14 2011
 *
 *  Basic facilities for asynchronous processes.
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

#ifndef PSYNTH_IO_ASYNC_BASE_H_
#define PSYNTH_IO_ASYNC_BASE_H_

#include <functional>
#include <atomic>

#include <psynth/io/exception.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_ERROR (error, async_error);
PSYNTH_DECLARE_ERROR (async_error, async_not_running_error);
PSYNTH_DECLARE_ERROR (async_error, async_not_idle_error);

enum class async_state : int
{
    idle,
    running
};

class async_base
{
public:
    typedef std::function<void (std::size_t)> callback_type;
    
    virtual ~async_base () {}
    
    virtual void start () = 0;
    virtual void stop () = 0;
    virtual async_state state () const = 0;
    virtual void set_callback (callback_type cb) = 0;
    
    void soft_start ()
    {
        if (state () == async_state::idle)
            start ();
    }
    
    void soft_stop ()
    {
        if (state () == async_state::running)
            stop ();
    }

    void check_running ()
    {
        if (state () != async_state::running)
            throw async_not_running_error ();
    }

    void check_idle ()
    {
        if (state () != async_state::idle)
            throw async_not_idle_error ();
    }
};

namespace detail
{

/**
 * Utility class for writing async_base implementations. This code
 * actually was in async_base but it was factored out to ease writing
 * async_base decorators.
 */
class async_base_impl : public virtual async_base
{
public:
    typedef std::function<void (std::size_t)> callback_type;

    async_base_impl (callback_type cb = callback_type ())
        : _state ((int) async_state::idle)
        , _callback (cb)
    {
    }
    
    virtual ~async_base_impl () {}
    
    async_state state () const
    { return (async_state) (int) _state; }
    
    void set_callback (callback_type cb)
    {
        check_idle ();
        _callback = cb;
    }
    
protected:
    void set_state (async_state state)
    { _state = (int) state; }

    void process (std::size_t nframes)
    { _callback (nframes); }
    
private:
    std::atomic<int> _state; // FIXME: Why does GCC does not support
                             // this with an enum?
    callback_type    _callback;
};

};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_ASYNC_BASE_H_ */
