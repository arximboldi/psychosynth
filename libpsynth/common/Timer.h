/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef PSYNTH_TIMER_H
#define PSYNTH_TIMER_H

#include <ctime>

namespace psynth
{

/**
 * A class for counting time and forcing update rates.
 */
class Timer
{
    timeval now;
    timeval start;
	
    int nowticks;
    int sinceticks;
    int ms;
    int framecount;
    int rate;
    float rateticks;
	
    void updateTicks();
	
public:
    /** Constructor. */
    Timer();

    /**
     * Constructor which sets a frames per second rate.
     * @param fpsrate The frames per second rate of the timer.
     * @see forceFps
     */
    Timer(int fpsrate);

    /** Destructor. */
    ~Timer();

    /**
     * Makes the timer force a frames per second update rate. This means
     * that every call to @c update() will cause a sleep to force a
     * rate of updates per second as near as possible to the desired one.
     * @param fpsrate The desired fpsrate. Set it to -1 if you no longer want
     * to have a fixed fpsrate.
     */
    void forceFps(int fpsrate);

    /**
     * Resets the timer counts.
     */
    void reset();

    /**
     * Updates the timer.
     */
    void update();	

    /**
     * Returns the milliseconds elapsed between the last call to
     * @c update() and the creation of the Timer or the last @c rest call.
     */
    int ticks() {
	return nowticks;
    };

    /**
     * Retusn the milliseconds elapsed between the last two @c update() calls.
     */
    int deltaticks() {
	return ms;
    };
};

} /* namespace psynth */

#endif /* PSYNTH_TIMER_H */
