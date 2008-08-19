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

#ifndef PSYNTH_WATCHVIEWER
#define PSYNTH_WATCHVIEWER

#include <libpsynth/common/mutex.h>
#include <libpsynth/common/ring_audio_buffer.h>
#include <libpsynth/common/ring_sample_buffer.h>
#include <libpsynth/object/Watch.h>

namespace psynth
{

template <class BufferType, class RingBufferType>
class WatchViewer : public Watch
{
protected:
    RingBufferType m_ring;
    BufferType m_buffer;
    int m_points;
    float m_secs;
    float m_factor;
    bool m_updated;
    mutex m_lock;
    
public:
    WatchViewer(int points, float secs) :
	m_ring(points),
	m_buffer(points),
	m_points(points),
	m_factor(1.0f),
	m_secs(secs),
	m_updated(false)
	{
	    m_ring.zero();
	    m_buffer.zero();
	}
    
    virtual void setInfo(const audio_info& info) {
	m_factor = (float) info.sample_rate / ((float) m_points / m_secs);
	m_ring.zero();
    }

    virtual void update(const BufferType& buf) {
	m_lock.lock();
	m_ring.write_fast_resample(buf, m_factor);
	//m_ring.write(buf);
	m_updated = false;
	m_lock.unlock();
    }

    const BufferType& getBuffer() {
	if (!m_updated) {
	    m_lock.lock();
	    typename RingBufferType::read_ptr ptr = m_ring.begin();
	    m_ring.read(ptr, m_buffer);
	    bool m_updated = true;
	    m_lock.unlock();
	}
	return m_buffer;
    }
};

typedef WatchViewer<sample_buffer, ring_sample_buffer> WatchViewControl;

class WatchViewAudio : public WatchViewer<audio_buffer, ring_audio_buffer>
{
public:
    WatchViewAudio(int points, float secs) :
	WatchViewer<audio_buffer,ring_audio_buffer>(points, secs) {}
    
    virtual void setInfo(const audio_info& newinfo) {
	audio_info info = newinfo;
	info.block_size = m_points;
	m_ring.set_info   (info);
	m_buffer.set_info (info);
	m_ring.zero();
	m_factor = (float) info.sample_rate / ((float) m_points / m_secs);
    }
};

} /* namespace psynth */

#endif /* PSYNTH_WATCHVIEWER */
