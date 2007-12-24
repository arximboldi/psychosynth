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

#ifndef OBJECTMIXER_H
#define OBJECTMIXER_H

#include "object/Object.h"

class ObjectMixer : public Object
{
    float m_param_ampl;
    
    int m_numchan;
	
    void mix(AudioBuffer* dest, const AudioBuffer* src,
	     const ControlBuffer* ampl);
    void doUpdate();
public:
    /*enum {
      N_IN_A_SOCKETS
      };*/
	
    enum InControlSocketID {
	IN_C_AMPLITUDE,
	N_IN_C_SOCKETS
    };
	
    enum OutAudioSocketID {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };
	
    enum OutControlSocketID {
	N_OUT_C_SOCKETS
    };

    enum ParamID {
	PARAM_AMPLITUDE,
	N_PARAM
    };
    
    ObjectMixer(const AudioInfo& info, int num_chan = 4);
};

#endif /* OBJECTMIXER_H */
