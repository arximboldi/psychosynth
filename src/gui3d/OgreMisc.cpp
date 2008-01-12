
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

#include "OGRE/Ogre.h"

#include "gui3d/OgreMisc.h"

using namespace Ogre;
using namespace std;


bool pointIsInPoly(const Ogre::Vector2& p,
		   const Ogre::Vector2& l1,
		   const Ogre::Vector2& l2,
		   const Ogre::Vector2& l3,
		   const Ogre::Vector2& l4)
{
    LineRelPos a = pointLinePosition(l1, l2, p);
    LineRelPos b = pointLinePosition(l2, l3, p);
     if (a != b)
	return false;
    LineRelPos c = pointLinePosition(l3, l4, p);
    if (b != c)
	return false;
    LineRelPos d = pointLinePosition(l4, l1, p);
    if (c != d)
	return false;

    return true;
}

LineRelPos pointLinePosition(const Ogre::Vector2& a,
			     const Ogre::Vector2& b,
			     const Ogre::Vector2& p)
{
    Real s;
	
    /* Calculamos el área del triángulo que forman, según sea positivo o
     * negativo podremos decir si el triángulo iba en sentido horario o
     * antihorario. Si el área es cero no existía triángulo, son colineales,
     * y por tanto sólo quedaría constatar si el punto está en el segmento
     * o no. El área lo calculamos al cuadrado porque es más rápido. */
    s = (a.x * b.y - a.y * b.x +
	 b.x * p.y - b.y * p.x +
	 p.x * a.y - p.y * a.x);
	
    if (s > 0)
	return LP_RIGHT;
    else if (s < 0)
	return LP_LEFT;
    else if (a.x < b.x) {
	if (p.x <= a.x || p.x >= b.x)
	    return LP_EXT;
	else
	    return LP_INT;
    } else {
	if (p.x <= b.x || p.x >= a.x)
	    return LP_EXT;
	else
	    return LP_INT;
    }
}

MaterialPtr createColourMaterial(const std::string& name,
				 const ColourValue&  colour)
{
    MaterialPtr matptr = 
	MaterialManager::getSingleton().createOrRetrieve(name, "General").first; 
	
    matptr->setReceiveShadows(true);
    Pass* pass = matptr->getTechnique(0)->getPass(0);
    pass->setDiffuse(colour);
    pass->setAmbient(colour);
    pass->setSpecular(colour);
    pass->setSelfIllumination(colour);
    //pass->setEmissive(ColourValue(0,0,0,colour.a)); 
    pass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
    pass->setDepthWriteEnabled(false);

    return matptr;
}
