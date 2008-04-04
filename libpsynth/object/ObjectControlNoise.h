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

#ifndef PSYNTH_OBJECTCONTROLNOISE_H
#define PSYNTH_OBJECTCONTROLNOISE_H

#include <libpsynth/object/ObjectNoise.h>
#include <libpsynth/object/KnownObjects.h>
#include <libpsynth/object/ObjectFactory.h>

namespace psynth
{

class ObjectControlNoise : public ObjectNoise
{
public:	
    enum OutControlSocketID {
	OUT_C_OUTPUT,
	N_OUT_C_SOCKETS
    };

private:
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance() {}
    void onInfoChange() {}
    
public:
    ObjectControlNoise(const AudioInfo& prop) :
	ObjectNoise(prop,
			 OBJ_CONTROLNOISE,
			 "control_noise",
			 0,
			 N_OUT_C_SOCKETS)
	{};
};

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectControlNoise, "control_noise");

} /* namespace psynth */

#endif /* PSYNTH_OBJECTCONTROLNOISE_H */

