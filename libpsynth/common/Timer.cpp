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

#include <unistd.h>
#include <iostream>
#include <sys/time.h>

#include "common/Timer.h"

using namespace std;

Timer::Timer() :
	nowticks(0),
	sinceticks(0),
	ms(0),
	framecount(0),
    rate(-1),
	rateticks(1)
{
	reset();
	updateTicks();
};

Timer::Timer(int fpsrate):
	nowticks(0),
	sinceticks(0),
	ms(0),
	framecount(0),
    rate(fpsrate),
    rateticks(1000.0/fpsrate)
{
	reset();
	updateTicks();
};

Timer::~Timer()
{
}
	
void Timer::forceFps(int fpsrate)
{
	framecount = 0;
	rate = fpsrate;
	rateticks = (1000.0 /rate);
}
	
void Timer::reset()
{
	gettimeofday(&start, NULL);
}

void Timer::update()
{
	int lastticks;
    int targetticks;
    
    lastticks = nowticks;
    
	updateTicks();
	
    if (rate > 0) {
        framecount++;
        
        targetticks = sinceticks + int(framecount * rateticks);
		
        if (nowticks <= targetticks)
            usleep((targetticks - nowticks)*1000);
        else {
            framecount = 0;
            updateTicks();
            sinceticks = nowticks;
        }
    } else {
		updateTicks();
    }
    
    ms = nowticks - lastticks;
}

void Timer::updateTicks()
{
	gettimeofday(&now, NULL);
	nowticks =
        (now.tv_sec - start.tv_sec) * 1000 + (now.tv_usec -
                                              start.tv_usec) / 1000;
}
