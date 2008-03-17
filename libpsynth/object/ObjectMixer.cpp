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

using namespace std;

namespace psynth
{

ObjectMixer::ObjectMixer(const AudioInfo& info,
			 int obj_type,
			 const std::string& name,
			 int num_audio_out,
			 int num_ctrl_out,
			 int num_in): 
    Object(info,
	   obj_type,
	   name,
	   num_audio_out ? num_in : 0, 
	   num_ctrl_out  ? num_in : N_IN_C_SOCKETS,
	   num_audio_out,
	   num_ctrl_out),
    m_numchan(num_in),
    m_param_ampl(0.5f),
    m_param_mixop(MIX_SUM)
{
    addParam("amplitude", ObjParam::FLOAT, &m_param_ampl);
    addParam("mixop", ObjParam::INT, &m_param_mixop);
}

void ObjectMixer::mix(Sample* dest, const Sample* src, size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while(n_samples--)
	    *dest++ += *src++ * m_param_ampl;
    else if (m_param_mixop == MIX_PRODUCT)
	while(n_samples--)
	    *dest++ *= *src++ * m_param_ampl;
}
  
void ObjectMixer::mix(Sample* dest, const Sample* src,
		      const Sample* ampl, size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while(n_samples--)
	    *dest++ += *src++ * (m_param_ampl + m_param_ampl * *ampl++);
    else if (m_param_mixop == MIX_PRODUCT)
	while(n_samples--)
	    *dest++ *= *src++ * (m_param_ampl + m_param_ampl * *ampl++);
}

void ObjectMixer::mix(Sample* dest, const Sample* src,
		      SimpleEnvelope& env, size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while(n_samples--)
	    *dest++ += *src++ * m_param_ampl * env.update();
    else if (m_param_mixop == MIX_PRODUCT)
	while(n_samples--)
	    *dest++ *= *src++ * m_param_ampl * env.update();
}
  
void ObjectMixer::mix(Sample* dest, const Sample* src,
		      const Sample* ampl,
		      SimpleEnvelope& env,
		      SimpleEnvelope& ctrl_env,
		      size_t n_samples)
{
    if (m_param_mixop == MIX_SUM)
	while(n_samples--)
	    *dest++ +=
		*src++ * (m_param_ampl + m_param_ampl * *ampl++ * ctrl_env.update()) * env.update();
    else if (m_param_mixop == MIX_PRODUCT)
	while(n_samples--)
	    *dest++ *=
		*src++ * (m_param_ampl + m_param_ampl * *ampl++ * ctrl_env.update()) * env.update();
}

void ObjectMixer::init(Sample* dest, size_t n_samples)
{
    float def_val = (m_param_mixop == MIX_SUM ? 0.0 : 1.0);
    
    while(n_samples--)
	*dest++ = def_val;
}

} /* namespace psynth */
