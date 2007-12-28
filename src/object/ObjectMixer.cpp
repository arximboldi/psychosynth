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
#include "object/ObjectMixer.h"

ObjectMixer::ObjectMixer(const AudioInfo& prop, int numchan) : 
    Object(prop,
	   OBJ_MIXER,
	   N_PARAM,
	   numchan, 
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_ampl(0.5f),
    m_numchan(numchan)
{
    configureLocalParam(PARAM_AMPLITUDE, Object::PARAM_FLOAT, &m_param_ampl);
}

void ObjectMixer::mix(AudioBuffer* dest, const AudioBuffer* src,
		      const ControlBuffer* ampl)
{
    int i, j;
    int channels = getAudioInfo().num_channels;
    int size = getAudioInfo().block_size;
    Sample* dbuf;
    const Sample* sbuf;
    const Sample* abuf = NULL;
    
    for (i = 0; i < channels; i++) {
	dbuf = dest->getChannel(i);
	sbuf = src->getChannel(i);
	if (ampl)
	    abuf = ampl->getData();
	
	if (!abuf)
	    for (j = 0; j < size; j++)
		*dbuf++ += *sbuf++ * m_param_ampl;
	else
	    for (j = 0; j < size; j++)
		*dbuf++ += *sbuf++ * (m_param_ampl + m_param_ampl * *abuf++);
    }
}

void ObjectMixer::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    AudioBuffer* buf = getOutput<AudioBuffer>(LINK_AUDIO, OUT_A_OUTPUT);
    const AudioBuffer* in = NULL;
    const ControlBuffer* ampl = getInput<ControlBuffer>(LINK_CONTROL, IN_C_AMPLITUDE);
    int i;
	
    buf->clear();
	
    for (i = 0; i < m_numchan; ++i)
	if ((in = getInput<AudioBuffer>(LINK_AUDIO, i)))
	    mix (buf, in, ampl);
}
