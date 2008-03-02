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

#ifndef PSYNTH_OBJECTLFO_H
#define PSYNTH_OBJECTLFO_H

#include <libpsynth/object/ObjectOscillator.h>
#include <libpsynth/object/KnownObjects.h>

namespace psynth
{

class ObjectLFO : public ObjectOscillator
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
    ObjectLFO(const AudioInfo& prop) :
	ObjectOscillator(prop, OBJ_LFO, 0, N_OUT_C_SOCKETS)
	{};
};

} /* namespace psynth */

#endif /* PSYNTH_OBJECTLFO_H */
