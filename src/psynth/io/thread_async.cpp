/**
 *  Time-stamp:  <2011-03-09 17:47:57 raskolnikov>
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

#include "thread_async.hpp"

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
    _thread.join ();
}

void thread_async::run ()
{
    while (state () == async_state::running)
        iterate ();
}

} /* namespace io */
} /* namespace psynth */
