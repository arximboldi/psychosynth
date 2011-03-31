/**
 *  Time-stamp:  <2011-03-31 14:01:43 raskolnikov>
 *
 *  @file        buffer.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 12:54:33 2010
 *
 *  Unit tests for the buffers.
 *
 *  @todo Split buffer_test class into several tests.
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
#include <psynth/sound/dynamic_buffer_range.hpp>
#include <psynth/sound/dynamic_buffer_range_factory.hpp>

#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/output.hpp>

using namespace psynth::sound;
using namespace std;
using namespace boost;

extern stereo32sf_planar_range sample_range;

// When PSYNTH_SOUND_GENERATE_REFERENCE_DATA is defined, the reference
// data is generated and saved.  When it is undefined, regression
// tests are checked against it #define
// PSYNTH_SOUND_GENERATE_REFERENCE_DATA

/*
 *
 *      Some algorithms to use in testing
 *
 */

template <typename MonoRange, typename R>
void mono_buffer_hist (const MonoRange& buf_range, R& hist)
{
    // TODO: Does this function make sense?
    // for_each_frame (buf_range, ++ lambda::var (hist) [lambda::_1]);
    for (auto it = buf_range.begin(); it != buf_range.end(); ++it)
        ++ hist [*it];
}

template <typename V, typename R>
void get_hist (const V& buf_range, R& hist)
{
    mono_buffer_hist (channel_converted_range<mono8_frame> (buf_range), hist);
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
void gradient (const T& src, const mono8s_range& dst)
{
    typename T::iterator src_it = src.begin ();
    mono8s_range::iterator dst_it = dst.begin ();

    for (int x = 1; x < src.size () - 1; ++x)
	dst_it [x] = (src_it [x+1] - src_it [x-1]) / 2;
}

// A quick test whether a range is homogeneous
template <typename Frame>
struct frame_is_homogeneous : public mpl::true_ {};

template <typename P, typename C, typename L>
struct frame_is_homogeneous<packed_frame<P,C,L> > : public mpl::false_ {};

template <typename Range>
struct range_is_homogeneous :
    public frame_is_homogeneous<typename Range::value_type> {};

/*
 *
 *      Tests buffer range transformations and algorithms
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
    virtual void check_range_impl (const stereo32sfc_range& range,
				  const string name) = 0;
    
    template <typename Range>
    void check_range (const Range& buf_range, const string name)
    {
        stereo32sf_buffer stereo_buf (buf_range.size ());
        copy_and_convert_frames (buf_range, range (stereo_buf));
        check_range_impl (const_range (stereo_buf), name);
    }

private:
    template <typename Buf>
    void basic_test (const string prefix);

    template <typename Range>
    void range_transformations_test (const Range& buf_range, const string prefix);

    template <typename Range>
    void homogeneous_range_transformations_test (
	const Range& buf_range, const string prefix, mpl::true_);

    template <typename Range>
    void homogeneous_range_transformations_test (
	const Range& buf_range, const string prefix, mpl::false_) {}

    template <typename Range>
    void histogram_test (const Range& buf_range, const string prefix);

    void virtual_range_test ();
    void packed_buffer_test ();
    void dynamic_buffer_test ();

    template <typename Buf>
    void buffer_all_test (const string prefix);
};


// testing buffer iterators, clone, fill, locators, channel convert
template <typename Buf>
void buffer_test::basic_test (const string prefix)
{
    typedef typename Buf::range Range;

    Buf buf (20);
    const Range& buf_range = range (buf);

    // fill it with "left on"
    stereo32sf_frame left32 (1.0f, 0), right32 (0, -1);
    
    typename Range::value_type left, right, zero (0);
    channel_convert (left32, left);
    default_channel_converter () (left32,left);
    left = channel_convert_deref_fn<
	stereo32sf_ref, typename Buf::range::value_type> () (left32);

    channel_convert (right32, right);
    
    fill (buf_range.begin (), buf_range.end (), left);
    
    channel_convert (left32, buf_range [0]);

    // pointer to frame in the middle
    typename Range::reference rt   = buf_range.at (0) [buf_range.size () >> 1];
    typename Range::iterator  ptr  = &rt;
    typename Range::reference rt2  = *(buf_range.at (0) + (buf_range.size () >> 1));
    typename Range::iterator  ptr2 = &rt2;
    BOOST_CHECK_EQUAL (ptr, ptr2);
    BOOST_CHECK_EQUAL (buf_range.at (0) + 10, 10 + buf_range.at (0));
    
    // draw 'right' dots with step of 3
    auto it = buf_range.at (buf_range.size () - 1);
    while (it >= buf_range.at (0)) {
        *it  = right;
        it  -= 3;
    }

    // Clone and zero every "left" frame white
    Buf buf_zero (buf);
    for (typename Range::iterator it = range (buf_zero).end ();
	 (it - 1) != range (buf_zero).begin (); --it)
    {
        if (*(it - 1) == left)
            *(it - 1) = zero;
    }

    check_range (buf_range, prefix + "left_x");
    check_range (range (buf_zero), prefix + "zero_x");
}

template <typename Range>
void buffer_test::histogram_test (const Range& buf_range, const string prefix)
{
    //  vector<int> histogram (255,0);
    //  get_hist (cropped, histogram.begin ());
    unsigned char histogram [256];
    fill (histogram, histogram + 256, 0);
    
    get_hist (buf_range, histogram);
    mono8c_range hist_range =
	interleaved_range (256, (const mono8_frame*) histogram);
    check_range (hist_range, prefix + "histogram");
}


template <typename Range>
void buffer_test::range_transformations_test (const Range& buf_range,
					     const string prefix)
{
    check_range (buf_range, prefix + "original");

    check_range (sub_range (
		    buf_range, buf_range.size () / 4, buf_range.size () / 2),
	prefix + "cropped");
    check_range(channel_converted_range<mono8_frame>(buf_range), prefix + "mono8");
    check_range(channel_converted_range<mono8_frame>(
		   buf_range, my_channel_converter ()), prefix + "my_mono8");
    check_range (flipped_range (buf_range), prefix + "flipped_ud");
    check_range (sub_sampled_range (buf_range, 2), prefix + "subsampled");   
    check_range (kth_sample_range<0>(buf_range), prefix + "0th_k_sample");
    homogeneous_range_transformations_test (
	buf_range, prefix, range_is_homogeneous<Range>());
}

template <typename Range>
void buffer_test::homogeneous_range_transformations_test (
    const Range& buf_range, const string prefix, mpl::true_)
{
    check_range (nth_sample_range (buf_range, 0), prefix + "0th_n_sample");
}

void buffer_test::virtual_range_test ()
{
    // TODO: Design a new test that does not require fucking
    // mandelbrot :D
    
#if 0
    typedef mandelbrot_fn<stereo32sf_frame_t> deref_t;
    typedef deref_t::point_t            point_t;
    typedef virtual_2d_locator<deref_t,false> locator_t;
    typedef buffer_range<locator_t> my_virt_range;

    boost::function_requires<FrameLocatorConcept<locator_t> >();
    gil_function_requires<StepIteratorConcept<locator_t::x_iterator> >();

    point_t dims(200,200);
    my_virt_range mandel(
	dims, locator_t(point_t(0,0), point_t(1,1),
			deref_t(dims, stereo32sf_frame_t(255,0,255),
				stereo32sf_frame_t(0,255,0))));

    mono8s_buffer buf(dims);
    fill_frames(range(buf),0);
    // our x_gradient algorithm doesn't change the first & last
    // column, so make sure they are 0
    x_gradient(channel_converted_range<mono8_frame_t>(mandel), range(buf));
    check_range(channel_converted_range<mono8_frame_t>(
		   const_range(buf)), "mandelLuminosityGradient");

    range_transformations_test(mandel,"virtual_");
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
    fill_frames (range (stereo13_buf), fill_val);
    
    stereo13_buffer stereo13a_buf (10, 1);
    copy_frames(const_range(stereo13_buf), range(stereo13a_buf));
    
    stereo13_buffer stereo13b_buf (10, 4);
    copy_frames(const_range (stereo13_buf), range(stereo13b_buf));
    
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
    
    stereo32sf_planar_buffer buf (sample_range.size ());
    copy_frames (sample_range, range (buf));
    
    test_buffer any_buf = test_buffer (buf);
    
    check_range (range (any_buf), "dynamic_");
    check_range (flipped_range (range (any_buf)), "dynamic_fliplr");
        
    test_buffer::range sub_buffer = sub_range (
	range (any_buf), 128, 256);
    
    check_range (sub_buffer, "dynamic_subbuffer");
    check_range (sub_sampled_range (range (any_buf), 2),
		"dynamic_subbuffer_subsampled180rot");
}

template <typename Buf>
void buffer_test::buffer_all_test (const string prefix)
{
    basic_test<Buf> (prefix + "basic_");
    
    Buf buf;
    buf.recreate (sample_range.size ());
    copy_and_convert_frames (sample_range, range (buf));
    
    range_transformations_test (range (buf), prefix + "ranges_");
    histogram_test (const_range (buf),prefix + "histogram_");
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
    range_transformations_test (sub_sampled_range (sample_range, 2), "subsampled_");
    range_transformations_test (channel_converted_range<mono8_frame>(sample_range),
			       "channel_converted_");

    virtual_range_test ();
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
    virtual void check_range_impl (const stereo32sf_range& v, const string name);
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

// Create a checksum for the given range and compare it with the
// reference checksum. Throw exception if different
void checksum_buffer_test::check_range_impl (const stereo32sf_range& buf_range,
					    const string name)
{
    boost::crc_32_type checksum_acumulator;
    checksum_acumulator.process_bytes (buf_range.begin (),
				       buf_range.size () *
				       sizeof (stereo32sf_range::value_type));

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
    virtual void check_range_impl (const stereo32sfc_range& buf_range,
				  const string name);
    virtual void finalize ();
};

// Add the checksum of the given range to the map of checksums
void checksum_buffer_generate::check_range_impl (const stereo32sfc_range& buf_range,
						const string name)
{
    boost::crc_32_type result;
    result.process_bytes (buf_range.begin (),
			  buf_range.size () * sizeof (stereo32sf_range::value_type));
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
    void check_range_impl (const stereo32sfc_range& buf_range,
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

    BOOST_STATIC_ASSERT(range_is_basic<stereo32sf_step_range>::value);
    BOOST_STATIC_ASSERT(range_is_basic<stereo32sfc_planar_step_range>::value);
    BOOST_STATIC_ASSERT(range_is_basic<stereo32sf_planar_range>::value);

#if 0 // We no longer have range_is_stepe_in_x
    BOOST_STATIC_ASSERT(range_is_step_in_x<stereo32sf_step_range>::value);
    BOOST_STATIC_ASSERT(range_is_step_in_x<stereo32sf_planar_step_range>::value);
    BOOST_STATIC_ASSERT(!range_is_step_in_x<stereo32sf_planar_range>::value);
#endif
    
    BOOST_STATIC_ASSERT(!is_planar<stereo32sf_step_range>::value);
    BOOST_STATIC_ASSERT(is_planar<stereo32sf_planar_step_range>::value);
    BOOST_STATIC_ASSERT(is_planar<stereo32sf_planar_range>::value);

    BOOST_STATIC_ASSERT(range_is_mutable<stereo32sf_step_range>::value);
    BOOST_STATIC_ASSERT(!range_is_mutable<stereo32sfc_planar_step_range>::value);
    BOOST_STATIC_ASSERT(range_is_mutable<stereo32sf_planar_range>::value);

    BOOST_STATIC_ASSERT(
	(boost::is_same<derived_range_type<stereo32sfc_planar_step_range>::type,
			stereo32sfc_planar_step_range>::value));
    BOOST_STATIC_ASSERT(
	(boost::is_same<derived_range_type<stereo32sfc_planar_step_range,
					  bits16,
					  stereo_layout>::type,
			stereo16c_planar_step_range>::value));
    BOOST_STATIC_ASSERT(
	(boost::is_same<derived_range_type<stereo32sf_planar_step_range,
					  use_default,
					  stereo_layout,
					  mpl::false_,
					  use_default,
					  mpl::false_>::type,
			stereo32sfc_step_range>::value));
    
    // test range get raw data (mostly compile-time test)
    {
	stereo8_buffer buf (128);
	unsigned char* data =
	    interleaved_range_get_raw_data (range (buf));
	const unsigned char* cdata =
	    interleaved_range_get_raw_data (const_range (buf));
	BOOST_CHECK_EQUAL (data, cdata);
    }

    {
	stereo16s_planar_buffer buf (128);
	short* data =
	    planar_range_get_raw_data (range (buf), 1);
	const short* cdata =
	    planar_range_get_raw_data (const_range (buf), 1);
	BOOST_CHECK_EQUAL (data, cdata);
    }
}

BOOST_AUTO_TEST_SUITE_END ();
