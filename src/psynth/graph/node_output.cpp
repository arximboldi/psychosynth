/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#define PSYNTH_MODULE_NAME "psynth.graph.node_output"

#include "base/logger.hpp"
#include "graph/node_types.hpp"
#include "graph/node_output.hpp"
#include "graph/node_manager.hpp"

const int SAFETY_FACTOR = 4;

using namespace std;

namespace psynth
{
namespace graph
{

PSYNTH_DEFINE_NODE_FACTORY(node_output);

node_output::slot::slot (audio_async_output_ptr out,
                         node_output* parent,
                         audio_ring_range::position ptr,
                         const audio_info& info)
    : m_ptr (ptr)
    , m_out (out)
    , m_parent (parent)
    , m_buf (out->buffer_size ())
{
    out->set_callback (std::bind (&node_output::slot::callback, this,
                                  std::placeholders::_1));
}

void node_output::slot::callback (std::size_t nframes)
{
    m_parent->do_output (*this, nframes);
}

node_output::~node_output()
{
    // for (auto i = m_slots.begin(); i != m_slots.end(); ++i)
    //     delete *i;

    // for (auto it = m_passive_slots.begin();
    //      it != m_passive_slots.end();
    //      ++it)
    //     delete *it;
}

void node_output::on_info_change ()
{
    m_buffer.recreate (get_info ().block_size);
    //.set_info (get_info());
    for (std::list<slot*>::iterator i = m_slots.begin(); i != m_slots.end(); ++i)
	(*i)->m_buf.recreate (get_info ().block_size); //.set_info (get_info());

}

void node_output::do_update (const node0* caller,
			      int caller_port_type, int caller_port)
{
    const audio_buffer* in;

    if ((in = get_input<audio_buffer>(LINK_AUDIO, IN_A_INPUT))) {
	//m_buflock.writeLock();
    	range (m_buffer).write (const_range (*in));
	//m_buflock.unlock();

	//m_passive_lock.lock();
	for (auto it = m_passive_slots.begin();
	     it != m_passive_slots.end();
	     ++it)
	    (*it)->put (const_range (*in));
	//m_passive_lock.unlock();
    } else {
    	//m_buflock.writeLock();
        // FIXME: This is slow.
        auto zero_buf = audio_buffer (get_info ().block_size, audio_frame (0), 0);
	range (m_buffer).write (const_range (zero_buf));
	//m_buflock.unlock();
    }
}

node_output::node_output (const audio_info& info) :
    node0 (info,
	  NODE_OUTPUT,
	  "name",
	  N_IN_A_SOCKETS,
	  N_IN_C_SOCKETS,
	  N_OUT_A_SOCKETS,
	  N_OUT_C_SOCKETS),
    m_buffer (16384), // HACK this should be calculated from the
                      // maximum buffer size of the outputs.
    m_manager (NULL)
{
}

/*
  TODO:
  Hay que pensarse bien como sincronizar esto. Aunque no lo necesitamos
  aún.
*/
void node_output::do_output (slot& slot, size_t nframes)
{
    if (nframes > (size_t) slot.m_buf.size()) {
	slot.m_buf.recreate (nframes);
    }

    if (nframes > (size_t) m_buffer.size ()) {
	m_buffer.recreate (nframes * SAFETY_FACTOR);
    }

    if (m_manager) {
	size_t avail;
        auto& rng = range (m_buffer);
	avail = rng.available (slot.m_ptr);

	while (avail < nframes)
        {
	    m_manager->update ();
            avail = rng.available (slot.m_ptr);
	}
    }

    range (m_buffer).read (slot.m_ptr, range (slot.m_buf), nframes);

    slot.m_out->put (sub_range (range (slot.m_buf), 0, nframes));
}

} /* namespace graph */
} /* namespace psynth */
