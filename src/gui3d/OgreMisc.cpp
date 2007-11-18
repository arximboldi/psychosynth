
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
