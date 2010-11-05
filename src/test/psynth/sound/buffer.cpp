/**
 *  Time-stamp:  <2010-11-05 14:59:49 raskolnikov>
 *
 *  @file        buffer.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 12:54:33 2010
 *
 *  Unit tests for the buffers.
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

#ifdef _MSC_VER
//#pragma warning(disable : 4244)
// conversion from 'gil::buffer<V,Alloc>::coord_t' to 'int', possible
// loss of data (visual studio compiler doesn't realize that the two
// types are the same)
#pragma warning(disable : 4503)
// decorated name length exceeded, name was truncated
#endif

#include <string>
#include <vector>
#include <ios>
#include <iostream>
#include <fstream>
#include <map>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/print.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/crc.hpp>
#include <boost/test/unit_test.hpp>

#include <psynth/sound/frame.hpp>
#include <psynth/sound/packed_frame.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/channel_convert.hpp>

#include <psynth/sound/frame_iterator.hpp>
#include <psynth/sound/bit_aligned_frame_iterator.hpp>
#include <psynth/sound/bit_aligned_frame_reference.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>
#include <psynth/sound/planar_frame_reference.hpp>
#include <psynth/sound/step_iterator.hpp>

#include <psynth/sound/dynamic_algorithm.hpp>
#include <psynth/sound/dynamic_buffer.hpp>
#include <psynth/sound/dynamic_buffer_view.hpp>
#include <psynth/sound/dynamic_buffer_view_factory.hpp>

#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/output.hpp>

using namespace psynth::sound;
using namespace std;
using namespace boost;

extern stereo32sf_planar_view sample_view;

// When PSYNTH_SOUND_GENERATE_REFERENCE_DATA is defined, the reference
// data is generated and saved.  When it is undefined, regression
// tests are checked against it #define
// PSYNTH_SOUND_GENERATE_REFERENCE_DATA

/*
 *
 *      Some algorithms to use in testing
 *
 */

template <typename MonoView, typename R>
void mono_buffer_hist (const MonoView& buf_view, R& hist)
{
    // TODO: Does this function make sense?
    // for_each_frame (buf_view, ++ lambda::var (hist) [lambda::_1]);
    for (auto it = buf_view.begin(); it != buf_view.end(); ++it)
        ++ hist [*it];
}

template <typename V, typename R>
void get_hist (const V& buf_view, R& hist)
{
    mono_buffer_hist (channel_converted_view<mono8_frame> (buf_view), hist);
}

// testing custom channel conversion
template <typename C1, typename C2>
struct my_channel_converter_impl :
    public default_channel_converter_impl<C1, C2> {};

template <typename C1>
struct my_channel_converter_impl<C1, mono_space>
{
    template <typename P1, typename P2>
    void operator () (const P1& src, P2& dst) const
    {
        default_channel_converter_impl<C1, mono_space> () (src, dst);
        get_channel (dst, mono_channel ()) = sample_invert (
	    get_channel (dst, mono_channel ()));
    }
};

struct my_channel_converter
{
    template <typename SrcP,typename DstP>
    void operator () (const SrcP& src,DstP& dst) const
    { 
        typedef typename channel_space_type<SrcP>::type src_cs_t;
        typedef typename channel_space_type<DstP>::type dst_cs_t;
        my_channel_converter_impl<src_cs_t, dst_cs_t>()(src, dst);
    }
};

/*
 *
 *   NOTE: Remove mandelbrot generator. 
 *
 */

template <typename T>
void gradient (const T& src, const mono8s_view& dst)
{
    typename T::iterator src_it = src.begin ();
    mono8s_view::iterator dst_it = dst.begin ();

    for (int x = 1; x < src.size () - 1; ++x)
	dst_it [x] = (src_it [x+1] - src_it [x-1]) / 2;
}

// A quick test whether a view is homogeneous
template <typename Frame>
struct frame_is_homogeneous : public mpl::true_ {};

template <typename P, typename C, typename L>
struct frame_is_homogeneous<packed_frame<P,C,L> > : public mpl::false_ {};

template <typename View>
struct view_is_homogeneous :
    public frame_is_homogeneous<typename View::value_type> {};

/*
 *
 *      Tests buffer view transformations and algorithms
 *
 */
class buffer_test
{
public:
    virtual void initialize () {}
    virtual void finalize () {}
    virtual ~buffer_test () {}

    void run();
    
protected:
    virtual void check_view_impl (const stereo32sfc_view& view,
				  const string name) = 0;
    
    template <typename View>
    void check_view (const View& buf_view, const string name)
    {
        stereo32sf_buffer stereo_buf (buf_view.size ());
        copy_and_convert_frames (buf_view, view (stereo_buf));
        check_view_impl (const_view (stereo_buf), name);
    }

private:
    template <typename Buf>
    void basic_test (const string prefix);

    template <typename View>
    void view_transformations_test (const View& buf_view, const string prefix);

    template <typename View>
    void homogeneous_view_transformations_test (
	const View& buf_view, const string prefix, mpl::true_);

    template <typename View>
    void homogeneous_view_transformations_test (
	const View& buf_view, const string prefix, mpl::false_) {}

    template <typename View>
    void histogram_test (const View& buf_view, const string prefix);

    void virtual_view_test ();
    void packed_buffer_test ();
    void dynamic_buffer_test ();

    template <typename Buf>
    void buffer_all_test (const string prefix);
};


// testing buffer iterators, clone, fill, locators, channel convert
template <typename Buf>
void buffer_test::basic_test (const string prefix)
{
    typedef typename Buf::view View;

    Buf buf (20);
    const View& buf_view = view (buf);

    // fill it with "left on"
    stereo32sf_frame left32 (1.0f, 0), right32 (0, -1);
    
    typename View::value_type left, right, zero (0);
    channel_convert (left32, left);
    default_channel_converter () (left32,left);
    left = channel_convert_deref_fn<
	stereo32sf_ref, typename Buf::view::value_type> () (left32);

    channel_convert (right32, right);
    
    fill (buf_view.begin (), buf_view.end (), left);
    
    channel_convert (left32, buf_view [0]);

    // pointer to frame in the middle
    typename View::reference rt   = buf_view.at (0) [buf_view.size () >> 1];
    typename View::iterator  ptr  = &rt;
    typename View::reference rt2  = *(buf_view.at (0) + (buf_view.size () >> 1));
    typename View::iterator  ptr2 = &rt2;
    BOOST_CHECK_EQUAL (ptr, ptr2);
    BOOST_CHECK_EQUAL (buf_view.at (0) + 10, 10 + buf_view.at (0));
    
    // draw 'right' dots with step of 3
    auto it = buf_view.at (buf_view.size () - 1);
    while (it >= buf_view.at (0)) {
        *it  = right;
        it  -= 3;
    }

    // Clone and zero every "left" frame white
    Buf buf_zero (buf);
    for (typename View::iterator it = view (buf_zero).end ();
	 (it - 1) != view (buf_zero).begin (); --it)
    {
        if (*(it - 1) == left)
            *(it - 1) = zero;
    }

    check_view (buf_view, prefix + "left_x");
    check_view (view (buf_zero), prefix + "zero_x");
}

template <typename View>
void buffer_test::histogram_test (const View& buf_view, const string prefix)
{
    //  vector<int> histogram (255,0);
    //  get_hist (cropped, histogram.begin ());
    unsigned char histogram [256];
    fill (histogram, histogram + 256, 0);
    
    get_hist (buf_view, histogram);
    mono8c_view hist_view =
	interleaved_view (256, (const mono8_frame*) histogram);
    check_view (hist_view, prefix + "histogram");
}


template <typename View>
void buffer_test::view_transformations_test (const View& buf_view,
					     const string prefix)
{
    check_view (buf_view, prefix + "original");

    check_view (sub_buffer_view (
		    buf_view, buf_view.size () / 4, buf_view.size () / 2),
	prefix + "cropped");
    check_view(channel_converted_view<mono8_frame>(buf_view), prefix + "mono8");
    check_view(channel_converted_view<mono8_frame>(
		   buf_view, my_channel_converter ()), prefix + "my_mono8");
    check_view (flipped_view (buf_view), prefix + "flipped_ud");
    check_view (sub_sampled_view (buf_view, 2), prefix + "subsampled");   
    check_view (kth_sample_view<0>(buf_view), prefix + "0th_k_sample");
    homogeneous_view_transformations_test (
	buf_view, prefix, view_is_homogeneous<View>());
}

template <typename View>
void buffer_test::homogeneous_view_transformations_test (
    const View& buf_view, const string prefix, mpl::true_)
{
    check_view (nth_sample_view (buf_view, 0), prefix + "0th_n_sample");
}

void buffer_test::virtual_view_test ()
{
    // TODO: Design a new test that does not require fucking
    // mandelbrot :D
    
#if 0
    typedef mandelbrot_fn<stereo32sf_frame_t> deref_t;
    typedef deref_t::point_t            point_t;
    typedef virtual_2d_locator<deref_t,false> locator_t;
    typedef buffer_view<locator_t> my_virt_view;

    boost::function_requires<FrameLocatorConcept<locator_t> >();
    gil_function_requires<StepIteratorConcept<locator_t::x_iterator> >();

    point_t dims(200,200);
    my_virt_view mandel(
	dims, locator_t(point_t(0,0), point_t(1,1),
			deref_t(dims, stereo32sf_frame_t(255,0,255),
				stereo32sf_frame_t(0,255,0))));

    mono8s_buffer buf(dims);
    fill_frames(view(buf),0);
    // our x_gradient algorithm doesn't change the first & last
    // column, so make sure they are 0
    x_gradient(channel_converted_view<mono8_frame_t>(mandel), view(buf));
    check_view(channel_converted_view<mono8_frame_t>(
		   const_view(buf)), "mandelLuminosityGradient");

    view_transformations_test(mandel,"virtual_");
    histogram_test(mandel,"virtual_");
#endif
}

// Test alignment and packed buffers
void buffer_test::packed_buffer_test ()
{
    typedef bit_aligned_buffer2_type<1,3, stereo_layout>::type stereo13_buffer;
    typedef stereo13_buffer::value_type stereo13_frame;
        
    stereo13_buffer stereo13_buf (10);

    stereo13_frame fill_val (1, 3);
    fill_frames (view (stereo13_buf), fill_val);
    
    stereo13_buffer stereo13a_buf (10, 1);
    copy_frames(const_view(stereo13_buf), view(stereo13a_buf));
    
    stereo13_buffer stereo13b_buf (10, 4);
    copy_frames(const_view (stereo13_buf), view(stereo13b_buf));
    
    BOOST_CHECK (stereo13_buf == stereo13a_buf);
    BOOST_CHECK (stereo13a_buf == stereo13b_buf);
}

void buffer_test::dynamic_buffer_test ()
{
    typedef dynamic_buffer<
	mpl::vector<
	    mono8_buffer,
	    mono32f_buffer,
	    stereo32sf_buffer,
	    stereo16_buffer,
	    stereo32sf_planar_buffer> >
	test_buffer;
    
    stereo32sf_planar_buffer buf (sample_view.size ());
    copy_frames (sample_view, view (buf));
    
    test_buffer any_buf = test_buffer (buf);
    
    check_view (view (any_buf), "dynamic_");
    check_view (flipped_view (view (any_buf)), "dynamic_fliplr");
        
    test_buffer::view sub_buffer = sub_buffer_view (
	view (any_buf), 128, 256);
    
    check_view (sub_buffer, "dynamic_subbuffer");
    check_view (sub_sampled_view (view (any_buf), 2),
		"dynamic_subbuffer_subsampled180rot");
}

template <typename Buf>
void buffer_test::buffer_all_test (const string prefix)
{
    basic_test<Buf> (prefix + "basic_");
    
    Buf buf;
    buf.recreate (sample_view.size ());
    copy_and_convert_frames (sample_view, view (buf));
    
    view_transformations_test (view (buf), prefix + "views_");
    histogram_test (const_view (buf),prefix + "histogram_");
}

void buffer_test::run ()
{
    initialize ();
    
    buffer_all_test<rlstereo8_buffer> ("bgr8_");
    buffer_all_test<stereo32sf_buffer> ("stereo32sf_");
    buffer_all_test<stereo32sf_planar_buffer> ("planarstereo32sf_");
    buffer_all_test<mono8_buffer> ("gray8_");
    
    typedef const bit_aligned_frame_reference <
	boost::uint8_t,
	mpl::vector2_c<int,1,2>,
	rlstereo_layout,
	true>
    rlstereo12_ref;

    typedef buffer<rlstereo12_ref, false> rlstereo12_buffer;
    buffer_all_test<rlstereo12_buffer> ("bgr12_");

    // TODO: Remove?
    view_transformations_test (sub_sampled_view (sample_view, 2), "subsampled_");
    view_transformations_test (channel_converted_view<mono8_frame>(sample_view),
			       "channel_converted_");

    virtual_view_test ();
    packed_buffer_test ();
    dynamic_buffer_test ();

    finalize ();
}


/*
 *
 *  Performs or generates buffer tests using checksums
 *
 */

class checksum_buffer_mgr : public buffer_test
{
protected:
    typedef map<string,boost::crc_32_type::value_type> crc_map_t;
    crc_map_t _crc_map;
};


/*
 *
 *  Performs buffer tests by comparing buffer frame checksums against a reference
 *
 */

class checksum_buffer_test : public checksum_buffer_mgr
{
public:
    checksum_buffer_test (const char* filename)
	: _filename(filename) {}
    
private:
    const char* _filename;
    virtual void initialize();
    virtual void check_view_impl (const stereo32sf_view& v, const string name);
};

// Load the checksums from the reference file and create the start
// buffer
void checksum_buffer_test::initialize ()
{
    string crc_name; 
    boost::crc_32_type::value_type crc_result;
    fstream checksum_ref (_filename, ios::in);
    while (true) {
        checksum_ref >> crc_name >> std::hex >> crc_result;
        if (checksum_ref.fail ())
	    break;
        _crc_map [crc_name] = crc_result;
    }
    checksum_ref.close ();
}

// Create a checksum for the given view and compare it with the
// reference checksum. Throw exception if different
void checksum_buffer_test::check_view_impl (const stereo32sf_view& buf_view,
					    const string name)
{
    boost::crc_32_type checksum_acumulator;
    checksum_acumulator.process_bytes (buf_view.begin (),
				       buf_view.size () *
				       sizeof (stereo32sf_view::value_type));

    cerr << "Checking checksum for " << name << endl;
    if (checksum_acumulator.checksum () != _crc_map [name])
    {
        cerr << "Checksum error in " << name << "\n";
        BOOST_CHECK (false);
    }
}

/*
 *
 *  Generates a set of reference checksums to compare against
 *
 */

class checksum_buffer_generate : public checksum_buffer_mgr
{
public:
    checksum_buffer_generate (const char* filename)
	: _filename(filename) {}

private:
    const char* _filename;
    virtual void check_view_impl (const stereo32sfc_view& buf_view,
				  const string name);
    virtual void finalize ();
};

// Add the checksum of the given view to the map of checksums
void checksum_buffer_generate::check_view_impl (const stereo32sfc_view& buf_view,
						const string name)
{
    boost::crc_32_type result;
    result.process_bytes (buf_view.begin (),
			  buf_view.size () * sizeof (stereo32sf_view::value_type));
    cerr << "Generating checksum for " << name << endl;
    _crc_map [name] = result.checksum ();
}

// Save the checksums into the reference file
void checksum_buffer_generate::finalize ()
{
    fstream checksum_ref (_filename, ios::out);
    for (crc_map_t::const_iterator it = _crc_map.begin();
	 it != _crc_map.end(); ++it)
    {
        checksum_ref << it->first << " " << std::hex << it->second << "\r\n";
    }
    checksum_ref.close ();
}


/*
 *
 *  Non checking buffer test.
 *
 */

class dummy_buffer_test : public buffer_test
{
    void check_view_impl (const stereo32sfc_view& buf_view,
			  const string name) {}
};


#ifdef BOOST_PSYNTH_NO_IO
typedef checksum_buffer_test     buffer_test_t;
typedef checksum_buffer_generate buffer_generate_t;
#else
typedef dummy_buffer_test buffer_test_t;
typedef dummy_buffer_test buffer_generate_t;
#endif

#ifdef BOOST_PSYNTH_GENERATE_REFERENCE_DATA
typedef buffer_generate_t        buffer_mgr_t;
#else
typedef buffer_test_t            buffer_mgr_t;
#endif

BOOST_AUTO_TEST_SUITE (buffer_test_suite);

BOOST_AUTO_TEST_CASE (test_buffer_all)
{
    buffer_mgr_t mgr; // TODO: Add checksums
    mgr.run ();
}

BOOST_AUTO_TEST_CASE (test_buffer_static_checks)
{
    psynth::base::psynth_function_requires<BufferConcept<stereo32sf_buffer> >();

    BOOST_STATIC_ASSERT(view_is_basic<stereo32sf_step_view>::value);
    BOOST_STATIC_ASSERT(view_is_basic<stereo32sfc_planar_step_view>::value);
    BOOST_STATIC_ASSERT(view_is_basic<stereo32sf_planar_view>::value);

#if 0 // We no longer have view_is_stepe_in_x
    BOOST_STATIC_ASSERT(view_is_step_in_x<stereo32sf_step_view>::value);
    BOOST_STATIC_ASSERT(view_is_step_in_x<stereo32sf_planar_step_view>::value);
    BOOST_STATIC_ASSERT(!view_is_step_in_x<stereo32sf_planar_view>::value);
#endif
    
    BOOST_STATIC_ASSERT(!is_planar<stereo32sf_step_view>::value);
    BOOST_STATIC_ASSERT(is_planar<stereo32sf_planar_step_view>::value);
    BOOST_STATIC_ASSERT(is_planar<stereo32sf_planar_view>::value);

    BOOST_STATIC_ASSERT(view_is_mutable<stereo32sf_step_view>::value);
    BOOST_STATIC_ASSERT(!view_is_mutable<stereo32sfc_planar_step_view>::value);
    BOOST_STATIC_ASSERT(view_is_mutable<stereo32sf_planar_view>::value);

    BOOST_STATIC_ASSERT(
	(boost::is_same<derived_view_type<stereo32sfc_planar_step_view>::type,
			stereo32sfc_planar_step_view>::value));
    BOOST_STATIC_ASSERT(
	(boost::is_same<derived_view_type<stereo32sfc_planar_step_view,
					  bits16,
					  stereo_layout>::type,
			stereo16c_planar_step_view>::value));
    BOOST_STATIC_ASSERT(
	(boost::is_same<derived_view_type<stereo32sf_planar_step_view,
					  use_default,
					  stereo_layout,
					  mpl::false_,
					  use_default,
					  mpl::false_>::type,
			stereo32sfc_step_view>::value));
    
    // test view get raw data (mostly compile-time test)
    {
	stereo8_buffer buf (128);
	unsigned char* data =
	    interleaved_view_get_raw_data (view (buf));
	const unsigned char* cdata =
	    interleaved_view_get_raw_data (const_view (buf));
	BOOST_CHECK_EQUAL (data, cdata);
    }

    {
	stereo16s_planar_buffer buf (128);
	short* data =
	    planar_view_get_raw_data (view (buf), 1);
	const short* cdata =
	    planar_view_get_raw_data (const_view (buf), 1);
	BOOST_CHECK_EQUAL (data, cdata);
    }
}

BOOST_AUTO_TEST_SUITE_END ();
