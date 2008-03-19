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

#include "input/FileReaderAny.h"
#include "common/RingAudioBuffer.h"
#include "common/AudioBuffer.h"
#include "common/Mutex.h"
#include "object/KnownObjects.h"
#include "object/ObjectSampler.h"

using namespace std;

#define SAMPLER_BLOCK_SIZE  16

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
    
    addParam("file", ObjParam::STRING, &m_param_file,
	     MakeDelegate(this, &ObjectSampler::onFileChange));
    addParam("amplitude", ObjParam::FLOAT, &m_param_ampl);
    addParam("pitch", ObjParam::FLOAT, &m_param_pitch);

    m_reader.setBufferSize(info.block_size);
    m_scaler.setChannels(info.num_channels);
    m_scaler.setRate(1.0);
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

//    m_inbuf.setInfo(m_reader.getInfo(), getInfo().block_size);
    m_scaler.setSampleRate(m_reader.getInfo().sample_rate);
    
    m_update_lock.unlock();
}

void ObjectSampler::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    AudioBuffer* out = getOutput<AudioBuffer>(Object::LINK_AUDIO, OUT_A_OUTPUT);
    const ControlBuffer* pitch = getInput<ControlBuffer>(Object::LINK_CONTROL, IN_C_PITCH);
    const Sample* pitch_buf = NULL;

    if (pitch)
	pitch_buf = pitch->getData();
    
    float factor =
	(float) m_reader.getInfo().sample_rate / getInfo().sample_rate * m_param_pitch;

    int must_read;
    int nread;
    float pos;
    
    if (m_reader.isOpen()) {
	while(m_buffer.availible(m_read_ptr) < getInfo().block_size) {
	    if (factor < 1.0)
		must_read = SAMPLER_BLOCK_SIZE * factor;
	    else
		must_read = SAMPLER_BLOCK_SIZE;

	    m_update_lock.lock();
	    nread = m_reader.read(m_inbuf, must_read);

	    if (nread == 0) {
		m_reader.seek(0);
		m_update_lock.unlock();
		continue;
	    }
	    
	    if (pitch)
		m_scaler.setRate(factor + factor * pitch_buf[(int) pos]);
	    else
		m_scaler.setRate(factor);
	    pos += nread / factor;
	    
	    m_update_lock.unlock();

	    m_buffer.writeScaler(m_inbuf, nread, m_scaler);
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
    m_buffer.resize(getInfo().block_size * 4);
    m_reader.setBufferSize(getInfo().block_size);
    m_scaler.setChannels(getInfo().num_channels);
}

} /* namespace psynth */
