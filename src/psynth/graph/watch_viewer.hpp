/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#ifndef PSYNTH_WATCH_VIEWER_H
#define PSYNTH_WATCH_VIEWER_H

#include <mutex>
#include <iostream> // FIXME
#include <psynth/graph/watch.hpp>
#include <psynth/sound/buffer.hpp>
#include <psynth/sound/ring_buffer.hpp>

namespace psynth
{
namespace graph
{

template <class BufferType, class RingBufferType>
class watch_viewer : public watch
{
protected:
    RingBufferType m_ring;
    BufferType m_buffer;
    int m_points;
    float m_factor;
    float m_secs;
    bool m_updated;
    std::mutex m_mutex;

public:
    watch_viewer (std::size_t points, std::size_t secs) :
	m_ring(points),
	m_buffer(points, typename BufferType::range::value_type (0), 0),
	m_points(points),
	m_factor(2),
	m_secs(secs),
	m_updated(false)
	{
            //FIXME: This does not work but it should. Fix it some day.
            //fill_frames (range (m_ring),
            //           typename BufferType::range::value_type (0));
	}

    virtual void set_info (const audio_info& info) {
	m_factor = (float) info.sample_rate / ((float) m_points / m_secs);
        //m_ring.zero();
    }

    virtual void update (const typename BufferType::const_range& buf)
    {
        std::unique_lock<std::mutex> lock (m_mutex, std::try_to_lock_t ());
        if (lock.owns_lock ())
        {
            range (m_ring).write (buf);//sound::sub_sampled_range (buf, m_factor));
            m_updated = false;
        }
    }

    const BufferType& get_buffer ()
    {
	if (!m_updated)
        {
	    std::unique_lock<std::mutex> lock (m_mutex);
	    typename RingBufferType::position ptr = range (m_ring).begin_pos ();
	    range (m_ring).read (ptr, range (m_buffer));
	    m_updated = true;
	}
	return m_buffer;
    }
};

typedef watch_viewer<sample_buffer, sample_ring_buffer> watch_view_control;

class watch_view_audio : public watch_viewer<audio_buffer, audio_ring_buffer>
{
public:
    typedef watch_viewer<audio_buffer, audio_ring_buffer> base;

    watch_view_audio (std::size_t points, std::size_t secs) :
	base (points, secs) {}

    virtual void set_info (const audio_info& newinfo)
    {
	audio_info info = newinfo;
	info.block_size = m_points;
	m_ring.recreate (info.block_size, audio_frame (0), 0);
	m_buffer.recreate (info.block_size, audio_frame (0), 0);
	//m_factor = (float) info.sample_rate / ((float) m_points / m_secs);
    }
};

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_WATCHVIEWER */
