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

#ifndef PSYNTH_OBJECTNOISE_H
#define PSYNTH_OBJECTNOISE_H

#include <libpsynth/object/Object.h>

namespace psynth
{

class ObjectNoise : public Object
{		
public:	
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_AMPLITUDE,
	IN_C_TRIGGER,
	N_IN_C_SOCKETS
    };

    enum Type {
	NOISE_WHITE,
	NOISE_PINK,
	N_TYPES
    };
    
    enum ParamID {
	PARAM_TYPE = Object::N_COMMON_PARAMS,
	PARAM_AMPLITUDE,
	N_PARAM
    };

    static const float DEFAULT_AMPL = 0.3f;
    
protected:
    Sample updatePink();
    Sample updateWhite();
    void updateNoise(Sample* buf);
    
private:
    /* Pink noise factors. */
    Sample m_b0;
    Sample m_b1;
    Sample m_b2;
    Sample m_b3;
    Sample m_b4;
    Sample m_b5;
    Sample m_b6;
    

    int   m_param_type;
    float m_param_ampl;
    //bool  m_restart;
    
public:
    ObjectNoise(const AudioInfo& prop,
		int obj_type,
		const std::string& name,
		int n_audio_out,
		int n_control_out);
    
    ~ObjectNoise();
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTNOISE_H */

