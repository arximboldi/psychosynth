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

#ifndef OGREMISC_H
#define OGREMISC_H

#include <string>

enum LineRelPos {
    LP_LEFT,
    LP_RIGHT,
    LP_INT,
    LP_EXT
};

bool pointIsInPoly(const Ogre::Vector2& p,
		   const Ogre::Vector2& l1,
		   const Ogre::Vector2& l2,
		   const Ogre::Vector2& l3,
		   const Ogre::Vector2& l4);

LineRelPos pointLinePosition(const Ogre::Vector2& l1,
			     const Ogre::Vector2& l2,
			     const Ogre::Vector2& p);

inline Ogre::Vector3 yawVector3(const Ogre::Vector3& v, Ogre::Real angle)
{
    Ogre::Real cosa = cos(angle);
    Ogre::Real sina = sin(angle);
    
    return Ogre::Vector3(cosa * v.x - sina * v.z,
		    v.y,
		    sina * v.x + cosa * v.z);    
}

inline Ogre::Vector2 yawVector2(const Ogre::Vector2& v, Ogre::Real angle)
{
    Ogre::Real cosa = cos(angle);
    Ogre::Real sina = sin(angle);
    
    return Ogre::Vector2(cosa * v.x - sina * v.y,
			 sina * v.x + cosa * v.y);    
}

Ogre::MaterialPtr createColourMaterial(const std::string& name,
				       const Ogre::ColourValue& colour);

#endif /* OGREMISC_H */
