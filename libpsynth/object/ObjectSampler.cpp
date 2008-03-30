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
#include "common/Misc.h"
#include "object/KnownObjects.h"
#include "object/ObjectSampler.h"

using namespace std;


#define SAMPLER_BLOCK_SIZE  16
//#define SAMPLER_BLOCK_SIZE  getInfo().block_size
#define SCALER_TRAINER_SIZE 1024

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
    m_fetcher(&m_reader),
    m_inbuf(info),
    m_ctrl_pos(0),
    m_param_ampl(0.75f),
    m_param_rate(1.0f),
    m_param_tempo(1.0f),
    m_param_pitch(1.0f)
{
    addParam("file", ObjParam::STRING, &m_param_file,
	     MakeDelegate(this, &ObjectSampler::onFileChange));
    addParam("amplitude", ObjParam::FLOAT, &m_param_ampl);
    addParam("rate", ObjParam::FLOAT, &m_param_rate);
    addParam("tempo", ObjParam::FLOAT, &m_param_tempo);
    addParam("pitch", ObjParam::FLOAT, &m_param_pitch);

    m_scaler.setChannels(info.num_channels);
    m_scaler.setRate(1.0);
    m_scaler.setSampleRate(info.sample_rate);

    m_fetcher.start();
}

ObjectSampler::~ObjectSampler()
{
    m_fetcher.finish();
}

void ObjectSampler::onFileChange(ObjParam& par)
{
    std::string val;
    par.get(val);

    cout << "OPENING FILE: " << val << endl;
    m_update_lock.lock();

    if (m_fetcher.isOpen())
	m_fetcher.close();

    m_fetcher.open(val.c_str());
    m_inbuf.setInfo(m_fetcher.getInfo(), getInfo().block_size);
    m_scaler.setChannels(m_fetcher.getInfo().num_channels);
    m_scaler.setSampleRate(m_fetcher.getInfo().sample_rate);

    m_update_lock.unlock();
}

void ObjectSampler::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    AudioBuffer* out = getOutput<AudioBuffer>(Object::LINK_AUDIO, OUT_A_OUTPUT);
    const ControlBuffer* rate = getInput<ControlBuffer>(Object::LINK_CONTROL, IN_C_RATE);
    const Sample* rate_buf = NULL;

    if (rate)
	rate_buf = rate->getData();
    
    float base_factor =
	(float) m_fetcher.getInfo().sample_rate / getInfo().sample_rate * m_param_rate;

    int must_read;
    int nread;
    float factor = base_factor;
    bool backwards;

    m_update_lock.lock();
    m_scaler.setTempo(m_param_tempo);
    m_scaler.setPitch(m_param_pitch);
    m_update_lock.unlock();
    
    if (m_fetcher.isOpen()) {
	Sample inter_buffer[getInfo().block_size * max(getInfo().num_channels,
						       m_inbuf.getInfo().num_channels)];
	while(m_scaler.availible() < getInfo().block_size) {
	    if (rate)
		factor = base_factor + base_factor * rate_buf[(int) m_ctrl_pos];
	    else
		factor = base_factor;
	    
	    if (factor < 0) {
		backwards = true;
		factor = -factor;
	    } else
		backwards = false;

	    if (backwards != m_fetcher.getBackwards())
		m_fetcher.setBackwards(backwards);
	    
	    if (factor < 0.2)
		factor = 0.2;
	    if (factor * m_param_tempo < 1.0)
		must_read = SAMPLER_BLOCK_SIZE * factor * m_param_tempo;
	    else
		must_read = SAMPLER_BLOCK_SIZE;

	    m_update_lock.lock();

	    nread = m_fetcher.read(m_inbuf, must_read);

	    m_ctrl_pos += (float) nread / factor;
	    if (m_ctrl_pos >= getInfo().block_size)
		m_ctrl_pos = phase(m_ctrl_pos) + ((int) m_ctrl_pos % getInfo().block_size);

	    m_inbuf.interleave(inter_buffer, nread);
	    m_scaler.setRate(factor);
	    m_scaler.update(inter_buffer, nread);
	    
	    m_update_lock.unlock();
	}

	m_scaler.receive(inter_buffer, getInfo().block_size);
	out->deinterleave(inter_buffer, getInfo().block_size, m_scaler.getChannels());
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
    m_scaler.setChannels(getInfo().num_channels);
}

} /* namespace psynth */
