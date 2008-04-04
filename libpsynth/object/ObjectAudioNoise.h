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

#ifndef PSYNTH_OBJECTAUDIONOISE_H
#define PSYNTH_OBJECTAUDIONOISE_H

#include <libpsynth/object/ObjectNoise.h>
#include <libpsynth/object/KnownObjects.h>
#include <libpsynth/object/ObjectFactory.h>

namespace psynth
{

class ObjectAudioNoise : public ObjectNoise
{
public:
    enum OutAudioSocketID {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

private:
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance() {}
    void onInfoChange() {}
    
public:
    ObjectAudioNoise(const AudioInfo& prop) :
	ObjectNoise(prop,
		    OBJ_AUDIONOISE,
		    "audio_noise",
		    N_OUT_A_SOCKETS,
		    0)
	{};
};

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectAudioNoise, "audio_noise");

} /* namespace psynth */

#endif /* PSYNTH_OBJECTAUDIONOISE_H */
