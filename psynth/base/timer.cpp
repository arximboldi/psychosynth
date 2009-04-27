/**
 *  Time-stamp:  <2009-04-27 16:57:24 raskolnikov>
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
    : nowticks (0)
    , sinceticks (0)
    , ms (0)
    , framecount(0)
    , rate (-1)
    , rateticks (1)
{
    reset ();
    update_ticks ();
};

timer::timer (int fpsrate)
    : nowticks (0)
    , sinceticks (0)
    , ms (0)
    , framecount (0)
    , rate (fpsrate)
    , rateticks (1000.0/fpsrate)
{
    reset ();
    update_ticks ();
};

timer::~timer ()
{
}
	
void timer::force_fps (int fpsrate)
{
    framecount = 0;
    rate = fpsrate;
    rateticks = (1000.0 /rate);
}
	
void timer::reset ()
{
    gettimeofday (&start, NULL);
}

void timer::update ()
{
    int lastticks;
    int targetticks;
    
    lastticks = nowticks;
    
    update_ticks ();
	
    if (rate > 0) {
        framecount++;
        
        targetticks = sinceticks + int(framecount * rateticks);
		
        if (nowticks <= targetticks)
            usleep((targetticks - nowticks)*1000);
        else {
            framecount = 0;
            update_ticks ();
            sinceticks = nowticks;
        }
    } else {
	update_ticks ();
    }
    
    ms = nowticks - lastticks;
}

void timer::update_ticks ()
{
    gettimeofday (&now, NULL);
    nowticks =
        (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec -
                                              start.tv_usec) / 1000;
}

} /* namespace psynth */
