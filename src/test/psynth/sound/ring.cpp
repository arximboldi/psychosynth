/**
 *  Time-stamp:  <2011-03-22 02:17:13 raskolnikov>
 *
 *  @file        ring.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Nov  9 18:34:14 2010
 *
 *  Ring buffer unit tests.
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

#include <iostream>

#include <boost/test/unit_test.hpp>

#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/output.hpp>

#include <psynth/sound/dynamic_ring_buffer.hpp>
#include <psynth/sound/ring_buffer.hpp>

using namespace psynth::sound;

extern stereo32sf_planar_range sample_range;
const size_t buffer_size = 1024;

BOOST_AUTO_TEST_SUITE (sound_ring_test);

BOOST_AUTO_TEST_CASE (test_ring_buffer)
{
    stereo32sf_planar_buffer bbuf (buffer_size);
    stereo32sf_planar_buffer::range buf (range (bbuf));
    fill_frames (buf, stereo32sf_frame (.0f));

    stereo32sf_ring_buffer rring (buffer_size * 1.3);
    stereo32sf_ring_buffer::range ring (range (rring));
    auto reader = ring.begin_pos ();

    ring.write (sample_range);
    ring.read (reader, buf);
    BOOST_CHECK (equal_frames (buf, sample_range));

    ring.write (sample_range, buffer_size);
    ring.read (reader, buf, buffer_size);

    BOOST_CHECK (equal_frames (buf, sample_range));
}

BOOST_AUTO_TEST_CASE (test_ring_buffer_nr)
{
    stereo32sf_planar_buffer buf (buffer_size);
    fill_frames (range (buf), stereo32sf_frame (.0f));

    stereo32sf_ring_buffer ring (buffer_size * 1.3);
    auto reader = range (ring).begin_pos ();

    range (ring).write (sample_range);
    range (ring).read (reader, range (buf));
    BOOST_CHECK (equal_frames (range (buf), sample_range));

    range (ring).write (sample_range, buffer_size);
    range (ring).read (reader, range (buf), buffer_size);

    BOOST_CHECK (equal_frames (range (buf), sample_range));
}

BOOST_AUTO_TEST_CASE (test_ring_buffer_iterator)
{
    stereo32sf_ring_buffer rring (buffer_size * 1.3);
    stereo32sf_ring_buffer::range ring (range (rring));

    ring.write (sample_range);
    BOOST_CHECK (std::equal (ring.begin_unsafe (), ring.end_unsafe (),
			     sample_range.begin ()));
    BOOST_CHECK (std::equal (ring.begin (), ring.end (),
			     sample_range.begin ()));

    ring.write (sample_range, buffer_size);
    BOOST_CHECK (std::equal (ring.end_unsafe () - buffer_size, ring.end_unsafe (),
			     sample_range.begin ()));
    BOOST_CHECK (std::equal (ring.end () - buffer_size, ring.end (),
			     sample_range.begin ()));

}

typedef
dynamic_buffer<boost::mpl::vector<
		   mono8_buffer, stereo16_planar_buffer> > some_buffer;

typedef
dynamic_ring_buffer<some_buffer> some_ring_buffer;

BOOST_AUTO_TEST_CASE (test_dynamic_ring_buffer)
{
    BOOST_TEST_CHECKPOINT ("Create buffer.");
    some_buffer buf { stereo16_planar_buffer (buffer_size) };
    fill_frames (range (buf), stereo16_frame (.0f));
    // TODO: Avoid range()?

    BOOST_TEST_CHECKPOINT ("Create ring buffer.");
    some_ring_buffer rring { mono8_buffer (buffer_size * 1.3) };

    BOOST_TEST_CHECKPOINT ("Create ring buffer range.");
    // BOOST_MPL_ASSERT_MSG(false,
    //                      PSYNTH_CHECK_TYPES,
    //                      (some_ring_buffer::range,
    //                       decltype (range (rring))));
    some_ring_buffer::range ring (range (rring));

    BOOST_TEST_CHECKPOINT ("Create position.");
    auto reader = ring.begin_pos ();

    BOOST_TEST_CHECKPOINT ("Write and convert.");
    ring.write_and_convert (sample_range);
    ring.read_and_convert (reader, range (buf));
    BOOST_CHECK (equal_frames (range (buf),
			       channel_converted_range<stereo16_frame> (
				   channel_converted_range<mono8_frame> (
				       sample_range))));

    BOOST_TEST_CHECKPOINT ("Write and convert more.");
    ring.write_and_convert (sample_range, buffer_size);
    ring.read_and_convert (reader, range (buf), buffer_size);
    BOOST_CHECK (equal_frames (range (buf),
			       channel_converted_range<stereo16_frame> (
				   channel_converted_range<mono8_frame> (
				       sample_range))));
}

BOOST_AUTO_TEST_SUITE_END ();
