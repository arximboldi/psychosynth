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

#ifndef OBJECTCONTROLMIXER_H
#define OBJECTCONTROLMIXER_H

#include <libpsynth/object/KnownObjects.h>
#include <libpsynth/object/ObjectMixer.h>

class ObjectControlMixer : public ObjectMixer
{
public:
    enum OutAudioSocketID {
	N_OUT_A_SOCKETS
    };

    enum OutControlSocketID {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };
    
private:
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance() {}

public:  
    ObjectControlMixer(const AudioInfo& prop, int n_chan = 2) :
	ObjectMixer(prop,
		    OBJ_CONTROLMIXER,
		    N_OUT_A_SOCKETS,
		    N_OUT_C_SOCKETS,
		    n_chan)
	{}
};

#endif /* OBJECTCONTROLMIXER_H */
