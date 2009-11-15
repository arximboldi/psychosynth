/**
 *  Time-stamp:  <2009-11-15 16:01:22 raskolnikov>
 *
 *  @file        misc.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Miscellaneos functions. Interface.
 *  This file is part of the Psychosynth project.
 */

/*
 *  Copyright (C) 2007, 2009 Juan Pedro Bolívar Puente
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

#include <cstdio>
#include <cstring>
#include <cmath>
#include <cctype>

#include "base/misc.hpp"

using namespace std;

namespace psynth
{

const char* get_extension (const char* file)
{
    const char* ext;
    
    for (ext = file; *ext != '\0'; ext++);
    while(--ext >= file && *ext != '.');

    return ext+1;
}

int strcmp_i (const char* a, const char* b)
{
    while (*a && *b && tolower(*a++) == tolower(*b++));

    if (*a == *b)
	return 0;
    else if (*a < *b)
	return -1;
    else
	return 1;
}

char* itoa (int val, int base)
{	
    static char buf[32] = {0};
    int i = 30;
    bool sign = val < 0;

    if (val == 0) {
	buf[30] = '0';
	return &buf[30];
    }
	
    if (sign) val = -val;
    
    for(; val && i ; --i, val /= base)
	buf[i] = "0123456789abcdef"[val % base];

    if (sign) buf[i--] = '-';

    return &buf[i+1];
}

char* ftoa (double f, double sigfigs)
{
    static char a[81];
    int prec, width, front;

    front = (f==0)? 1 : (int)log10(fabs(f))+1;

    if (sigfigs < 1.0 && sigfigs >= 0.0) {
	/* fit number to tolerance */
        double rem = fabs(f) - int(f);
        prec=0;
        int num = (int)rem;
        while (rem * pow(10.0, prec) - num > sigfigs)
            num = int(rem * pow(10.0, ++prec));
        width = front;
        sprintf(a, "%#*.*f", width, prec, f );
    } else {
        if ( sigfigs < 2.0 ) sigfigs = 2.0;
            
        if ( front > (int)sigfigs )
            sprintf( a, "%#.*e", (int)sigfigs-1, f );
        else {
            prec = (int)sigfigs - front;
            if ( f==0.0 ) width = 2;
            else width = front + prec + 1;
            sprintf( a, "%#*.*f", width, prec, f );
	}
    }
    
    return strdup(a);
}

string dirname_str(string str)
{
    string::iterator end = str.end();
    string::iterator beg;
    
    for (beg = --str.end(); beg != --str.begin() && *beg != '/'; --beg);
    str.erase(++beg, end);   
    return str;
}

} /* namespace psynth */
