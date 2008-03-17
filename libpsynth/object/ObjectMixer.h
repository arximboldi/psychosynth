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

#ifndef PSYNTH_OBJECTMIXER_H
#define PSYNTH_OBJECTMIXER_H

#include <libpsynth/object/Object.h>

namespace psynth
{

class ObjectMixer : public Object
{
public:
       /*enum {
      N_IN_A_SOCKETS
      };*/
	
    enum InControlSocketID {
	IN_C_AMPLITUDE,
	N_IN_C_SOCKETS
    };

    enum ParamID {
	PARAM_AMPLITUDE = Object::N_COMMON_PARAMS,
	PARAM_MIXOP,
	N_PARAM
    };

    enum MixOp {
	MIX_SUM,
	MIX_PRODUCT,
	N_MIXOPS
    };


protected:
    int m_numchan;

    void mix(Sample* dest, const Sample* src, size_t n_samples);
    
    void mix(Sample* dest, const Sample* src,
	     const Sample* ampl, size_t n_samples);

    void mix(Sample* dest, const Sample* src,
	     SimpleEnvelope& env, size_t n_samples);

    void mix(Sample* dest, const Sample* src, const Sample* ampl,
	     SimpleEnvelope& env, SimpleEnvelope& ctrl_env,
	     size_t n_samples);
    
    void init(Sample* dest, size_t n_samples);
    
private:
    float m_param_ampl;
    int m_param_mixop;    
    
public:
    ObjectMixer(const AudioInfo& info,
		int obj_type,
		const std::string& name,
		int num_audio_out,
		int num_ctrl_out,
		int num_in = 2);
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTMIXER_H */
