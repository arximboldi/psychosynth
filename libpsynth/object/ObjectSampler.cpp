/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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

#include "input/FileReaderAny.h"
#include "common/RingAudioBuffer.h"
#include "common/AudioBuffer.h"
#include "common/Mutex.h"
#include "object/KnownObjects.h"
#include "object/ObjectSampler.h"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectSampler);

ObjectSampler::ObjectSampler(const AudioInfo& info): 
    Object(info,
	   OBJ_SAMPLER,
	   "sampler",
	   N_IN_A_SOCKETS, 
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_buffer(info, 2 * info.block_size),
    m_inbuf(info),
    m_param_ampl(0.5f),
    m_param_pitch(1.0f)
{
    m_read_ptr = m_buffer.begin();
    
    addParam("file", ObjParam::STRING, &m_param_file, MakeDelegate(this, &ObjectSampler::onFileChange));
    addParam("amplitude", ObjParam::FLOAT, &m_param_ampl);
    addParam("pitch", ObjParam::FLOAT, &m_param_pitch);

    m_reader.setBufferSize(getInfo().block_size);
}

void ObjectSampler::onFileChange(ObjParam& par)
{
    std::string val;
    par.get(val);

    cout << "OPENING FILE: " << val << endl;
    m_update_lock.lock();

    if (m_reader.isOpen())
	m_reader.close();

    m_reader.open(val.c_str());
    
    m_update_lock.unlock();
}

void ObjectSampler::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    AudioBuffer* out = getOutput<AudioBuffer>(Object::LINK_AUDIO, OUT_A_OUTPUT);
    const ControlBuffer* pitch = getInput<ControlBuffer>(Object::LINK_CONTROL, IN_C_PITCH);
    
    float factor =
	(float) getInfo().sample_rate / m_reader.getInfo().sample_rate * m_param_pitch;

    int must_read;
    int nread;
    
    if (m_reader.isOpen()) {
	while(m_buffer.availible(m_read_ptr) < getInfo().block_size) {	
	    if (factor < 1.0)
		must_read = getInfo().block_size * factor;
	    else
		must_read = getInfo().block_size;

	    m_update_lock.lock();
	    nread = m_reader.read(m_inbuf, must_read);
	    m_update_lock.unlock();
	    
	    if (nread < must_read)
		m_reader.seek(0);

	    m_buffer.writeFastResample(m_inbuf, nread, factor);
	    cout << "read: " << nread << endl;
	    cout << "must read: " << must_read << endl;
	    cout << "buffer: " << m_reader.getBufferSize() << endl;
	    cout << "factor: " << factor << endl;
	}

	m_buffer.read(m_read_ptr, *out);
    } else {
	out->zero();
    }

    Sample* buf = out->getData()[0];
    int count = getInfo().block_size * getInfo().num_channels;
    while(count--)
	*buf++ *= m_param_ampl; 
}

void ObjectSampler::doAdvance()
{
}

void ObjectSampler::onInfoChange()
{
    m_buffer.setAudioInfo(getInfo());
    m_buffer.resize(getInfo().block_size);
    m_reader.setBufferSize(getInfo().block_size);
}

} /* namespace psynth */
