/**
 *  Time-stamp:  <2010-10-17 20:09:36 raskolnikov>
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

#ifndef PSYNTH_MISC_H
#define PSYNTH_MISC_H

#include <string>

namespace psynth
{
namespace base
{

/**
 * Returns the fractional part of a number.
 * @param x The number to get its floating point part.
 */
inline float phase (float x)
{
    return x - (x >= 0.0f ?
		static_cast<int>(x) :
		static_cast<int>(x) - 1);
}

/**
 * Returns the linear interpolation between to values.
 * @param val The center value to interpolate.
 * @param next The next value to interpolate.
 * @param delta The percentage between the two values.
 */
template <class T>
inline T linear_interp (T val, T next, T delta)
{
    return val + delta * (next - val); 
}

/**
 * Returns a the string representation of an integer. The returned value
 * must not be freed by the user because it is statically defined in the
 * function and so new calls invalidates old returned value and it is not
 * thread safe.
 * @param val The value to represent as a string.
 * @param base The numerical base in which we want to represent the number.
 */
char* itoa (int val, int base);

/**
 * Returns a the string representation of a floating point. The returned
 * value must not be freed by the user because it is statically defined in the
 * function and so new calls invalidates old returned value and it is not
 * thread safe.
 * @param f The value to convert to string.
 * @param sigfigs The number of significant 
 */
char* ftoa (double f, double sigfigs);

/**
 * Compares two strings without being case sensible.
 * @return @c -1 if @a a is less than @b b. @c 1 If @a a is greater than @c b.
 * And @c 0 if @a a equals @a b.
 */
int strcmp_i (const char* a, const char* b);

/**
 * String implementation of @c dirname. Returns everything that is before the
 * last dash in the string.
 * @param str The string where to extract the dirname.
 */
std::string dirname_str (std::string str);

/**
 * Returns a pointer to the begining of the extension part of a file name
 * string, defined as the first character after the last dot in the string
 * or the begining of the string if no dot is in it.
 */
const char* get_extension (const char* file);

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_MISC_H */
