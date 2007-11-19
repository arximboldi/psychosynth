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

#include "gui3d/ElementMixer.h"
#include "gui3d/ElemMainComponent.h"
#include "object/ObjectMixer.h"

using namespace std;
using namespace Ogre;

ElementMixer::ElementMixer(const TableObject& obj,
			   Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{ 
    addComponent(new ElemMainComponent("oscsine.mesh",
				       ObjectMixer::PARAM_AMPLITUDE, 0.0f, 1.0f));   
}
