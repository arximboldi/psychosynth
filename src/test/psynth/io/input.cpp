/**
 *  Time-stamp:  <2011-03-17 15:57:42 raskolnikov>
 *
 *  @file        input.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Mar 17 14:54:17 2011
 *
 *  @brief Tests for the input classes in psynth::io.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
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
#include <thread>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/quote.hpp>
#include <boost/filesystem/operations.hpp>

#include "../util.hpp"

#include <psynth/version.hpp>
#include <psynth/base/meta.hpp>
#include <psynth/base/scope_guard.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/buffer.hpp>
#include <psynth/sound/buffer_range.hpp>
#include <psynth/sound/output.hpp>
#include <psynth/new_io/input.hpp>
#include <psynth/new_io/buffered_input.hpp>

#ifdef PSYNTH_HAVE_PCM
#include <psynth/new_io/file_input.hpp>
#endif

namespace mpl = boost::mpl;

typedef mpl::list<
    psynth::sound::mono8s_range,
    psynth::sound::mono16_range,
    psynth::sound::stereo16s_range,
    psynth::sound::stereo16s_planar_range,
    psynth::sound::stereo32sf_range,
    psynth::sound::stereo32sf_planar_range
    >
input_test_types;

typedef psynth::base::flatten<
    mpl::transform<
        input_test_types,
        mpl::lambda<psynth::base::pair_seq<
                        mpl::_1, input_test_types> >
        >::type
    >::type
input_test_types_product;

#ifdef PSYNTH_DEBUG
BOOST_FIXTURE_TEST_SUITE (io_input_test_suite,
                          psynth::test::std_logger_fixture);
#else
BOOST_AUTO_TEST_SUITE (io_input_test_suite);
#endif


/* General case. */
template <class R1, class R2>
struct do_test_buffered_input
{
    void operator () ()
    {
        using namespace psynth;

        typedef R1 src_range;
        typedef R2 dst_range;
        
        typedef typename sound::buffer_type_from_range<src_range>::type
            src_buffer_type;

        const std::size_t buffer_size = 1024;

        const typename dst_range::value_type zero (
            sound::sample_traits<
                typename sound::sample_type <dst_range>::type>::zero_value ()); 
        typename src_range::value_type src_zero;
        sound::channel_convert (zero, src_zero);
        
        src_buffer_type buf (buffer_size);
        src_buffer_type test_buf (buffer_size, src_zero, 0);
        
        io::buffered_input<src_range, io::dummy_input<dst_range> > out;
        out.take (range (buf));
        BOOST_CHECK(equal_frames (range (buf), range (test_buf)));
        
        auto aout = io::make_buffered_input<src_range> (&out.input ());
        aout.take (range (buf));
        BOOST_CHECK(equal_frames (range (buf), range (test_buf)));
    }
};

/*
 * When both are the same it skips the buffer.
 * TODO: Find a way to test this.
 */
template <class R>
struct do_test_buffered_input<R, R>
{
    void operator () () {}
};

BOOST_AUTO_TEST_CASE_TEMPLATE (buffered_input_test, RangePair,
                               input_test_types_product)
{
    
    typedef typename RangePair::first  src_range;
    typedef typename RangePair::second dst_range;

    do_test_buffered_input<src_range, dst_range> () ();
}

/* General case. */
template <class R1, class R2>
struct do_test_async_buffered_input
{
    void operator () ()
    {
        using namespace psynth;

        typedef R1 src_range;
        typedef R2 dst_range;
        
        typedef typename sound::buffer_type_from_range<src_range>::type
            src_buffer_type;

        const std::size_t buffer_size = 1024;

        const typename dst_range::value_type zero (
            sound::sample_traits<
                typename sound::sample_type <dst_range>::type>::zero_value ()); 
        typename src_range::value_type src_zero;
        sound::channel_convert (zero, src_zero);
        
        src_buffer_type buf (buffer_size);
        src_buffer_type test_buf (buffer_size, src_zero, 0);
        
        io::buffered_async_input<
            src_range,
            io::dummy_async_input<dst_range> > out (buffer_size);
        out.take (range (buf));
        BOOST_CHECK(equal_frames (range (buf), range (test_buf)));
        
        auto aout = io::make_buffered_input<src_range> (&out.input ());
        aout.take (range (buf));
        BOOST_CHECK(equal_frames (range (buf), range (test_buf)));
    }
};

/*
 * When both are the same it skips the buffer.
 * TODO: Find a way to test this.
 */
template <class R>
struct do_test_async_buffered_input<R, R>
{
    void operator () () {}
};

BOOST_AUTO_TEST_CASE_TEMPLATE (buffered_async_input_test, RangePair,
                               input_test_types_product)
{
    
    typedef typename RangePair::first  src_range;
    typedef typename RangePair::second dst_range;

    do_test_async_buffered_input<src_range, dst_range> () ();
}


BOOST_AUTO_TEST_SUITE_END ();

