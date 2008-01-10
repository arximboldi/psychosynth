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

#ifndef FLATRING_H
#define FLATRING_H

#include <OGRE/Ogre.h>
#include "gui3d/OgreMisc.h"


class FlatRing : public Ogre::ManualObject
{
    Ogre::Degree m_startangle;
    Ogre::Degree m_endangle;
    Ogre::Real m_innerrad;
    Ogre::Real m_outerrad;
    Ogre::ColourValue m_colour;
    int m_numtrig;
	
    void build();

public:
    FlatRing(const Ogre::String& id,
	     Ogre::Degree start_angle = Ogre::Degree(0.0), 
	     Ogre::Degree end_angle = Ogre::Degree(360.0),
	     Ogre::Real inner_radious = 0.5,
	     Ogre::Real outer_radious = 1,
	     Ogre::ColourValue colour = Ogre::ColourValue(),
	     int num_triangles = 20);
	
    ~FlatRing();
	
    void setStartAngle(Ogre::Degree start_angle) {
	m_startangle = start_angle;
    };
	
    void setEndAngle(Ogre::Degree end_angle) {
	m_endangle = end_angle;
    };
	
    void setInnerRadious(Ogre::Real inner_radious) {
	m_innerrad = inner_radious;
    };
	
    void setOuterRadious(Ogre::Real outer_radious) {
	m_outerrad = outer_radious;
    };
	
    void setColour(Ogre::ColourValue colour) {
	m_colour = colour;
	createColourMaterial(getName(), m_colour);
    };
	
    void setTriangles(int numtrig) {
	m_numtrig = numtrig;
    };
	
    Ogre::Degree getStartAngle() const {
	return m_startangle;
    };
	
    Ogre::Degree getEndAngle() const {
	return m_endangle;
    };
	
    Ogre::Real getInnerRadious() const {
	return m_outerrad;
    };
	
    Ogre::Real getOuterRadious() const {
	return m_outerrad;
    };
	
    Ogre::ColourValue getColour() const {
	return m_colour;
    };
	
    int getTriangles() const {
	return m_numtrig;
    }
	
    void update();
};

#endif /* FLATRING_H */
