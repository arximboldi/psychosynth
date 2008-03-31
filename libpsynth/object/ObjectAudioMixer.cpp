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

#include "object/ObjectAudioMixer.h"

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectAudioMixer);

void ObjectAudioMixer::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    AudioBuffer* buf = getOutput<AudioBuffer>(LINK_AUDIO, OUT_A_OUTPUT);
    const AudioBuffer* in = NULL;
    const ControlBuffer* ampl = getInput<ControlBuffer>(LINK_CONTROL, IN_C_AMPLITUDE);
    int i, j;
    bool input = false;
    
    for (i = 0; i < getInfo().num_channels; ++i) {
	init(buf->getChannel(i), getInfo().block_size);

	for (j = 0; j < m_numchan; ++j)
	    if ((in = getInput<AudioBuffer>(LINK_AUDIO, j))) {
		EnvelopeSimple env = getInEnvelope(LINK_AUDIO, j);
		if (!ampl)
		    mix(buf->getChannel(i), in->getChannel(i),
			env, getInfo().block_size);
		else {
		    EnvelopeSimple ctrl_env = getInEnvelope(LINK_CONTROL,
							    IN_C_AMPLITUDE);
		    mix(buf->getChannel(i), in->getChannel(i), ampl->getData(),
			env, ctrl_env, getInfo().block_size);
		}
		input = true;
	    }

	if (!input)
	    memset(buf->getChannel(i), 0, sizeof(Sample) * getInfo().block_size);
    }
}

} /* namespace psynth */
