/**
 *  Time-stamp:  <2010-11-09 12:39:39 raskolnikov>
 *
 *  @file        performance.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Nov  5 18:59:31 2010
 *
 *  psynth::sound performance test suite.
 *
 *    fill_pixels() on stereo8_image_t with stereo8_pixel_t
 *    fill_pixels() on stereo8_planar_image_t with stereo8_pixel_t
 *    fill_pixels() on stereo8_image_t with bgr8_pixel_t
 *    fill_pixels() on stereo8_planar_image_t with bgr8_pixel_t
 *    for_each_pixel() on stereo8_image_t
 *    for_each_pixel() on stereo8_planar_t
 *    copy_pixels() between stereo8_image_t and stereo8_image_t
 *    copy_pixels() between stereo8_image_t and bgr8_image_t
 *    copy_pixels() between stereo8_planar_image_t and stereo8_planar_image_t
 *    copy_pixels() between stereo8_image_t and stereo8_planar_image_t
 *    copy_pixels() between stereo8_planar_image_t and stereo8_image_t
 *    transform_pixels() between stereo8_image_t and stereo8_image_t
 *    transform_pixels() between stereo8_planar_image_t and stereo8_planar_image_t
 *    transform_pixels() between stereo8_planar_image_t and stereo8_image_t
 *    transform_pixels() between stereo8_image_t and stereo8_planar_image_t
 *
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
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

/*
 *  Copyright 2005-2007 Adobe Systems Incorporated
 * 
 *  Use, modification and distribution are subject to the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 */

#include <cstddef>
#include <ctime>
#include <iostream>

#include <boost/test/unit_test.hpp>

#include <psynth/sound/frame.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>
#include <psynth/sound/planar_frame_reference.hpp>
#include <psynth/sound/step_iterator.hpp>
#include <psynth/sound/stereo.hpp>
#include <psynth/sound/buffer_range.hpp>
#include <psynth/sound/buffer.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/algorithm.hpp>
#include <psynth/sound/output.hpp>

using namespace psynth::sound;

// returns time in milliseconds per call
template <typename Op>
double measure_time (Op op, std::size_t num_loops)
{
    clock_t begin = clock();
    for (std::size_t ii = 0; ii < num_loops; ++ii)
	op ();
    return double (clock() - begin) / double (num_loops);
}

// buffer dimension
std::size_t buffer_size = 1024;

// macros for standard GIL ranges
#define STEREO_RANGE(T) \
    buffer_range<frame<T,stereo_layout>*>

#define RLSTEREO_RANGE(T) \
    buffer_range<frame<T,rlstereo_layout>*>

#define STEREO_PLANAR_RANGE(T) \
    buffer_range<planar_frame_iterator<T*,stereo_space> >


template <typename Range>
void fill_range_max (const Range& v)
{
    fill_frames (
	v, typename Range::value_type (
	    sample_traits<
		typename element_reference_type <
		    typename Range::value_type>::type>::max_value ()));
}

template <typename Range, typename P>
struct fill_psynth
{
    Range _v;
    P _p;

    fill_psynth (const Range& v_in, const P& p_in)
	: _v(v_in)
	, _p(p_in) {}

    void operator ()() const
    {
	fill_frames(_v, _p);
    }
};

template <typename Range, typename P>
struct fill_nonpsynth;

template <typename T, typename P>
struct fill_nonpsynth<STEREO_RANGE(T), P>
{
    typedef STEREO_RANGE(T) Range;
    Range _v;
    P _p;

    fill_nonpsynth(const Range& v_in, const P& p_in)
	: _v(v_in)
	, _p(p_in) {}

    void operator()() const
    {
        T* first = (T*)_v.begin ();
        T* last  = first + _v.size () * 2;
        while (first != last)
	{
            first[0] = psynth::sound::at_c<0>(_p);
            first[1] = psynth::sound::at_c<1>(_p);
	    first    += 2;
        }
    }
};

template <typename T1, typename T2>
struct fill_nonpsynth<STEREO_RANGE(T1), frame<T2,rlstereo_layout> >
{
    typedef STEREO_RANGE(T1) Range;
    typedef frame<T2,rlstereo_layout> P;
    Range _v;
    P _p;

    fill_nonpsynth (const Range& v_in, const P& p_in)
	: _v(v_in)
	, _p(p_in) {}
    
    void operator()() const
    {
        T1* first = (T1*)_v.begin ();
        T1* last  = first + _v.size () * 2;
        while (first != last)
	{
            first[0] = psynth::sound::at_c<1>(_p);
            first[1] = psynth::sound::at_c<0>(_p);
            first    += 2;
        }
    }
};

template <typename T1, typename T2>
struct fill_nonpsynth<STEREO_PLANAR_RANGE(T1), frame<T2,stereo_layout> >
{
    typedef STEREO_PLANAR_RANGE(T1) Range;
    typedef frame<T2,stereo_layout> P;
    Range _v;
    P _p;
    
    fill_nonpsynth (const Range& v_in, const P& p_in)
	: _v(v_in)
	, _p(p_in) {}

    void operator () () const
    {
        std::size_t size = _v.size();
        T1* first;
        first = (T1*) psynth::sound::at_c<0>(_v.begin ());
        std::fill (first, first + size, psynth::sound::at_c<0>(_p));
        first = (T1*) psynth::sound::at_c<1>(_v.begin ());
        std::fill (first, first + size, psynth::sound::at_c<1>(_p));
    }
};

template <typename T1, typename T2>
struct fill_nonpsynth<STEREO_PLANAR_RANGE(T1), frame<T2,rlstereo_layout> >
{
    typedef STEREO_PLANAR_RANGE(T1) Range;
    typedef frame<T2, rlstereo_layout> P;
    Range _v;
    P _p;
    
    fill_nonpsynth (const Range& v_in, const P& p_in)
	: _v(v_in), _p(p_in) {}

    void operator () () const
    {
        std::size_t size = _v.size();
        T1* first;
        first = (T1*) psynth::sound::at_c<0> (_v.begin ());
        std::fill (first, first + size, psynth::sound::at_c<0> (_p));
        first = (T1*) psynth::sound::at_c<1> (_v.begin ());
        std::fill (first, first + size, psynth::sound::at_c<1> (_p));
    }
};

template <typename Range, typename P>
void test_fill (std::size_t trials)
{
    buffer<typename Range::value_type, is_planar<Range>::value> bufp (buffer_size);
    buffer<typename Range::value_type, is_planar<Range>::value> bufn (buffer_size);

    fill_range_max (range (bufp));
    fill_range_max (range (bufn));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    fill_psynth<Range,P> (range (bufp), P()), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: "<< measure_time (
	    fill_nonpsynth<Range,P> (range (bufn), P()), trials));

    BOOST_CHECK (range (bufp) [0] == P());
    BOOST_CHECK (equal_frames (range (bufp), range (bufn)));
};

template <typename T>
struct stereo_fr_t
{
    void operator () (frame<T,stereo_layout>& p) const
    {
	p[0] = 0;
	p[1] = 1;
    }
    
    void operator () (const planar_frame_reference<T&, stereo_space>& p) const
    {
	p[0] = 0;
	p[1] = 1;
    }
};

template <typename Range, typename F>
struct for_each_psynth
{
    Range _v;
    F _f;

    for_each_psynth (const Range& v_in, const F& f_in)
	: _v(v_in), _f(f_in) {}
    void operator () () const
    {
	for_each_frame (_v, _f);
    }
};

template <typename Range, typename F> struct for_each_nonpsynth;
template <typename T, typename T2>
struct for_each_nonpsynth<STEREO_RANGE(T), stereo_fr_t<T2> >
{
    typedef STEREO_RANGE(T) Range;
    typedef stereo_fr_t<T2> F;
    Range _v;
    F _f;

    for_each_nonpsynth (const Range& v_in, const F& f_in)
	: _v (v_in)
	, _f (f_in) {}

    void operator () () const
    {
        T* first = (T*)_v.begin ();
        T* last  = first + _v.size () * 2;
        while (first != last) {
            first [0] =  0;
            first [1] =  1;
            first     += 2;
        }
    }
};

template <typename T1, typename T2>
struct for_each_nonpsynth<STEREO_PLANAR_RANGE(T1), stereo_fr_t<T2> >
{
    typedef STEREO_PLANAR_RANGE(T1) Range;
    typedef stereo_fr_t<T2> F;
    Range _v;
    F _f;

    for_each_nonpsynth (const Range& v_in, const F& f_in)
	: _v (v_in)
	, _f (f_in) {}

    void operator () () const
    {
        T1 *first0, *first1, *last0;
        first0 = (T1*) psynth::sound::at_c<0> (_v.begin ());
        first1 = (T1*) psynth::sound::at_c<1> (_v.begin ());
        last0  = first0 + _v.size();

	while (first0 != last0)
	{
            *first0++ = 0;
            *first1++ = 1;
        }
    }
};

template <typename Range, typename F>
void test_for_each (std::size_t trials)
{
    buffer<typename Range::value_type, is_planar<Range>::value> bufp (buffer_size);
    buffer<typename Range::value_type, is_planar<Range>::value> bufn (buffer_size);

    fill_range_max (range (bufp));
    fill_range_max (range (bufn));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    for_each_psynth<Range, F> (range (bufp), F ()), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: " << measure_time (
	    for_each_nonpsynth<Range, F> (range (bufn), F ()), trials));

    BOOST_CHECK (equal_frames (range (bufp), range (bufn)));
}

// copy
template <typename Range1, typename Range2>
struct copy_psynth
{
    Range1 _v1;
    Range2 _v2;
    copy_psynth (const Range1& v1_in, const Range2& v2_in)
	: _v1(v1_in), _v2(v2_in) {}
    void operator () () const {
	copy_frames (_v1, _v2);
    }
};

template <typename Range1, typename Range2>
struct copy_nonpsynth;

template <typename T1, typename T2>
struct copy_nonpsynth<STEREO_RANGE(T1),STEREO_RANGE(T2)>
{
    typedef STEREO_RANGE(T1) Range1;
    typedef STEREO_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;

    copy_nonpsynth (const Range1& v1_in, const Range2& v2_in)
	: _v1 (v1_in)
	, _v2 (v2_in) {}

    void operator () () const
    {
        T1* first1 = (T1*) _v1.begin ();
        T1* last1  = first1 + _v1.size () * 2;
        T2* first2 = (T2*) _v2.begin ();
        std::copy (first1, last1, first2);
    }
};

template <typename T1, typename T2>
struct copy_nonpsynth<STEREO_RANGE(T1), RLSTEREO_RANGE(T2)>
{
    typedef STEREO_RANGE(T1) Range1;
    typedef RLSTEREO_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;

    copy_nonpsynth (const Range1& v1_in, const Range2& v2_in)
	: _v1 (v1_in)
	, _v2 (v2_in) {}
    
    void operator() () const
    {
        T1* first1 = (T1*)_v1.begin ();
        T1* last1  = first1 + _v1.size() * 2;
        T2* first2 = (T2*)_v2.begin ();
	
        while (first1!=last1)
	{
            first2 [1] = first1 [0];
            first2 [0] = first1 [1];
            first1 += 2; first2 += 2;
        }
    }
};
template <typename T1, typename T2>
struct copy_nonpsynth<STEREO_PLANAR_RANGE(T1),STEREO_PLANAR_RANGE(T2)>
{
    typedef STEREO_PLANAR_RANGE(T1) Range1;
    typedef STEREO_PLANAR_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;

    copy_nonpsynth (const Range1& v1_in, const Range2& v2_in)
	: _v1 (v1_in)
	, _v2(v2_in) {}

    void operator()() const
    {
        std::size_t size=_v1.size();
        T1* first10 = (T1*) psynth::sound::at_c<0>(_v1.begin ());
        T1* first11 = (T1*) psynth::sound::at_c<1>(_v1.begin ());
        
	T2* first20 = (T2*) psynth::sound::at_c<0>(_v2.begin ());
        T2* first21 = (T2*) psynth::sound::at_c<1>(_v2.begin ());

        std::copy (first10,first10+size,first20);
        std::copy (first11,first11+size,first21);
    }
};

template <typename T1, typename T2>
struct copy_nonpsynth<STEREO_RANGE(T1),STEREO_PLANAR_RANGE(T2)>
{
    typedef STEREO_RANGE(T1) Range1;
    typedef STEREO_PLANAR_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;

    copy_nonpsynth (const Range1& v1_in, const Range2& v2_in)
	: _v1(v1_in), _v2(v2_in) {}

    void operator()() const
    {
        T1* first  = (T1*) _v1.begin ();
        T1* last   = first + _v1.size() * 2;
        T2* first0 = (T2*) psynth::sound::at_c<0> (_v2.begin ());
        T2* first1 = (T2*) psynth::sound::at_c<1> (_v2.begin ());
	
	while (first != last)
	{
            *first0++ = first[0];
            *first1++ = first[1];
            first     += 2;
        }
    }
};

template <typename T1, typename T2>
struct copy_nonpsynth<STEREO_PLANAR_RANGE(T1),STEREO_RANGE(T2)>
{
    typedef STEREO_PLANAR_RANGE(T1) Range1;
    typedef STEREO_RANGE(T2) Range2;

    Range1 _v1;
    Range2 _v2;

    copy_nonpsynth (const Range1& v1_in, const Range2& v2_in)
	: _v1(v1_in), _v2(v2_in) {}

    void operator()() const
    {
        T1* first  = (T1*) _v2.begin ();
        T1* last   = first + _v2.size () * 2;
        T2* first0 = (T2*) psynth::sound::at_c<0> (_v1.begin ());
        T2* first1 = (T2*) psynth::sound::at_c<1> (_v1.begin ());
        while (first != last)
	{
            first [0] = *first0++;
            first [1] = *first1++;
            first     += 2;
        }
    }
};

template <typename Range1, typename Range2>
void test_copy (std::size_t trials)
{
    buffer<typename Range1::value_type,is_planar<Range1>::value> bufp1 (buffer_size);
    buffer<typename Range2::value_type,is_planar<Range2>::value> bufp2 (buffer_size);
    buffer<typename Range1::value_type,is_planar<Range1>::value> bufn1 (buffer_size);
    buffer<typename Range2::value_type,is_planar<Range2>::value> bufn2 (buffer_size);

    fill_range_max (range (bufp1));
    fill_range_max (range (bufn1));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    copy_psynth<Range1,Range2> (range(bufp1), range (bufp2)), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: " << measure_time (
	    copy_nonpsynth<Range1,Range2> (range (bufn1), range (bufn2)), trials));

    BOOST_CHECK (equal_frames (range (bufp1), range (bufp2)));
    BOOST_CHECK (equal_frames (range (bufn1), range (bufn2)));
    BOOST_CHECK (equal_frames (range (bufp1), range (bufn1)));
    BOOST_CHECK (equal_frames (range (bufp2), range (bufn2)));
}

// transform()
template <typename T,typename Frame>
struct rlstereo_to_stereo
{
    frame<T,stereo_layout> operator() (const Frame& p) const
    {
        return frame<T,stereo_layout> (
	    T (get_channel (p, right_channel ())  * 0.1f),
	    T (get_channel (p, left_channel ()) * 0.2f));
    }
};

template <typename Range1, typename Range2, typename F>
struct transform_psynth
{
    Range1 _v1;
    Range2 _v2;
    F _f;

    transform_psynth(const Range1& v1_in, const Range2& v2_in, const F& f_in)
	: _v1(v1_in), _v2(v2_in), _f(f_in) {}

    void operator() () const
    {
	transform_frames (_v1, _v2, _f);
    }
};

template <typename Range1, typename Range2, typename F> struct transform_nonpsynth;
template <typename T1, typename T2, typename F>
struct transform_nonpsynth<STEREO_RANGE(T1), STEREO_RANGE(T2),F>
{
    typedef STEREO_RANGE(T1) Range1;
    typedef STEREO_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;
    F _f;

    transform_nonpsynth (const Range1& v1_in, const Range2& v2_in, const F& f_in)
	: _v1(v1_in), _v2(v2_in), _f(f_in) {}

    void operator ()() const
    {
        T1* first1 = (T1*)_v1.begin ();
        T2* first2 = (T1*)_v2.begin ();
        T1* last1  = first1 + _v1.size () * 2;
        while(first1!=last1)
	{
            first2[0] = T2 (first1[1] * 0.1f);
            first2[1] = T2 (first1[0] * 0.2f);
            first1 += 2; first2 += 2;
        }
    }
};

template <typename T1, typename T2, typename F>
struct transform_nonpsynth<STEREO_PLANAR_RANGE(T1),STEREO_PLANAR_RANGE(T2),F>
{
    typedef STEREO_PLANAR_RANGE(T1) Range1;
    typedef STEREO_PLANAR_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;
    F _f;
    
    transform_nonpsynth (const Range1& v1_in, const Range2& v2_in, const F& f_in)
	: _v1(v1_in), _v2(v2_in), _f(f_in) {}

    void operator()() const
    {
        T1* first10 = (T1*) psynth::sound::at_c<0>(_v1.begin ());
        T1* first11 = (T1*) psynth::sound::at_c<1>(_v1.begin ());
        T1* first20 = (T2*) psynth::sound::at_c<0>(_v2.begin ());
        T1* first21 = (T2*) psynth::sound::at_c<1>(_v2.begin ());
        T1* last10  = first10 + _v1.size();
        while (first10 != last10)
	{
            *first20++ = T2 (*first11++ * 0.1f);
            *first21++ = T2 (*first10++ * 0.2f);
        }
    }
};

template <typename T1, typename T2, typename F>
struct transform_nonpsynth<STEREO_RANGE(T1),STEREO_PLANAR_RANGE(T2),F>
{
    typedef STEREO_RANGE(T1) Range1;
    typedef STEREO_PLANAR_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;
    F _f;

    transform_nonpsynth (const Range1& v1_in,const Range2& v2_in,const F& f_in)
	: _v1 (v1_in), _v2(v2_in), _f(f_in) {}

    void operator()() const
    {
        T1* first1  = (T1*) _v1.begin ();
        T1* last1   = first1 + _v1.size() * 2;
        T1* first20 = (T2*) psynth::sound::at_c<0> (_v2.begin ());
        T1* first21 = (T2*) psynth::sound::at_c<1> (_v2.begin ());
	while (first1 != last1)
	{
            *first20++ = T2 (first1[1] * 0.1f);
            *first21++ = T2 (first1[0] * 0.2f);
            first1 += 2;
        }
    }
};

template <typename T1, typename T2, typename F>
struct transform_nonpsynth<STEREO_PLANAR_RANGE(T1),STEREO_RANGE(T2),F> {
    typedef STEREO_PLANAR_RANGE(T1) Range1;
    typedef STEREO_RANGE(T2) Range2;
    Range1 _v1;
    Range2 _v2;
    F _f;

    transform_nonpsynth(const Range1& v1_in,const Range2& v2_in,const F& f_in)
	: _v1(v1_in),_v2(v2_in),_f(f_in) {}

    void operator()() const
    {
        T1* first10 = (T1*)psynth::sound::at_c<0> (_v1.begin ());
        T1* first11 = (T1*)psynth::sound::at_c<1> (_v1.begin ());
        T2* first2  = (T1*)_v2.begin ();
        T1* last2   = first2 + _v1.size() * 2;
        while (first2 != last2)
	{
            first2[0] = T2 (*first11++ * 0.1f);
            first2[1] = T2 (*first10++ * 0.2f);
	    first2 += 2;
        }
    }
};

template <typename Range1, typename Range2, typename F>
void test_transform (std::size_t trials)
{
    buffer<typename Range1::value_type,is_planar<Range1>::value> bufp1 (buffer_size);
    buffer<typename Range2::value_type,is_planar<Range2>::value> bufp2 (buffer_size);
    buffer<typename Range1::value_type,is_planar<Range1>::value> bufn1 (buffer_size);
    buffer<typename Range2::value_type,is_planar<Range2>::value> bufn2 (buffer_size);

    fill_range_max (range (bufp1));
    fill_range_max (range (bufp2));
    fill_range_max (range (bufn1));
    fill_range_max (range (bufn2));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    transform_psynth<Range1, Range2, F>(
		bufp1, bufp2, F()), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: "<< measure_time (
	    transform_nonpsynth<Range1,Range2,F>(
		range(bufn1), range(bufn2), F()), trials));

    BOOST_CHECK (equal_frames (range (bufp1), range (bufn1)));
    BOOST_CHECK (equal_frames (range (bufp2), range (bufn2)));
}

#ifdef NDEBUG
    const std::size_t num_trials = 10000;
#else
    const std::size_t num_trials = 100;
#endif

BOOST_AUTO_TEST_SUITE (sound_peformance_test_suite);

BOOST_AUTO_TEST_CASE (test_fill_frames_performance)
{
    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_buffer with stereo8_frame");
    test_fill<stereo8_range, stereo8_frame>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_planar_buffer with stereo8_frame");
    test_fill<stereo8_planar_range, stereo8_frame>(num_trials);

    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_buffer with rlstereo8_frame");
    test_fill<stereo8_range, rlstereo8_frame>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_planar_buffer with rlstereo8_frame");
    test_fill<stereo8_planar_range, rlstereo8_frame>(num_trials);
}

BOOST_AUTO_TEST_CASE (test_for_each_frame_performance)
{
    BOOST_TEST_MESSAGE ("Test for_each_frame() on stereo8_buffer");
    test_for_each<stereo8_range, stereo_fr_t<bits8> >(num_trials);

    BOOST_TEST_MESSAGE ("Test for_each_frame() on stereo8_planar_buffer");
    test_for_each<stereo8_planar_range, stereo_fr_t<bits8> >(num_trials);
}

BOOST_AUTO_TEST_CASE (test_copy_frames_performance)
{
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_buffer and stereo8_buffer");
    test_copy<stereo8_range,stereo8_range>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_buffer and rlstereo8_buffer");
    test_copy<stereo8_range,rlstereo8_range>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_planar_buffer and "
	"stereo8_planar_buffer");
    test_copy<stereo8_planar_range,stereo8_planar_range>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_buffer and stereo8_planar_buffer");
    test_copy<stereo8_range,stereo8_planar_range>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_planar_buffer and stereo8_buffer");
    test_copy<stereo8_planar_range,stereo8_range>(num_trials);
}

BOOST_AUTO_TEST_CASE (test_transform_frames_performance)
{
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_buffer and stereo8_buffer");
    test_transform<
	stereo8_range,
	stereo8_range,
	rlstereo_to_stereo<bits8,frame<bits8,stereo_layout> > >(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_planar_buffer and "
	"stereo8_planar_buffer");
    test_transform<
	stereo8_planar_range,
	stereo8_planar_range,
	rlstereo_to_stereo<
	    bits8, planar_frame_reference<bits8,stereo_space> > >(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_buffer and "
	"stereo8_planar_buffer");
    test_transform<
	stereo8_range,
	stereo8_planar_range,
	rlstereo_to_stereo<bits8,frame<bits8,stereo_layout> > >(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_planar_buffer and "
	"stereo8_buffer");
    test_transform<
	stereo8_planar_range,
	stereo8_range,
	rlstereo_to_stereo<
	    bits8,planar_frame_reference<bits8,stereo_space> > >(num_trials);   
}

BOOST_AUTO_TEST_SUITE_END ();
