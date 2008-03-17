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

#ifndef THEELEMENTS_H
#define THEELEMENTS_H

#include "gui3d/Element.h"

class ElementMixer : public Element
{  
public:
    ElementMixer(psynth::TableObject& obj,
		 Ogre::SceneManager* m_scene);
};

class ElementControlMixer : public Element
{  
public:
    ElementControlMixer(psynth::TableObject& obj,
			Ogre::SceneManager* m_scene);
};

class ElementOscillator : public Element
{
public:
    ElementOscillator(psynth::TableObject& obj,
		      Ogre::SceneManager* m_scene);
};

class ElementLFO : public Element
{
public:
    ElementLFO(psynth::TableObject& obj,
	       Ogre::SceneManager* m_scene);
};

class ElementFilter : public Element
{
public:
    ElementFilter(psynth::TableObject& obj,
		  Ogre::SceneManager* m_scene);
};

class ElementSampler : public Element
{
public:
    ElementSampler(psynth::TableObject& obj,
		   Ogre::SceneManager* m_scene);
};

#endif /* THEELEMENTS_H */
