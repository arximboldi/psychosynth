/**
 *  Time-stamp:  <2010-10-17 20:07:07 raskolnikov>
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

#ifndef PSYNTH_TIMER_H
#define PSYNTH_TIMER_H

#include <ctime>

namespace psynth
{
namespace base
{

/**
 * A class for counting time and forcing update rates.
 */
class timer
{
public:
    typedef int tick_type;
    typedef size_t frame_type;

    /** Constructor. */
    timer ();

    /**
     * Constructor which sets a frames per second rate.
     * @param fpsrate The frames per second rate of the timer.
     * @see forceFps
     */
    timer (int fpsrate);

    /** Destructor. */
    ~timer();

    /**
     * Makes the timer force a frames per second update rate. This means
     * that every call to @c update() will cause a sleep to force a
     * rate of updates per second as near as possible to the desired one.
     * @param fpsrate The desired fpsrate. Set it to -1 if you no longer want
     * to have a fixed fpsrate.
     */
    void force_fps (int fpsrate);

    /**
     * Resets the timer counts.
     */
    void reset ();

    /**
     * Updates the timer.
     */
    void update ();

    /**
     * Returns the milliseconds elapsed between the last call to
     * @c update() and the creation of the Timer or the last @c rest call.
     */
    tick_type ticks ()
    {
	return m_time_count;
    }

    /**
     * Returns the milliseconds elapsed between the last two @c update() calls.
     */
    tick_type delta_ticks ()
    {
	return m_delta;
    }

    /**
     * Returns the number of total updates since start.
     */
    frame_type frames ()
    {
	return m_total_frame_count;
    }

private:
    timeval m_clock_now;
    timeval m_clock_start;

    frame_type m_frame_count;
    frame_type m_total_frame_count;

    tick_type m_time_count;
    tick_type m_delta;
    tick_type m_last_time;

    int    m_fps;
    double m_rate;

    void update_ticks ();
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_TIMER_H */
