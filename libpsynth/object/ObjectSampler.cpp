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
#include "common/FileManager.h"
#include "object/KnownObjects.h"
#include "object/ObjectSampler.h"

using namespace std;

#define TIME_STRETCH_MIN_SAMPLES 4096
#define SMALL_BLOCK_SIZE 64
#define BIG_BLOCK_SIZE   1024

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
    m_param_pitch(1.0f),
    m_restart(false)
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
    std::string path;
    par.get(val);

    path = FileManager::instance().getPath("psychosynth/samples").find(val);
    
    m_update_lock.lock();

    if (m_fetcher.isOpen())
	m_fetcher.close();
    
    m_fetcher.open(path.c_str());

    if (m_fetcher.isOpen()) {
	m_inbuf.setInfo(m_fetcher.getInfo(), getInfo().block_size);
	m_scaler.setChannels(m_fetcher.getInfo().num_channels);
	//m_scaler.setSampleRate(m_fetcher.getInfo().sample_rate);
    }
    
    m_update_lock.unlock();
}

void ObjectSampler::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    AudioBuffer* out = getOutput<AudioBuffer>(Object::LINK_AUDIO, OUT_A_OUTPUT);
    const ControlBuffer* trig = getInput<ControlBuffer>(Object::LINK_CONTROL, IN_C_TRIGGER);
    const Sample* trig_buf = trig ? trig->getData() : 0;
    EnvelopeSimple trig_env =  getInEnvelope(LINK_CONTROL, IN_C_TRIGGER);

    /* Read the data. */
    size_t start = 0;
    size_t end = getInfo().block_size;

    if (m_fetcher.isOpen()) {
	while(start < getInfo().block_size) {
	    if (m_restart) {
		if (trig_buf && trig_buf[start] != 0.0f) {
		    restart();
		    m_restart = false;
		}
	    }
	
	    if (trig)
		end = trig->findHill(start);

	    read(*out, start, end);
	
	    float env_val = trig_env.update(end - start);
	    if (env_val == 1.0f && trig_buf && trig_buf[end - 1] == 0.0f)
		m_restart = true;
	    
	    start = end;
	} 
    } else
	out->zero();
    
    /* Set amplitude. */
    Sample* buf = out->getData()[0];
    int count = getInfo().block_size * getInfo().num_channels;
    while(count--)
	*buf++ *= m_param_ampl;

    /* Apply trigger envelope. */
    if (trig_buf) {
	for (int i = 0; i < getInfo().num_channels; ++i) {
	    buf = out->getChannel(i);
	    int n_samp = getInfo().block_size;
	    trig_buf = trig->getData();
	    trig_env = getInEnvelope(LINK_CONTROL, IN_C_TRIGGER);

	    while (n_samp--) {
		float env_val = trig_env.update();
		*buf = *buf * ((1.0f - env_val) + (env_val * *trig_buf));
		++buf;
		++trig_buf;
	    }
	}
    }
    
}

void ObjectSampler::restart()
{
    m_fetcher.forceSeek(0);
    m_scaler.clear();
}

void ObjectSampler::read(AudioBuffer& buf, int start, int end)
{
    const ControlBuffer* rate = getInput<ControlBuffer>(Object::LINK_CONTROL, IN_C_RATE);
    const Sample* rate_buf = rate ? rate->getData() : 0;
    
    float base_factor =
	(float) m_fetcher.getInfo().sample_rate / getInfo().sample_rate * m_param_rate;

    int must_read;
    int nread;
    float factor;

    bool backwards = false;;
    bool high_latency = false;
    
    m_update_lock.lock();
    m_scaler.setTempo(m_param_tempo);
    m_scaler.setPitch(m_param_pitch);
    m_update_lock.unlock();

    if (m_param_tempo != 1.0f ||
	m_param_pitch != 1.0f)
	high_latency = true;

    if (base_factor < 0) {
	backwards = true;
	base_factor = -base_factor;
    }
    factor = base_factor;

    while(m_scaler.availible() < (high_latency ?
				  TIME_STRETCH_MIN_SAMPLES :
				  end - start)) {
	if (rate)
	    factor = base_factor + base_factor * rate_buf[(int) m_ctrl_pos];
	    
	if (backwards != m_fetcher.getBackwards())
	    m_fetcher.setBackwards(backwards);
	    
	if (factor < 0.2)
	    factor = 0.2;

	must_read = high_latency ? getInfo().block_size : SMALL_BLOCK_SIZE;
	if (factor * m_param_tempo < 1.0)
	    must_read = (float)must_read * factor * m_param_tempo;
	else
	    must_read = must_read;
	   
	m_update_lock.lock();
	nread = m_fetcher.read(m_inbuf, must_read);

	if (nread) {
	    m_ctrl_pos += (float) nread / (factor * m_param_tempo);
	    if (m_ctrl_pos >= getInfo().block_size)
		m_ctrl_pos = phase(m_ctrl_pos) + ((int) m_ctrl_pos % getInfo().block_size);

	    int old_availible = m_scaler.availible();

	    Sample inter_buffer[nread * m_inbuf.getInfo().num_channels];
	    m_inbuf.interleave(inter_buffer, nread);

	    m_scaler.setRate(factor);
	    
	    m_scaler.update(inter_buffer, nread);
	}


	
	m_update_lock.unlock();
    }

    Sample inter_buffer[(end - start) * m_inbuf.getInfo().num_channels];
    m_update_lock.lock();
    m_scaler.receive(inter_buffer, end - start);
    m_update_lock.unlock();
    buf.deinterleave(inter_buffer, start, end, m_scaler.getChannels());
}

void ObjectSampler::doAdvance()
{
}

void ObjectSampler::onInfoChange()
{
    m_scaler.setChannels(getInfo().num_channels);
}

} /* namespace psynth */
