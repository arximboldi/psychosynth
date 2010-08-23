/**
 *  Time-stamp:  <2009-10-23 13:27:37 raskolnikov>
 *
 *  @file        timer.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  A time counter and delay generator.
 */

/*
 *  Copyright (C) 2007 Juan Pedro Bolívar Puente
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

#include <unistd.h>
#include <iostream>
#include <sys/time.h>

#include "base/timer.hpp"

using namespace std;

namespace psynth
{

timer::timer ()
 : m_fps (0)
 , m_rate (0)
{
    reset ();
}

timer::timer (int fpsrate)
{
    force_fps (fpsrate);
    reset ();
}

timer::~timer ()
{
}
	
void timer::force_fps (int fpsrate)
{
    m_fps = fpsrate;
    if (m_fps > 0)
	m_rate = 1000.0 / double (m_fps);
    m_frame_count = 0;
}
	
void timer::reset ()
{
    m_total_frame_count = 0;

    m_delta = 0;
    m_time_count = 0.0;

    m_last_time = 0;
    m_frame_count = 0;
     
    gettimeofday (&m_clock_start, NULL);
}

void timer::update ()
{
    m_total_frame_count ++;

    if (m_fps > 0)
    {
	m_frame_count ++;

	tick_type target_time =
	    m_last_time + m_frame_count * m_rate;

	update_ticks ();
	if (m_time_count < target_time)
	    usleep ((target_time - m_time_count) * 1000);
	else {
	    m_last_time = m_time_count;
	    m_frame_count = 0;
	}
    }
    else
    	update_ticks ();
}

void timer::update_ticks ()
{
    gettimeofday (&m_clock_now, NULL);
    tick_type new_time =
        tick_type (m_clock_now.tv_sec  - m_clock_start.tv_sec) * 1000 +
	tick_type (m_clock_now.tv_usec - m_clock_start.tv_usec) / 1000;
    m_delta = new_time - m_time_count;
    m_time_count = new_time;
}

} /* namespace psynth */
