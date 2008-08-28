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

#include "gui3d/flat_ring.h"
#include "gui3d/ogre_misc.h"

using namespace std;
using namespace Ogre;

void flat_ring::build()
{
    int n_steps = m_numtrig;
    Real angle = m_startangle.valueRadians();
    Real step = (angle - m_endangle.valueRadians()) / n_steps;
    int i;

    if (step < 0)
	for (i = 0; i <= n_steps; i++) {
	    position (m_innerrad * Math::Cos(angle), 0, m_innerrad * Math::Sin(angle));
	    position (m_outerrad * Math::Cos(angle), 0, m_outerrad * Math::Sin(angle));
	    angle += step;
	}
    else
	for (i = 0; i <= n_steps; i++) {
	    position (m_outerrad * Math::Cos(angle), 0, m_outerrad * Math::Sin(angle));
	    position (m_innerrad * Math::Cos(angle), 0, m_innerrad * Math::Sin(angle));
	    angle += step;
	}
}

flat_ring::~flat_ring()
{
    //clear();
    Ogre::MaterialManager::getSingleton().remove(getName());
}

flat_ring::flat_ring(const Ogre::String& id,
		   Ogre::Degree start_angle, Ogre::Degree end_angle,
		   Ogre::Real inner_radious, Ogre::Real outer_radious,
		   Ogre::ColourValue colour, int num_triangles) :
    ManualObject(id),
    m_startangle(start_angle),
    m_endangle(end_angle),
    m_innerrad(inner_radious),
    m_outerrad(outer_radious),
    m_colour(colour),
    m_numtrig(num_triangles)
{
    create_colour_material (getName(), m_colour);
    begin(id, RenderOperation::OT_TRIANGLE_STRIP);
    build();
    end();
    setDynamic(false);
}

void flat_ring::update()
{
    beginUpdate (0);
    build();
    end();
}

