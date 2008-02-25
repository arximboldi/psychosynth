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

#ifndef PSYNTH_OBJECTAUDIOMIXER_H
#define PSYNTH_OBJECTAUDIOMIXER_H

#include <libpsynth/object/KnownObjects.h>
#include <libpsynth/object/ObjectMixer.h>

namespace psynth
{

class ObjectAudioMixer : public ObjectMixer
{
public:
    enum OutAudioSocketID {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

    enum OutControlSocketID {
	N_OUT_C_SOCKETS
    };
    
private:
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance() {}

public:  
    ObjectAudioMixer(const AudioInfo& prop, int n_chan = 2) :
	ObjectMixer(prop,
		    OBJ_MIXER,
		    N_OUT_A_SOCKETS,
		    N_OUT_C_SOCKETS,
		    n_chan)
	{}
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTAUDIOMIXER_H */
