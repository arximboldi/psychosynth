/**
 *  Time-stamp:  <2010-11-07 16:19:07 raskolnikov>
 *
 *  @file        frame_iterator.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Sun Nov  7 12:50:41 2010
 *
 *  Frame iterator unit tests.
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

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <boost/mpl/vector.hpp>

#include <psynth/sound/planar_frame_reference.hpp>
#include <psynth/sound/stereo.hpp>
#include <psynth/sound/frame_iterator.hpp>
#include <psynth/sound/frame_iterator_adaptor.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>
#include <psynth/sound/bit_aligned_frame_iterator.hpp>
#include <psynth/sound/packed_frame.hpp>
#include <psynth/sound/step_iterator.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/channel_convert.hpp>
#include <psynth/sound/buffer_view_factory.hpp>
#include <psynth/sound/output.hpp>

using namespace psynth::base;
using namespace psynth::sound;
using namespace std;
using namespace boost;

BOOST_AUTO_TEST_SUITE (frame_iterator_test_suite);

BOOST_AUTO_TEST_CASE (test_frame_iterator)
{
    boost::function_requires<MutableFrameIteratorConcept<rlstereo8_ptr> >();
    boost::function_requires<MutableFrameIteratorConcept<quad8_planar_ptr> >();
    boost::function_requires<FrameIteratorConcept<stereo8c_planar_step_ptr> >();
    boost::function_requires<MutableStepIteratorConcept<stereo8_step_ptr> >();

    boost::function_requires<MutableStepIteratorConcept<quad8_planar_step_ptr> >();
    boost::function_requires<StepIteratorConcept<mono8c_step_ptr> >();

    typedef const bit_aligned_frame_reference<
	boost::uint8_t, boost::mpl::vector2_c<int,1,2>,
	rlstereo_layout, true>  rlstereo12_ref;

    typedef bit_aligned_frame_iterator<rlstereo12_ref> rlstereo12_ptr;

    boost::function_requires<MutableFrameIteratorConcept<rlstereo12_ptr> >();
    boost::function_requires<FrameBasedConcept<rlstereo12_ptr> >();
    boost::function_requires<MemoryBasedIteratorConcept<rlstereo12_ptr> >();
    boost::function_requires<HasDynamicStepTypeConcept<rlstereo12_ptr> >();

    // TEST dynamic_step
    BOOST_STATIC_ASSERT(
	(boost::is_same <quad16_step_ptr,
			 dynamic_step_type<quad16_step_ptr>::type>::value)); 

    BOOST_STATIC_ASSERT(
	(boost::is_same <quad16_planar_step_ptr,
			 dynamic_step_type<quad16_planar_ptr>::type>::value)); 

    BOOST_STATIC_ASSERT(
	(boost::is_same<
	    iterator_type<bits8, mono_layout, false, false, false>::type,
	    mono8c_ptr>::value));

    // TEST iterator_is_step
    BOOST_STATIC_ASSERT(iterator_is_step <quad16_step_ptr>::value);
    BOOST_STATIC_ASSERT(iterator_is_step <quad16_planar_step_ptr>::value);
    BOOST_STATIC_ASSERT(!iterator_is_step <quad16_planar_ptr>::value);

    typedef channel_convert_deref_fn<stereo8c_ref, mono8_frame> ccv_stereo_g_fn;
    typedef channel_convert_deref_fn<mono8c_ref, stereo8_frame> ccv_g_stereo_fn;
    psynth_function_requires<
	FrameDereferenceAdaptorConcept<ccv_stereo_g_fn> >();
    psynth_function_requires<
	FrameDereferenceAdaptorConcept<
	    deref_compose<ccv_stereo_g_fn, ccv_g_stereo_fn> > >();

    typedef dereference_iterator_adaptor<stereo8_ptr, ccv_stereo_g_fn>
	stereo2mono_ptr;
    BOOST_STATIC_ASSERT(!iterator_is_step< stereo2mono_ptr >::value);

    typedef dynamic_step_type<stereo2mono_ptr>::type stereo2mono_step_ptr;
    BOOST_STATIC_ASSERT(
	(boost::is_same <
	    stereo2mono_step_ptr,
	    dereference_iterator_adaptor<stereo8_step_ptr,
					 ccv_stereo_g_fn> >::value));
    make_step_iterator (stereo2mono_ptr (), 2);

    typedef dereference_iterator_adaptor<stereo8_step_ptr, ccv_stereo_g_fn>
	stereo2mono_step_ptr1;
    
    BOOST_STATIC_ASSERT (iterator_is_step <stereo2mono_step_ptr1>::value);
    BOOST_STATIC_ASSERT (
	(boost::is_same <
	    stereo2mono_step_ptr1,
	    dynamic_step_type<stereo2mono_step_ptr1>::type>::value));

    typedef memory_based_step_iterator<
	dereference_iterator_adaptor <stereo8_ptr, ccv_stereo_g_fn> >
	stereo2mono_step_ptr2;
    BOOST_STATIC_ASSERT(iterator_is_step< stereo2mono_step_ptr2 >::value);
    BOOST_STATIC_ASSERT(
	(boost::is_same<
	    stereo2mono_step_ptr2,
	    dynamic_step_type<stereo2mono_step_ptr2>::type >::value));
    make_step_iterator (stereo2mono_step_ptr2 (), 2);

    // bit_aligned iterators test

    // Mutable reference to a RLSTEREO232 frame
    typedef const bit_aligned_frame_reference<
	boost::uint8_t, boost::mpl::vector2_c<unsigned,2,3>,
	rlstereo_layout, true>  rlstereo23_ref;

    // A mutable iterator over RLSTEREO23 frames
    typedef bit_aligned_frame_iterator<rlstereo23_ref> rlstereo23_ptr;

    // RLSTEREO23 frame value. It is a packed_frame of size 1
    // byte. (The last bit is unused)
    typedef std::iterator_traits<rlstereo23_ptr>::value_type rlstereo23_frame; 
    BOOST_STATIC_ASSERT((sizeof (rlstereo23_frame) == 1));

    rlstereo23_frame left (0, 3); // = 0RRGGGBB, = 01100000
    // a buffer of 7 bytes fits exactly 8 RLSTEREO23 frames.
    unsigned char pix_buffer [7];    
    std::fill (pix_buffer, pix_buffer + 7, 0);
    rlstereo23_ptr pix_it (&pix_buffer [0], 0);
    // start at bit 0 of the first frame
    for (int i = 0; i < 8; ++i)
        *pix_it++ = left;
}

/*
  GIL-TODO: Make better tests. Use some code from below.
  Psynth: We re-enabled and cleaned up the tests a bit, take a deeper
  look at it later.
*/

template <typename Frame>
void invert_frame1 (Frame& pix)
{
    psynth::sound::at_c<0> (pix) = 0;
}

BOOST_AUTO_TEST_CASE (test_frame_iterator_old)
{
    stereo8_frame stereo8 (1, 2);
    quad8_frame   quad8;

    stereo8_ptr ptr1 = &stereo8;
    memunit_advance (ptr1, 3);
    const stereo8_ptr ptr2 = memunit_advanced (ptr1, 10);

    memunit_distance (ptr1, ptr2);
    const stereo8_frame& ref = memunit_advanced_ref (ptr1,10);
    ignore_unused_variable_warning (ref);

    stereo8_planar_ptr planar_ptr1 (&stereo8);
    stereo8_planar_ptr planar_ptr2 (&stereo8);
    memunit_advance (planar_ptr1, 10);
    memunit_distance (planar_ptr1, planar_ptr2);
    stereo8_planar_ptr planar_ptr3 = memunit_advanced (planar_ptr1, 10);

    // planar_ptr2=&quad8;

    planar_frame_reference<bits8&, stereo_space> pxl = *(planar_ptr1 + 5);
    stereo8_frame pv2 = pxl;
    stereo8_frame pv3 = *(planar_ptr1 + 5);
    stereo8_frame pv  = planar_ptr1[5];

    BOOST_CHECK (*(planar_ptr1 + 5) == planar_ptr1 [5]);

    stereo8_planar_ref planar_ref = memunit_advanced_ref (planar_ptr1, 10);

    stereo8_step_ptr step_it (&stereo8, 5);
    step_it++;
    stereo8_step_ptr step_it2 = step_it + 10;
    step_it2 = step_it;
    
    stereo8_step_ptr step_it3(&stereo8,5);

    stereo8_frame& ref1=step_it3[5];
    //  bool
    //  v=boost::is_pod<iterator_traits<memory_based_step_iterator<stereo8_ptr>
    //  >::value_type>::value; v=boost::is_pod<stereo8_frame>::value;
    //  v=boost::is_pod<int>::value;

    stereo8_step_ptr stereo8_step_it (ptr1, 10);
    stereo8_step_ptr stereo8_step_it2 = stereo8_step_it;
    stereo8_step_it = stereo8_step_it2;
    ++ stereo8_step_it;
    stereo8_ref reff = *stereo8_step_it;
    ignore_unused_variable_warning(reff);
    stereo8_step_it += 10;
    ptrdiff_t dst = stereo8_step_it2-stereo8_step_it;
    ignore_unused_variable_warning (dst);

    stereo8_frame val1 = ref1;
    stereo8_ptr ptr = &ref1;

    invert_frame1 (*planar_ptr1);
    //    invert_frame1(*ptr);
    stereo8c_planar_ptr r8cpp;
    //    invert_frame1(*r8cpp);

    stereo8_frame& val21 = step_it3 [5];
    stereo8_frame val22 = val21;

    stereo8_frame val2 = step_it3[5];
    stereo8_ptr ptr11  = &(step_it3[5]); ignore_unused_variable_warning(ptr11);
    stereo8_ptr ptr3   = &*(step_it3+5); ignore_unused_variable_warning(ptr3);

    stereo8_step_ptr step_it4 (ptr,5);
    ++step_it4;

    stereo8_step_ptr step_it5;
    BOOST_CHECK (step_it4 != step_it5);
    
    make_step_iterator(stereo8_ptr(),3);
    make_step_iterator(stereo8_planar_ptr(),3);
    make_step_iterator(stereo8_planar_step_ptr(),3);

    // Test operator-> on planar ptrs
    {
#if 0
	stereo8c_planar_ptr cp (&stereo8);
	stereo8_planar_ptr p (&stereo8);
        //    get_channel(p,red_t()) = get_channel(cp,green_t());
        //    does not compile - cannot assign a non-const pointer to
        //    a const pointer. Otherwise you will be able to modify
        //    the value through it.
#endif
    }

    // dimensions to explore
    //
    // values, references, pointers
    // channel spaces (stereo,quad,mono)
    // sample ordering (rlstereo vs stereo) 
    // planar vs interleaved    

    // Frame POINTERS
    //  typedef const iterator_traits<stereo8_ptr>::pointer  stereo8_const_ptr;
    typedef const stereo8_ptr  stereo8_const_ptr;
    typedef const stereo8_planar_ptr  stereo8_const_planar_ptr;
    // typedef const iterator_traits<stereo8_planar_ptr>::pointer
    // stereo8_const_planar_ptr;

    // constructing from values, references and other pointers
    stereo8_const_ptr stereo8constptr = 0;
    ignore_unused_variable_warning (stereo8constptr);
    stereo8_ptr stereo8ptr = &stereo8;


    stereo8 = rlstereo8_frame (30, 20);
    stereo8_planar_ptr stereo8_pptr = &stereo8;
    ++stereo8_pptr;
    stereo8_pptr--;
    stereo8_pptr [0] = stereo8;
    stereo8_const_planar_ptr stereo8constplanarptr = &stereo8;

    stereo8c_planar_ptr r8c = &stereo8;
    r8c = &stereo8;
    
    //  stereo8_const_planar_ptr = &stereo16p;
    //  error: incompatible bit depth

    //  iterator_traits<QUAD8>::pointer quad8_ptr = &stereo8;
    //  error: incompatible pointer type

    stereo8_const_ptr stereo8_const_ptr_err = stereo8ptr;
    // const pointer from non-regular pointer
    ignore_unused_variable_warning (stereo8_const_ptr_err);
    // dereferencing pointers to obtain references
    stereo8_ref stereo8ref_2 = *stereo8ptr;
    BOOST_CHECK_EQUAL (stereo8ref_2, stereo8);
    //  STEREO8Ref stereo8ref_2_err=*stereo8_const_planar_ptr;
    // error: non-const reference from const pointer
    stereo8_planar_ref stereo8planarref_3 = *stereo8_pptr;
    // planar reference from planar pointer
    BOOST_CHECK_EQUAL (stereo8planarref_3, stereo8);
    BOOST_CHECK_EQUAL (*r8c, stereo8);
    //  STEREO8Ref stereo8ref_3=*stereo8_planar_ptr;
    // error: non-planar reference from planar pointer

    const stereo8_frame cstereo8 = stereo8;
    *stereo8_pptr = stereo8;
    *stereo8_pptr = cstereo8;

    memunit_advance (stereo8_pptr, 3);
    memunit_advance (stereo8_pptr, -3);
}

BOOST_AUTO_TEST_SUITE_END ();
