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

#include "gui3d/ElementOscillator.h"
#include "gui3d/ElemMainComponent.h"
#include "gui3d/ElemSecondComponent.h"
#include "object/ObjectOscillator.h"

using namespace std;
using namespace Ogre;

ElementOscillator::ElementOscillator(const TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* names[ObjectOscillator::N_OSC_TYPES] =
	{"oscsine.mesh", "oscsine.mesh", "oscsine.mesh", "oscsine.mesh"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectOscillator::PARAM_FREQUENCY, 20.0f, 5000.0f,
		     ObjectOscillator::PARAM_WAVE, names));
    
    addComponent(new ElemSecondComponent(
		     ObjectOscillator::PARAM_AMPLITUDE, 0.0f, 1.0f));
}
