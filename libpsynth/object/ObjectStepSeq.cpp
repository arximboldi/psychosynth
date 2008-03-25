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

#include "object/KnownObjects.h"
#include "object/ObjectStepSeq.h"

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectStepSeq);

ObjectStepSeq::ObjectStepSeq(const AudioInfo& info) :
    Object(info,
	   OBJ_STEPSEQ,
	   "step_sequencer",
	   N_IN_A_SOCKETS,
	   N_IN_C_SOCKETS,
	   N_OUT_A_SOCKETS,
	   N_OUT_C_SOCKETS)
{
}

void ObjectStepSeq::doUpdate(const Object* caller,
			     int caller_port_type,
			     int caller_port)
{
}

void ObjectStepSeq::doAdvance()
{
}

void ObjectStepSeq::onInfoChange()
{
}

}
