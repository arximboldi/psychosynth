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

#include "object/ObjectFilter.h"
#include "object/KnownObjects.h"

using namespace std;

namespace psynth
{

ObjectFilter::ObjectFilter(const AudioInfo& prop, int mode) : 
    Object(prop,
	   OBJ_FILTER,
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS),
    m_param_type(mode),
    m_param_cutoff(DEFAULT_CUTOFF),
    m_param_resonance(DEFAULT_RESONANCE),
    m_filter_values((FilterValues::Type)m_param_type,
		    m_param_cutoff,
		    m_param_resonance,
		    prop.sample_rate),
    m_filter(prop.num_channels, Filter(&m_filter_values))
{
    addParam("type", ObjParam::INT, &m_param_type);
    addParam("cutoff", ObjParam::FLOAT, &m_param_cutoff);
    addParam("resonance", ObjParam::FLOAT, &m_param_resonance);
}

ObjectFilter::~ObjectFilter()
{
}

void ObjectFilter::doUpdate(const Object* caller, int caller_port_type, int caller_por)
{
    const AudioBuffer* input = getInput<AudioBuffer>(LINK_AUDIO, IN_A_INPUT);
    const ControlBuffer* cutoff = getInput<ControlBuffer>(LINK_CONTROL, IN_C_CUTOFF);
    AudioBuffer* output = getOutput<AudioBuffer>(LINK_AUDIO, IN_A_INPUT);    

    if (input) {
	if (m_param_type != m_filter_values.getType() ||
	    m_param_cutoff != m_filter_values.getFrequency() ||
	    m_param_resonance != m_filter_values.getResonance())
	    m_filter_values.calculate((FilterValues::Type)m_param_type,
				      m_param_cutoff,
				      m_param_resonance,
				      getAudioInfo().sample_rate);
	
	for (int i = 0; i < getAudioInfo().num_channels; ++i) {
	    Sample* outbuf = output->getChannel(i);
	    const Sample* inbuf = input->getChannel(i);
	    Filter& filter = m_filter[i];

	    if (!cutoff)
		for (size_t j = 0; j < output->size(); ++j)
		    *outbuf++ = filter.update(*inbuf++);
	    else {
		const Sample* cutoff_buf = cutoff->getData();
		
		for (size_t j = 0; j < output->size(); ++j) {
		    /* FIXME: Slow */
		    m_filter_values.calculate(m_param_cutoff
					      + *cutoff_buf++ * m_param_cutoff);
		    *outbuf++ = filter.update(*inbuf++);
		}
	    }
	}
    } else {
	output->zero();
    }
}

} /* namespace psynth */
