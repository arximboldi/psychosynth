/**
 *  Time-stamp:  <2010-11-04 14:51:59 raskolnikov>
 *
 *  @file        frame.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 12:12:39 2010
 *
 *  Unit tests for the frame.
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

#include <iterator>
#include <iostream>
#include <boost/type_traits.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/test/unit_test.hpp>

#include <psynth/sound/planar_frame_reference.hpp>
#include <psynth/sound/packed_frame.hpp>
#include <psynth/sound/frame.hpp>
#include <psynth/sound/sample_algorithm.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/channel_convert.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/bit_aligned_frame_reference.hpp>
#include <psynth/sound/output.hpp>

#include <psynth/sound/stereo.hpp>
#include <psynth/sound/mono.hpp>
#include <psynth/sound/quad.hpp>
#include <psynth/sound/surround.hpp>

// Testing frame references and values, frame operations, channel
// conversion

using namespace psynth::sound;
using std::swap;
using namespace boost;

BOOST_AUTO_TEST_SUITE (sound_frame_test_suite);

struct increment {
    template <typename Incrementable>
    void operator () (Incrementable& x) const { ++x; }
};

struct prev {
    template <typename Subtractable>
    typename sample_traits<Subtractable>::value_type
    operator () (const Subtractable& x) const { return x-1; }
};

struct set_to_one {
    int operator ()() const { return 1; }
};

// Construct with two frame types. They must be compatible and the
// second must be mutable
template <typename C1, typename C2>
struct do_basic_test : public C1, public C2
{
    typedef typename C1::type                frame1;
    typedef typename C2::type                frame2;
    typedef typename C1::frame::value_type frame1_value;
    typedef typename C2::frame::value_type frame2_value;
    typedef frame1_value frame_value;

    do_basic_test (const frame_value& v)
	: C1(v), C2(v) {}

    void test_all ()
    {
        test_heterogeneous ();

        // test homogeneous algorithms - fill, max, min
        static const int num_chan = num_samples<typename C2::frame>::value;
        static_fill (C2::_frame, psynth::sound::at_c<0>(C1::_frame) + 1);
        BOOST_CHECK_EQUAL (psynth::sound::at_c<0> (C2::_frame),
			   psynth::sound::at_c<num_chan-1>(C2::_frame));

        C2::_frame = C1::_frame;
        BOOST_CHECK_EQUAL (static_max (C2::_frame), static_max (C1::_frame));
        BOOST_CHECK_EQUAL (static_min (C2::_frame), static_min (C1::_frame));
        BOOST_CHECK_GE    (static_max (C2::_frame), static_min (C2::_frame));

        // test operator[]
        C2::_frame[0] = C1::_frame[0]+1;
        BOOST_CHECK_EQUAL (C2::_frame[0], C1::_frame[0]+1);
    }

    void test_heterogeneous ()
    {
        // Both must be frame types (not necessarily frame
        // values). The second must be mutable. They must be
        // compatible
	boost::function_requires<FrameConcept<typename C1::frame> >();
        boost::function_requires<MutableFrameConcept<typename C2::frame> >();
        boost::function_requires<FramesCompatibleConcept<
	    typename C1::frame, typename C2::frame> >();

        C2::_frame = C1::_frame;                    // test operator=
        BOOST_CHECK_EQUAL (C1::_frame, C2::_frame); // test operator==

        // construct a frame value from it
        frame1_value v1 (C1::_frame);
        frame2_value v2 (C2::_frame);
        BOOST_CHECK_EQUAL (v1, v2);

        // construct from a frame value
        frame1 c1 (v1);
        frame2 c2 (v2);
        BOOST_CHECK_EQUAL (c1, c2);

        // Invert the first semantic sample.
        C2::_frame = C1::_frame;
        semantic_at_c<0>(C2::_frame) =
	    sample_invert (semantic_at_c<0>(C2::_frame));

	BOOST_CHECK_NE (C1::_frame, C2::_frame); // now they must not be equal

        // test frame algorithms
        C2::_frame = C1::_frame;
        static_for_each (C2::_frame, increment ());
        static_transform (C2::_frame, C2::_frame, prev());
        BOOST_CHECK_EQUAL (C1::_frame, C2::_frame);

        static_generate (C2::_frame, set_to_one());
        BOOST_CHECK_EQUAL (psynth::sound::at_c<0>(C2::_frame), 1);

        // Test swap if both are mutable and if their value type is
        // the same (We know the second one is mutable)
        typedef typename boost::add_reference<typename C1::type>::type p1_ref;
        test_swap(
            boost::mpl::bool_<
                frame_reference_is_mutable<p1_ref>::value &&
                boost::is_same<frame1_value,frame2_value>::value> ());
    }

    void test_swap (boost::mpl::false_) {}

    void test_swap (boost::mpl::true_)
    {
	// test swap
	static_fill(C1::_frame, 0);
	static_fill(C2::_frame, 1);
	frame_value pv1(C1::_frame);
	frame_value pv2(C2::_frame);
	BOOST_CHECK_NE (C2::_frame, C1::_frame);

	swap (C1::_frame, C2::_frame);
	BOOST_CHECK_EQUAL (C1::_frame, pv2);
	BOOST_CHECK_EQUAL (C2::_frame, pv1);
    }
};

template <typename FrameValue, int Tag=0>
class value_core
{
public:
    typedef FrameValue type;
    typedef type        frame;
    type _frame;

    value_core ()
	: _frame(0) {}

    value_core (const type& val)
	: _frame(val)
    {
	// test copy constructor
        boost::function_requires<FrameValueConcept<frame> >();
        type p2;            // test default constructor
    }
};

template <typename FrameRef, int Tag=0>
class reference_core : public value_core<
    typename boost::remove_reference<FrameRef>::type::value_type, Tag>
{
public:
    typedef FrameRef type;
    typedef typename boost::remove_reference<FrameRef>::type frame;
    typedef value_core<typename frame::value_type, Tag> parent;

    type _frame;

    reference_core ()
	: parent()
	, _frame (parent::_frame)
    {}

    reference_core (const typename frame::value_type& val)
	: parent(val)
	, _frame(parent::_frame)
    {
        boost::function_requires<FrameConcept<frame> >();
    }
};

/*
 * Use a subset of frame models that covers all channel spaces, sample
 * depths, reference/value, planar/interleaved, const/mutable channel
 * conversion will be invoked on pairs of them. Having an exhaustive
 * binary check would be too big/expensive.
 */
typedef mpl::vector<
    value_core     <mono8_frame>,
    reference_core <mono16_frame&>
/*    reference_core <mono32fs_frame&>,
    value_core     <rlstereo8_frame>,
    reference_core <stereo8_planar_ref>,
    value_core     <quad32_frame>,
    value_core     <quad32f_frame>,
    value_core     <quad32fs_frame>,
    reference_core <surround32f_frame&>,
    reference_core <rlstereo16c_ref>,       // immutable reference
    reference_core <stereo32fc_planar_ref> */
    > representative_frames;

namespace detail
{

template <typename Vector, typename Fun, int K>
struct for_each_impl
{
    static void apply (Fun fun)
    {
        for_each_impl<Vector,Fun,K-1>::apply (fun);
        fun (typename mpl::at_c<Vector,K>::type ());
    }
};

template <typename Vector, typename Fun>
struct for_each_impl <Vector,Fun,-1>
{
    static void apply (Fun fun) {}
};

template <typename Vector, typename Fun>
void for_each (Fun fun)
{
    for_each_impl<Vector,Fun, mpl::size<Vector>::value-1>::apply (fun);
}

} /* namespace detail */

template <typename Frame1>
struct ccv2
{
    template <typename P1, typename P2>
    void channel_convert_compatible (const P1& p1, P2& p2, mpl::true_)
    {
        typedef typename P1::value_type value;
        p2 = p1;
        value converted;
        channel_convert (p1, converted);
        BOOST_CHECK_EQUAL (converted, p2);
    }

    template <typename P1, typename P2>
    void channel_convert_compatible(const P1& p1, P2& p2, mpl::false_)
    {
        channel_convert (p1,p2);
    }

    template <typename P1, typename P2>
    void channel_convert_impl(const P1& p1, P2& p2)
    {
        channel_convert_compatible (
	    p1, p2, mpl::bool_<frames_are_compatible<P1,P2>::value>());
    }

    template <typename Frame2>
    void operator () (Frame2)
    {
        // convert from Frame1 to Frame2 (or, if Frame2 is immutable,
        // to its value type)
        static const int p2_is_mutable =
	    frame_reference_is_mutable<typename Frame2::type>::type::value;
        typedef typename boost::remove_reference<typename Frame2::type>::type
	    frame_model;
        typedef typename frame_model::value_type
	    p2_value;
        typedef typename mpl::if_c<
	    p2_is_mutable, Frame2, value_core<p2_value> >::type frame2_mutable;

        Frame1 p1;
        frame2_mutable p2;

        channel_convert_impl (p1._frame, p2._frame);
    }
};

struct ccv1
{
    template <typename Frame>
    void operator () (Frame)
    {
	detail::for_each<representative_frames> (ccv2<Frame>());
    }
};

BOOST_AUTO_TEST_CASE (test_channel_convert)
{
    detail::for_each<representative_frames> (ccv1());
}

BOOST_AUTO_TEST_CASE (test_packed_frame_0)
{
    typedef packed_frame_type<uint16_t,
			      mpl::vector2_c<unsigned,7,9>,
			      stereo_layout >::type
	stereo79_frame;

    boost::function_requires <FrameValueConcept<stereo79_frame> >();
    BOOST_STATIC_ASSERT((sizeof(stereo79_frame) == 2));

    typedef packed_frame_type<uint16_t,
			      mpl::vector2_c<unsigned,9,7>,
			      rlstereo_layout>::type
	rlstereo97_frame;

    boost::function_requires<FrameValueConcept<rlstereo97_frame> >();

    // Create a zero packed frame and a full regular unpacked frame.
    stereo79_frame r79; //((uint16_t)0);
    stereo8_frame stereo_full (255, 255);

    // Convert all samples of the unpacked frame to the packed one &
    // assert the packed one is full
    get_channel(r79, left_channel ())   = sample_convert <
	kth_element_type<stereo79_frame, 0>::type>(
	    get_channel (stereo_full, left_channel ()));
    get_channel(r79, right_channel ())  = sample_convert <
	kth_element_type<stereo79_frame, 1>::type>(
	    get_channel (stereo_full, right_channel ()));

    BOOST_CHECK_EQUAL (r79, stereo79_frame ((uint16_t) 65535));

    // stereo79 is compatible with rlstereo97. Test interoperability
    boost::function_requires<FramesCompatibleConcept<
	stereo79_frame, rlstereo97_frame> >();

    do_basic_test<value_core<stereo79_frame,0>,
		  value_core<rlstereo97_frame,1> >(r79).test_heterogeneous ();

    channel_convert (r79, stereo_full);
    channel_convert (stereo_full, r79);
}

BOOST_AUTO_TEST_CASE (test_packed_frame_1)
{
    // Test bit-aligned frame reference
    typedef const bit_aligned_frame_reference<
	boost::uint8_t,
	boost::mpl::vector2_c<int, 1, 1>,
	stereo_layout, true>  rlstereo11_ref;

    typedef const bit_aligned_frame_reference<
	boost::uint8_t,
	boost::mpl::vector2_c<int, 1, 1>,
	stereo_layout, true>  stereo11_ref;

    typedef stereo11_ref::value_type stereo11_frame;

    stereo11_frame p11 (0);
    do_basic_test <reference_core<rlstereo11_ref, 0>,
		   reference_core<stereo11_ref, 1> >(p11).test_heterogeneous();
    do_basic_test<value_core<stereo11_frame, 0>,
		  reference_core<stereo11_ref, 1> >(p11).test_heterogeneous();

    BOOST_STATIC_ASSERT((frame_reference_is_proxy<stereo8_planar_ref>::value));
    BOOST_STATIC_ASSERT((frame_reference_is_proxy<rlstereo11_ref>::value));

    BOOST_STATIC_ASSERT(!(frame_reference_is_proxy<stereo8_frame>::value));
    BOOST_STATIC_ASSERT(!(frame_reference_is_proxy<stereo8_frame&>::value));
    BOOST_STATIC_ASSERT(!(frame_reference_is_proxy<
			  const stereo8_frame&>::value));

    BOOST_STATIC_ASSERT((frame_reference_is_mutable<stereo8_frame&>::value));
    BOOST_STATIC_ASSERT(!(frame_reference_is_mutable<
			  const stereo8_frame&>::value));

    BOOST_STATIC_ASSERT((frame_reference_is_mutable<
			 const stereo8_planar_ref&>::value));
    BOOST_STATIC_ASSERT((frame_reference_is_mutable<
			     stereo8_planar_ref >::value));

    BOOST_STATIC_ASSERT(!(frame_reference_is_mutable<
			  const stereo8c_planar_ref&>::value));
    BOOST_STATIC_ASSERT(!(frame_reference_is_mutable<
			      stereo8c_planar_ref >::value));

    BOOST_STATIC_ASSERT((frame_reference_is_mutable<rlstereo11_ref>::value));
    BOOST_STATIC_ASSERT(!(frame_reference_is_mutable<
			      rlstereo11_ref::const_reference>::value));
}

BOOST_AUTO_TEST_CASE (test_frame)
{
    stereo8_frame stereo8 (1, 2);

    do_basic_test<value_core<stereo8_frame,0>,
		  reference_core<stereo8_frame&,1> >(stereo8).test_all();
    do_basic_test<value_core<rlstereo8_frame,0>,
		  reference_core<stereo8_planar_ref,1> >(stereo8).test_all();
    do_basic_test<reference_core<stereo8_planar_ref,0>,
		  reference_core<rlstereo8_frame&,1> >(stereo8).test_all();
    do_basic_test<reference_core<const stereo8_frame&,0>,
		  reference_core<stereo8_frame&,1> >(stereo8).test_all();

    // Semantic vs physical sample accessors. Named sample accessors
    rlstereo8_frame rlstereo8(stereo8);
    BOOST_CHECK_NE (rlstereo8 [0],
		    stereo8 [0]);
    BOOST_CHECK_NE (dynamic_at_c (rlstereo8, 0),
		    dynamic_at_c (stereo8, 0));
    BOOST_CHECK_NE (psynth::sound::at_c<0>(rlstereo8),
		    psynth::sound::at_c<0>(stereo8));
    BOOST_CHECK_EQUAL (semantic_at_c<0> (rlstereo8),
		       semantic_at_c<0> (stereo8));
    BOOST_CHECK_EQUAL (get_channel (rlstereo8, left_channel ()),
		       get_channel (stereo8, left_channel ()));

    // Assigning a grayscale sample to a frame
    mono16_frame g16 (34);
    g16 = 8;
    bits16 g = get_channel (g16, mono_channel ());
    BOOST_CHECK_EQUAL (g, 8);
    BOOST_CHECK_EQUAL (g16, 8);
}

BOOST_AUTO_TEST_SUITE_END ();
