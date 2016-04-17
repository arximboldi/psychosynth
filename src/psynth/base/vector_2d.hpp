/**
 *  Time-stamp:  <2010-10-17 20:18:35 raskolnikov>
 *
 *  @file        vector_2d.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  2d space vector implementation.
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

#ifndef PSYNTH_VECTOR2D_H
#define PSYNTH_VECTOR2D_H

#include <iostream>

namespace psynth
{
namespace base
{

/**
 * A bidimensional vector type that can hold any scalar type.
 *
 * @todo Add more vector operations and calculations.
 */
template<class T>
class vector_2d
{
    static const int NUM_D = 2;
public:
    T x;   /**< X axis component of the vector. */
    T y;   /**< Y axis component of the vector. */

    /**
     * Constructor which sets the components of the vector.
     * @param _x The X axis component of the vector.
     * @param _x The Y axis component of the vector.
     */
    vector_2d(T _x = 0, T _y = 0) :
	x(_x), y(_y)
	{}

    /**
     * Sets the components of the vector.
     * @param _x The X axis component of the vector.
     * @param _x The Y axis component of the vector.
     */
    void set(T _x, T _y) {
	x = _x;
	y = _y;
    }

    /**
     * Accesses the components of the vector by index. The X axis has index 0
     * and the the Y axis has index 1.
     */
    T& operator[](size_t index) {
	return *(&x + index);
    }

    /**
     * Accesses the components of the vector by index. The X axis has index 0
     * and the the Y axis has index 1. Const version.
     */
    const T& operator[](size_t index) const {
	return *(&x + index);
    }

    /**
     * Returns the distance between this vector and another one.
     * @param v The other vector.
     */
    T distance(const vector_2d& v) const {
	return sqrt (sqr_distance (v));
    }

    /**
     * Returns the squared distance between this vector and another one.
     * For better efficiency, use this version of the function if you want the
     * distance only for comparison and do not mind about its real value.
     * @param v The other vector.
     */
    T sqr_distance(const vector_2d& v) const {
	return
	    (v.x-x) * (v.x-x) +
	    (v.y-y) * (v.y-y);
    }

    /**
     * Returns the length of the vector.
     */
    T length() const {
	return sqrt(x*x + y*y);
    }

    /**
     * Returns the length of the vector.
     * For better efficiency, use this version of the function if you want the
     * length only for comparison and do not mind about its real value.
     */
    T sqr_length() const {
	return x*x + y*y;
    }
};

/**
 * Output stream operator for the vector class. Relies on the output stream
 * operator of the @a T type stored in the vector.
 */
template<class T>
std::ostream& operator<< (std::ostream& os,
			  const psynth::base::vector_2d<T> v)
{
    os << v.x << " " << v.y;
    return os;
}

/**
 * A bidimensional vector of floats.
 */
typedef vector_2d<float> vector_2f;

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_VECTOR2D */
