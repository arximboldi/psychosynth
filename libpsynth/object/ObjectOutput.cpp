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

#include "object/KnownObjects.h"
#include "object/ObjectOutput.h"
#include "object/ObjectManager.h"

const int SAFETY_FACTOR = 4;

using namespace std;

namespace psynth
{

ObjectOutput::~ObjectOutput()
{
    for (std::list<Slot*>::iterator i = m_slots.begin(); i != m_slots.end(); ++i)
	delete *i;

    for (list<Output*>::iterator it = m_passive_slots.begin();
	 it != m_passive_slots.end();
	 ++it)
	delete *it;
}

void ObjectOutput::onInfoChange()
{
    m_buffer.setAudioInfo(getInfo());
    for (std::list<Slot*>::iterator i = m_slots.begin(); i != m_slots.end(); ++i)
	(*i)->m_buf.setInfo(getInfo());
    
}

void ObjectOutput::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    const AudioBuffer* in;
    
    if ((in = getInput<AudioBuffer>(LINK_AUDIO, IN_A_INPUT))) {
	//m_buflock.writeLock();
	m_buffer.write(*in);
	//m_buflock.unlock();
	
	//m_passive_lock.lock();
	for (list<Output*>::iterator it = m_passive_slots.begin();
	     it != m_passive_slots.end();
	     ++it)
	    (*it)->put(*in);
	//m_passive_lock.unlock();
    } else {
	//m_buflock.writeLock();
	m_buffer.write(AudioBuffer(getAudioInfo()));
	//m_buflock.unlock();
    }
}

ObjectOutput::ObjectOutput(AudioInfo& info) :
    Object(info,
	   OBJ_OUTPUT,
	   N_IN_A_SOCKETS, 
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_buffer(info, info.block_size * SAFETY_FACTOR),
    m_manager(NULL)
{
}

void ObjectOutput::outputCallback(int nframes, void* arg)
{
    Slot* slot = static_cast<Slot*>(arg);
	
    slot->m_parent->output(*slot, nframes);
}

/*
  TODO:
  Hay que pensarse bien como sincronizar esto. Aunque no lo necesitamos
  aún.
*/
void ObjectOutput::output(Slot& slot, size_t nframes)
{
    if (nframes > slot.m_buf.size()) {
	slot.m_buf.resize(nframes);
    }

    if (nframes > m_buffer.size()) {
	m_buffer.resize(nframes * SAFETY_FACTOR);
    }

    if (m_manager) {
	int avail;

	avail = m_buffer.availible(slot.m_ptr);
	
	while(avail < nframes) {
	    m_manager->update();
	    
	    avail = m_buffer.availible(slot.m_ptr);
	}
    }

    m_buffer.read(slot.m_ptr, slot.m_buf, nframes);
    
    slot.m_out->put(slot.m_buf, nframes);
}

} /* namespace psynth */
