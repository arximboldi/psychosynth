/**
 *  Time-stamp:  <2011-03-09 21:37:48 raskolnikov>
 *
 *  @file        output.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 14:03:53 2011
 *
 *  Unit tests for output devices.
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

#include <thread>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/mpl/filter_view.hpp>
#include <boost/mpl/quote.hpp>

#include "../util.hpp"

#include <psynth/version.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/buffer.hpp>
#include <psynth/sound/buffer_range.hpp>
#include <psynth/new_io/output.hpp>

#ifdef PSYNTH_HAVE_ALSA
#include <psynth/new_io/alsa_output.hpp>
#endif

#ifdef PSYNTH_HAVE_OSS
#include <psynth/new_io/oss_output.hpp>
#endif

#ifdef PSYNTH_HAVE_JACK
#include <psynth/new_io/jack_output.hpp>
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

#ifdef PSYNTH_DEBUG
BOOST_FIXTURE_TEST_SUITE (io_output_test_suite,
                          psynth::test::std_logger_fixture);
#else
BOOST_AUTO_TEST_SUITE (io_output_test_suite);
#endif

BOOST_AUTO_TEST_CASE (buffered_output_test)
{
    
}

BOOST_AUTO_TEST_CASE (buffered_output_adapter_test)
{
    
}

#ifdef PSYNTH_HAVE_ALSA

typedef mpl::filter_view<output_test_types,
                         mpl::quote1<psynth::io::alsa_is_supported> >
alsa_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (alsa_output_test, Range, alsa_test_types)
{
    using namespace psynth;
    typedef typename sound::buffer_type_from_range<Range>::type buffer_type;
    
    const std::size_t buffer_size = 1024;
    const std::size_t sample_rate = 44100;

    try
    {
        buffer_type buf (buffer_size);
        
        std::size_t nframes       = 0;
        std::size_t async_nframes = 0;
        
        io::alsa_output<Range> device (
            "default", buffer_size, sample_rate, [&] (std::size_t)
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
    typedef typename sound::buffer_type_from_range<Range>::type buffer_type;
    
    const std::size_t buffer_size = 1024;
    const std::size_t sample_rate = 44100;
    
    buffer_type buf (buffer_size);
        
    std::size_t nframes       = 0;
    std::size_t async_nframes = 0;

    try
    {
        io::oss_output<Range> device (
            "/dev/dsp", buffer_size, sample_rate, [&] (std::size_t)
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

typedef mpl::filter_view<output_test_types,
                         mpl::quote1<psynth::io::jack_is_supported> >
jack_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE (jack_output_test, Range, jack_test_types)
{    
    using namespace psynth;
    typedef typename sound::buffer_type_from_range<Range>::type buffer_type;
    
    const std::size_t buffer_size = 1024;
    const std::size_t sample_rate = 44100;
    
    buffer_type buf (buffer_size);
    
    std::size_t async_nframes = 0;

    try
    {
        io::jack_output<Range> device (
            "psynth_test", sample_rate, [&] (std::size_t)
            { async_nframes = device.put (range (buf)); });
        
        device.start ();
        for (std::size_t i = 100; async_nframes == 0 && i > 0; --i)
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

BOOST_AUTO_TEST_SUITE_END ();

