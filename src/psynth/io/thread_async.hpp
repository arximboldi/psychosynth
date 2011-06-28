/**
 *  Time-stamp:  <2011-06-28 15:26:52 raskolnikov>
 *
 *  @file        thread_async.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 17:55:05 2011
 *
 *  Threaded asynchronous IO interface.
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

#ifndef PSYNTH_IO_THREAD_ASYNC_H_
#define PSYNTH_IO_THREAD_ASYNC_H_

#include <thread>
#include <psynth/io/async_base.hpp>

namespace psynth
{
namespace io
{

class thread_async : public detail::async_base_impl
{
public:
    typedef detail::async_base_impl::callback_type callback_type;
    
    thread_async (callback_type cb, bool realtime = false)
        : detail::async_base_impl (cb)
        , _realtime (realtime)
    {}
    
    ~thread_async () { soft_stop (); }
    
    void start ();
    void stop ();

protected:
    virtual void run ();
    virtual void iterate () {}
    
private:
    void _request_rt ();

    bool        _realtime;
    std::thread _thread;
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_THREAD_ASYNC_H_ */
