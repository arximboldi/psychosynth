/**
 *  Time-stamp:  <2011-07-07 10:52:54 raskolnikov>
 *
 *  @file        thread_async.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 17:57:21 2011
 *
 *  Threaded asynchronous IO implementation.
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

#define PSYNTH_MODULE_NAME "psynth.io.thread_async"

#include "base/logger.hpp"
#include "thread_async.hpp"

#if __GTHREADS
#include <pthread.h>
#include <string.h>
#endif

namespace psynth
{
namespace io
{

void thread_async::start ()
{
    check_idle ();
    _thread = std::thread (std::bind (&thread_async::run, this));
    set_state (async_state::running);
}

void thread_async::stop ()
{
    check_running ();
    set_state (async_state::idle);
    if (_thread.joinable ())
        _thread.join ();
}

void thread_async::run ()
{
    if (_realtime)
        _request_rt ();
    while (state () == async_state::running)
        iterate ();
}

void thread_async::_request_rt ()
{
#if __GTHREADS
    auto hdl = _thread.native_handle ();
    sched_param p;
    p.sched_priority = 1;
    auto ret = pthread_setschedparam (hdl, SCHED_FIFO, &p);
    
#ifndef PSYNTH_NO_LOG_RT_REQUEST
    if (ret)
        PSYNTH_LOG << base::log::warning
                   << "Could not set RT priority: "
                   << strerror (ret);
    else
        PSYNTH_LOG << "Thread running with RT priority.";
#endif
#endif
}

} /* namespace io */
} /* namespace psynth */
