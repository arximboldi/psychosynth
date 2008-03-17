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

#include "object/ObjectControlMixer.h"

namespace psynth
{

PSYNTH_DEFINE_OBJECT_FACTORY(ObjectControlMixer);

void ObjectControlMixer::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    ControlBuffer* buf = getOutput<ControlBuffer>(LINK_CONTROL, OUT_C_OUTPUT);
    const ControlBuffer* in = NULL;
    int j;

    init(buf->getData(), getInfo().block_size);
    
    for (j = 0; j < m_numchan; ++j)
	if ((in = getInput<ControlBuffer>(LINK_CONTROL, j))) {
	    SimpleEnvelope env = getInEnvelope(LINK_CONTROL, j);
	    mix(buf->getData(), in->getData(), env, getInfo().block_size);
	}
}

} /* namespace psynth */

