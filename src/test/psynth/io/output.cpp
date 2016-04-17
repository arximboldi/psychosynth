/**
 *  @file        output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 14:03:53 2011
 *
 *  Unit tests for output devices.
 */

/*
 *  Copyright (C) 2011, 2016 Juan Pedro Bolívar Puente
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
#include <psynth/io/output.hpp>
#include <psynth/io/buffered_output.hpp>

#ifdef PSYNTH_HAVE_ALSA
#include <psynth/io/alsa_output.hpp>
#define TEST_DEVICE_ALSA "null"
//#define TEST_DEVICE_ALSA "default"
#endif

#ifdef PSYNTH_HAVE_OSS
#include <psynth/io/oss_output.hpp>
#define TEST_DEVICE_OSS "/dev/null"
//#define TEST_DEVICE_OSS "/dev/dsp"
#endif

#ifdef PSYNTH_HAVE_JACK
#include <psynth/io/jack_output.hpp>
#endif

#ifdef PSYNTH_HAVE_PCM
#include <psynth/io/file_output.hpp>
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
output_test_types;

typedef psynth::base::flatten<
    mpl::transform<
        output_test_types,
        mpl::lambda<psynth::base::pair_seq<
                        mpl::_1, output_test_types> >
        >::type
    >::type
output_test_types_product;

#ifdef PSYNTH_DEBUG
BOOST_FIXTURE_TEST_SUITE (io_output_test_suite,
                          psynth::test::std_logger_fixture);
#else
BOOST_AUTO_TEST_SUITE (io_output_test_suite);
#endif

/* General case. */
template <class R1, class R2>
struct do_test_buffered_output
{
    void operator () ()
    {
        using namespace psynth;

        typedef R1 src_range;
        typedef R2 dst_range;
        
        typedef typename sound::buffer_from_range<src_range>::type
            src_buffer_type;

        const std::size_t tmp_buffer_size = 1024;
        const std::size_t out_buffer_size = 512;
    
        src_buffer_type buf (tmp_buffer_size);
        fill_frames (range (buf), typename src_range::value_type (0));
    
        io::buffered_output<src_range, io::dummy_output<dst_range> > out;
        out.set_buffer_size (out_buffer_size);
        out.put (range (buf));

        BOOST_CHECK(
            equal_frames (
                sound::channel_converted_range<typename dst_range::value_type> (
                    sub_range (range (buf), 512, 512)),
                const_range (out.buffer ())));

        auto aout = io::make_buffered_output<src_range> (&out.output ());
        aout.set_buffer_size (out_buffer_size);
        aout.put (range (buf));

        BOOST_CHECK(
            equal_frames (
                sound::channel_converted_range<typename dst_range::value_type> (
                    sub_range (range (buf), 512, 512)),
                const_range (aout.buffer ())));
    }
};

/*
 * When both are the same it skips the buffer.
 * TODO: Find a way to test this.
 */
template <class R>
struct do_test_buffered_output<R, R>
{
    void operator () () {}
};

BOOST_AUTO_TEST_CASE_TEMPLATE (buffered_output_test, RangePair,
                               output_test_types_product)
{
    
    typedef typename RangePair::first  src_range;
    typedef typename RangePair::second dst_range;

    do_test_buffered_output<src_range, dst_range> () ();
}

/* General case. */
template <class R1, class R2>
struct do_test_async_buffered_output
{
    void operator () ()
    {
        using namespace psynth;

        typedef R1 src_range;
        typedef R2 dst_range;
        
        typedef typename sound::buffer_from_range<src_range>::type
            src_buffer_type;

        const std::size_t buffer_size = 1024;
            
        src_buffer_type buf (buffer_size);
        fill_frames (range (buf), typename src_range::value_type (0));
    
        io::buffered_async_output<
            src_range,
            io::dummy_async_output<dst_range> > out (buffer_size);
        out.put (range (buf));
        
        BOOST_CHECK(
            equal_frames (
                sound::channel_converted_range<typename dst_range::value_type> (
                    range (buf)),
                const_range (out.buffer ())));

        auto aout = io::make_buffered_output<src_range> (&out.output ());
        aout.put (range (buf));

        BOOST_CHECK(
            equal_frames (
                sound::channel_converted_range<typename dst_range::value_type> (
                    range (buf)),
                const_range (aout.buffer ())));
    }
};

/*
 * When both are the same it skips the buffer.
 * TODO: Find a way to test this.
 */
template <class R>
struct do_test_async_buffered_output<R, R>
{
    void operator () () {}
};

BOOST_AUTO_TEST_CASE_TEMPLATE (buffered_async_output_test, RangePair,
                               output_test_types_product)
{
    
    typedef typename RangePair::first  src_range;
    typedef typename RangePair::second dst_range;

    do_test_async_buffered_output<src_range, dst_range> () ();
}

#ifdef PSYNTH_HAVE_ALSA

typedef mpl::filter_view<output_test_types,
                         mpl::quote1<psynth::io::alsa_is_supported> >
alsa_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (alsa_output_test, Range, alsa_test_types)
{
    using namespace psynth;
    typedef typename sound::buffer_from_range<Range>::type buffer_type;
    
    const std::size_t buffer_size = 1024;
    const std::size_t sample_rate = 44100;

    try
    {
        buffer_type buf (buffer_size, 0);
        
        std::size_t nframes       = 0;
        std::size_t async_nframes = 0;
        
        io::alsa_output<Range> device (
            TEST_DEVICE_ALSA, 2, buffer_size/2, sample_rate, [&] (std::size_t)
            { async_nframes = device.put (range (buf)); });

        nframes = device.put (range (buf));

        device.start ();
        for (std::size_t i = 100; async_nframes == 0 && i > 0; --i)
            // HACK because g++-4.5 does not have this_thread::sleep
            ::usleep (10);
        device.stop ();
        
        BOOST_CHECK_EQUAL (nframes, buffer_size);
        BOOST_CHECK_EQUAL (async_nframes, buffer_size);
    }
    catch (io::alsa_error& err)
    {
        BOOST_TEST_MESSAGE("ALSA test did throw an exception but it "
                           "might be caused by the system.");
        err.log ();
    }
    
}

#endif /* PSYNTH_HAVE_ALSA */

#ifdef PSYNTH_HAVE_OSS

typedef mpl::filter_view<output_test_types,
                         mpl::quote1<psynth::io::oss_is_supported> >
oss_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (oss_output_test, Range, oss_test_types)
{    
    using namespace psynth;
    typedef typename sound::buffer_from_range<Range>::type buffer_type;
    
    const std::size_t buffer_size = 1024;
    const std::size_t sample_rate = 44100;
    
    buffer_type buf (buffer_size, 0);
        
    std::size_t nframes       = 0;
    std::size_t async_nframes = 0;

    try
    {
        io::oss_output<Range> device (
            TEST_DEVICE_OSS, buffer_size, sample_rate, [&] (std::size_t)
            { async_nframes = device.put (range (buf)); });

        nframes = device.put (range (buf));

        device.start ();
        for (std::size_t i = 100; async_nframes == 0 && i > 0; --i)
            // HACK because g++-4.5 does not have this_thread::sleep
            ::usleep (10);
        device.stop ();
       
        BOOST_CHECK_EQUAL (nframes, buffer_size);
        BOOST_CHECK_EQUAL (async_nframes, buffer_size);
    }
    catch (io::oss_error& err)
    {
        BOOST_TEST_MESSAGE("OSS test did throw an exception but it "
                           "might be caused by the system.");
        err.log ();
    }
}

#endif /* PSYNTH_HAVE_OSS */

#ifdef PSYNTH_HAVE_JACK

#if 0
typedef mpl::filter_view<output_test_types,
                         mpl::quote1<psynth::io::jack_is_supported> >
jack_test_types;
#endif

typedef mpl::vector<> jack_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (jack_output_test, Range, jack_test_types)
{    
    using namespace psynth;
    typedef typename sound::buffer_from_range<Range>::type buffer_type;
    
    const std::size_t buffer_size = 1024;
    const std::size_t sample_rate = 44100;
    
    buffer_type buf (buffer_size, 0);
    
    std::size_t async_nframes = 0;

    try
    {
        io::jack_output<Range> device (
            "psynth_test", sample_rate, [&] (std::size_t)
            { async_nframes = device.put (range (buf)); });
        
        device.start ();
        for (std::size_t i = 1000; async_nframes == 0 && i > 0; --i)
            // HACK because g++-4.5 does not have this_thread::sleep
            ::usleep (10);
        device.stop ();
       
        BOOST_CHECK_EQUAL (async_nframes, buffer_size);
    }
    catch (io::jack_error& err)
    {
        BOOST_TEST_MESSAGE("JACK test did throw an exception but it "
                           "might be caused by the system.");
        err.log ();
    }
}

#endif /* PSYNTH_HAVE_JACK */

#ifdef PSYNTH_HAVE_PCM

typedef mpl::filter_view<output_test_types,
                         mpl::quote1<psynth::io::file_is_supported> >
file_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (file_output_test, Range, file_test_types)
{    
    using namespace psynth;
    namespace fs = boost::filesystem;
    
    typedef typename sound::buffer_from_range<Range>::type buffer_type;

    const std::string filename    = std::tmpnam (0);
    const std::size_t sample_rate = 44100;
    const std::size_t buffer_size = 1024;
    
    buffer_type buf (buffer_size);
    PSYNTH_ON_BLOCK_EXIT ([&] { fs::remove (fs::path (filename)); });

    for (int fmt = 0; fmt < (int) io::file_fmt::num_formats; ++fmt)
        try
        {
            io::file_output<Range> device (
                filename, (io::file_fmt) fmt, sample_rate);
            std::size_t nframes = device.put (range (buf));
            BOOST_CHECK_EQUAL (nframes, buffer_size);
        }
        catch (io::file_error& err)
        {
            BOOST_TEST_MESSAGE("File test did throw an exception but it "
                               "might be caused by the system.");
            err.log ();
        }
}

#endif /* PSYNTH_HAVE_PCM */

BOOST_AUTO_TEST_SUITE_END ();

