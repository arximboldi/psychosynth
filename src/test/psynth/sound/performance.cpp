/**
 *  Time-stamp:  <2010-11-07 14:05:05 raskolnikov>
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
#include <psynth/sound/buffer_view.hpp>
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

// macros for standard GIL views
#define STEREO_VIEW(T) \
    buffer_view<frame<T,stereo_layout>*>

#define RLSTEREO_VIEW(T) \
    buffer_view<frame<T,rlstereo_layout>*>

#define STEREO_PLANAR_VIEW(T) \
    buffer_view<planar_frame_iterator<T*,stereo_space> >


template <typename View>
void fill_view_max (const View& v)
{
    fill_frames (
	v, typename View::value_type (
	    sample_traits<
		typename element_reference_type <
		    typename View::value_type>::type>::max_value ()));
}

template <typename View, typename P>
struct fill_psynth
{
    View _v;
    P _p;

    fill_psynth (const View& v_in, const P& p_in)
	: _v(v_in)
	, _p(p_in) {}

    void operator ()() const
    {
	fill_frames(_v, _p);
    }
};

template <typename View, typename P>
struct fill_nonpsynth;

template <typename T, typename P>
struct fill_nonpsynth<STEREO_VIEW(T), P>
{
    typedef STEREO_VIEW(T) View;
    View _v;
    P _p;

    fill_nonpsynth(const View& v_in, const P& p_in)
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
struct fill_nonpsynth<STEREO_VIEW(T1), frame<T2,rlstereo_layout> >
{
    typedef STEREO_VIEW(T1) View;
    typedef frame<T2,rlstereo_layout> P;
    View _v;
    P _p;

    fill_nonpsynth (const View& v_in, const P& p_in)
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
struct fill_nonpsynth<STEREO_PLANAR_VIEW(T1), frame<T2,stereo_layout> >
{
    typedef STEREO_PLANAR_VIEW(T1) View;
    typedef frame<T2,stereo_layout> P;
    View _v;
    P _p;
    
    fill_nonpsynth (const View& v_in, const P& p_in)
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
struct fill_nonpsynth<STEREO_PLANAR_VIEW(T1), frame<T2,rlstereo_layout> >
{
    typedef STEREO_PLANAR_VIEW(T1) View;
    typedef frame<T2, rlstereo_layout> P;
    View _v;
    P _p;
    
    fill_nonpsynth (const View& v_in, const P& p_in)
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

template <typename View, typename P>
void test_fill (std::size_t trials)
{
    buffer<typename View::value_type, is_planar<View>::value> bufp (buffer_size);
    buffer<typename View::value_type, is_planar<View>::value> bufn (buffer_size);

    fill_view_max (view (bufp));
    fill_view_max (view (bufn));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    fill_psynth<View,P> (view (bufp), P()), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: "<< measure_time (
	    fill_nonpsynth<View,P> (view (bufn), P()), trials));

    BOOST_CHECK (view (bufp) [0] == P());
    BOOST_CHECK (equal_frames (view (bufp), view (bufn)));
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

template <typename View, typename F>
struct for_each_psynth
{
    View _v;
    F _f;

    for_each_psynth (const View& v_in, const F& f_in)
	: _v(v_in), _f(f_in) {}
    void operator () () const
    {
	for_each_frame (_v, _f);
    }
};

template <typename View, typename F> struct for_each_nonpsynth;
template <typename T, typename T2>
struct for_each_nonpsynth<STEREO_VIEW(T), stereo_fr_t<T2> >
{
    typedef STEREO_VIEW(T) View;
    typedef stereo_fr_t<T2> F;
    View _v;
    F _f;

    for_each_nonpsynth (const View& v_in, const F& f_in)
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
struct for_each_nonpsynth<STEREO_PLANAR_VIEW(T1), stereo_fr_t<T2> >
{
    typedef STEREO_PLANAR_VIEW(T1) View;
    typedef stereo_fr_t<T2> F;
    View _v;
    F _f;

    for_each_nonpsynth (const View& v_in, const F& f_in)
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

template <typename View, typename F>
void test_for_each (std::size_t trials)
{
    buffer<typename View::value_type, is_planar<View>::value> bufp (buffer_size);
    buffer<typename View::value_type, is_planar<View>::value> bufn (buffer_size);

    fill_view_max (view (bufp));
    fill_view_max (view (bufn));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    for_each_psynth<View, F> (view (bufp), F ()), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: " << measure_time (
	    for_each_nonpsynth<View, F> (view (bufn), F ()), trials));

    BOOST_CHECK (equal_frames (view (bufp), view (bufn)));
}

// copy
template <typename View1, typename View2>
struct copy_psynth
{
    View1 _v1;
    View2 _v2;
    copy_psynth (const View1& v1_in, const View2& v2_in)
	: _v1(v1_in), _v2(v2_in) {}
    void operator () () const {
	copy_frames (_v1, _v2);
    }
};

template <typename View1, typename View2>
struct copy_nonpsynth;

template <typename T1, typename T2>
struct copy_nonpsynth<STEREO_VIEW(T1),STEREO_VIEW(T2)>
{
    typedef STEREO_VIEW(T1) View1;
    typedef STEREO_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;

    copy_nonpsynth (const View1& v1_in, const View2& v2_in)
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
struct copy_nonpsynth<STEREO_VIEW(T1), RLSTEREO_VIEW(T2)>
{
    typedef STEREO_VIEW(T1) View1;
    typedef RLSTEREO_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;

    copy_nonpsynth (const View1& v1_in, const View2& v2_in)
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
struct copy_nonpsynth<STEREO_PLANAR_VIEW(T1),STEREO_PLANAR_VIEW(T2)>
{
    typedef STEREO_PLANAR_VIEW(T1) View1;
    typedef STEREO_PLANAR_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;

    copy_nonpsynth (const View1& v1_in, const View2& v2_in)
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
struct copy_nonpsynth<STEREO_VIEW(T1),STEREO_PLANAR_VIEW(T2)>
{
    typedef STEREO_VIEW(T1) View1;
    typedef STEREO_PLANAR_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;

    copy_nonpsynth (const View1& v1_in, const View2& v2_in)
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
struct copy_nonpsynth<STEREO_PLANAR_VIEW(T1),STEREO_VIEW(T2)>
{
    typedef STEREO_PLANAR_VIEW(T1) View1;
    typedef STEREO_VIEW(T2) View2;

    View1 _v1;
    View2 _v2;

    copy_nonpsynth (const View1& v1_in, const View2& v2_in)
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

template <typename View1, typename View2>
void test_copy (std::size_t trials)
{
    buffer<typename View1::value_type,is_planar<View1>::value> bufp1 (buffer_size);
    buffer<typename View2::value_type,is_planar<View2>::value> bufp2 (buffer_size);
    buffer<typename View1::value_type,is_planar<View1>::value> bufn1 (buffer_size);
    buffer<typename View2::value_type,is_planar<View2>::value> bufn2 (buffer_size);

    fill_view_max (view (bufp1));
    fill_view_max (view (bufn1));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    copy_psynth<View1,View2> (view(bufp1), view (bufp2)), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: " << measure_time (
	    copy_nonpsynth<View1,View2> (view (bufn1), view (bufn2)), trials));

    BOOST_CHECK (equal_frames (view (bufp1), view (bufp2)));
    BOOST_CHECK (equal_frames (view (bufn1), view (bufn2)));
    BOOST_CHECK (equal_frames (view (bufp1), view (bufn1)));
    BOOST_CHECK (equal_frames (view (bufp2), view (bufn2)));
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

template <typename View1, typename View2, typename F>
struct transform_psynth
{
    View1 _v1;
    View2 _v2;
    F _f;

    transform_psynth(const View1& v1_in, const View2& v2_in, const F& f_in)
	: _v1(v1_in), _v2(v2_in), _f(f_in) {}

    void operator() () const
    {
	transform_frames (_v1, _v2, _f);
    }
};

template <typename View1, typename View2, typename F> struct transform_nonpsynth;
template <typename T1, typename T2, typename F>
struct transform_nonpsynth<STEREO_VIEW(T1), STEREO_VIEW(T2),F>
{
    typedef STEREO_VIEW(T1) View1;
    typedef STEREO_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;
    F _f;

    transform_nonpsynth (const View1& v1_in, const View2& v2_in, const F& f_in)
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
struct transform_nonpsynth<STEREO_PLANAR_VIEW(T1),STEREO_PLANAR_VIEW(T2),F>
{
    typedef STEREO_PLANAR_VIEW(T1) View1;
    typedef STEREO_PLANAR_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;
    F _f;
    
    transform_nonpsynth (const View1& v1_in, const View2& v2_in, const F& f_in)
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
struct transform_nonpsynth<STEREO_VIEW(T1),STEREO_PLANAR_VIEW(T2),F>
{
    typedef STEREO_VIEW(T1) View1;
    typedef STEREO_PLANAR_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;
    F _f;

    transform_nonpsynth (const View1& v1_in,const View2& v2_in,const F& f_in)
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
struct transform_nonpsynth<STEREO_PLANAR_VIEW(T1),STEREO_VIEW(T2),F> {
    typedef STEREO_PLANAR_VIEW(T1) View1;
    typedef STEREO_VIEW(T2) View2;
    View1 _v1;
    View2 _v2;
    F _f;

    transform_nonpsynth(const View1& v1_in,const View2& v2_in,const F& f_in)
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

template <typename View1, typename View2, typename F>
void test_transform (std::size_t trials)
{
    buffer<typename View1::value_type,is_planar<View1>::value> bufp1 (buffer_size);
    buffer<typename View2::value_type,is_planar<View2>::value> bufp2 (buffer_size);
    buffer<typename View1::value_type,is_planar<View1>::value> bufn1 (buffer_size);
    buffer<typename View2::value_type,is_planar<View2>::value> bufn2 (buffer_size);

    fill_view_max (view (bufp1));
    fill_view_max (view (bufp2));
    fill_view_max (view (bufn1));
    fill_view_max (view (bufn2));
    
    BOOST_TEST_MESSAGE (
	"psynth: " << measure_time (
	    transform_psynth<View1, View2, F>(
		view (bufp1), view(bufp2), F()), trials));

    BOOST_TEST_MESSAGE (
	"non-psynth: "<< measure_time (
	    transform_nonpsynth<View1,View2,F>(
		view(bufn1), view(bufn2), F()), trials));

    BOOST_CHECK (equal_frames (view (bufp1), view (bufn1)));
    BOOST_CHECK (equal_frames (view (bufp2), view (bufn2)));
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
    test_fill<stereo8_view, stereo8_frame>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_planar_buffer with stereo8_frame");
    test_fill<stereo8_planar_view, stereo8_frame>(num_trials);

    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_buffer with rlstereo8_frame");
    test_fill<stereo8_view, rlstereo8_frame>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test fill_frames() on stereo8_planar_buffer with rlstereo8_frame");
    test_fill<stereo8_planar_view, rlstereo8_frame>(num_trials);
}

BOOST_AUTO_TEST_CASE (test_for_each_frame_performance)
{
    BOOST_TEST_MESSAGE ("Test for_each_frame() on stereo8_buffer");
    test_for_each<stereo8_view, stereo_fr_t<bits8> >(num_trials);

    BOOST_TEST_MESSAGE ("Test for_each_frame() on stereo8_planar_buffer");
    test_for_each<stereo8_planar_view, stereo_fr_t<bits8> >(num_trials);
}

BOOST_AUTO_TEST_CASE (test_copy_frames_performance)
{
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_buffer and stereo8_buffer");
    test_copy<stereo8_view,stereo8_view>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_buffer and rlstereo8_buffer");
    test_copy<stereo8_view,rlstereo8_view>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_planar_buffer and "
	"stereo8_planar_buffer");
    test_copy<stereo8_planar_view,stereo8_planar_view>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_buffer and stereo8_planar_buffer");
    test_copy<stereo8_view,stereo8_planar_view>(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test copy_frames() between stereo8_planar_buffer and stereo8_buffer");
    test_copy<stereo8_planar_view,stereo8_view>(num_trials);
}

BOOST_AUTO_TEST_CASE (test_transform_frames_performance)
{
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_buffer and stereo8_buffer");
    test_transform<
	stereo8_view,
	stereo8_view,
	rlstereo_to_stereo<bits8,frame<bits8,stereo_layout> > >(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_planar_buffer and "
	"stereo8_planar_buffer");
    test_transform<
	stereo8_planar_view,
	stereo8_planar_view,
	rlstereo_to_stereo<
	    bits8, planar_frame_reference<bits8,stereo_space> > >(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_buffer and "
	"stereo8_planar_buffer");
    test_transform<
	stereo8_view,
	stereo8_planar_view,
	rlstereo_to_stereo<bits8,frame<bits8,stereo_layout> > >(num_trials);
    
    BOOST_TEST_MESSAGE (
	"Test transform_frames() between stereo8_planar_buffer and "
	"stereo8_buffer");
    test_transform<
	stereo8_planar_view,
	stereo8_view,
	rlstereo_to_stereo<
	    bits8,planar_frame_reference<bits8,stereo_space> > >(num_trials);   
}

BOOST_AUTO_TEST_SUITE_END ();
