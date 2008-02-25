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

#include "object/ObjectLFO.h"

using namespace std;

namespace psynth
{

void ObjectLFO::doUpdate(const Object* caller, int caller_port_type, int caller_port)
{
    ControlBuffer*       buf = getOutput<ControlBuffer>(LINK_CONTROL, OUT_C_OUTPUT);
    const ControlBuffer* pitch_buf = getInput<ControlBuffer>(LINK_CONTROL, IN_C_FREQUENCY);

    Sample*       out = buf->getData();
    const Sample* mod = pitch_buf ? pitch_buf->getData() : NULL;

    updateOscParams();

    if (!mod)
	m_oscillator.update(out, getInfo().block_size);
    else
	m_oscillator.update(out, mod, getInfo().block_size);
}

} /* namespace psynth */
