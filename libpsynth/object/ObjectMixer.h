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

#ifndef OBJECTMIXER_H
#define OBJECTMIXER_H

#include <libpsynth/object/Object.h>

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
	PARAM_AMPLITUDE,
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

    void init(Sample* dest, size_t n_samples);
    
private:
    float m_param_ampl;
    int m_param_mixop;    
    
public:
    ObjectMixer(const AudioInfo& info,
		int obj_type,
		int num_audio_out,
		int num_ctrl_out,
		int num_in = 2);
};

#endif /* OBJECTMIXER_H */
