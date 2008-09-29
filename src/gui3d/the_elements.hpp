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

#include "gui3d/element.hpp"

class element_mixer : public element
{  
public:
    element_mixer (psynth::world_node& obj,
		   Ogre::SceneManager* m_scene);
};

class element_control_mixer : public element
{  
public:
    element_control_mixer (psynth::world_node& obj,
			Ogre::SceneManager* m_scene);
};

class element_oscillator : public element
{
public:
    element_oscillator (psynth::world_node& obj,
			Ogre::SceneManager* m_scene);
};

class element_lfo : public element
{
public:
    element_lfo (psynth::world_node& obj,
		 Ogre::SceneManager* m_scene);
};

class element_filter : public element
{
public:
    element_filter (psynth::world_node& obj,
		    Ogre::SceneManager* m_scene);
};

class element_sampler : public element
{
public:
    element_sampler (psynth::world_node& obj,
		     Ogre::SceneManager* m_scene);
};

class element_step_seq : public element
{
public:
    element_step_seq (psynth::world_node& obj,
		      Ogre::SceneManager* m_scene);
};

class element_audio_noise : public element
{
public:
    element_audio_noise (psynth::world_node& obj,
			 Ogre::SceneManager* m_scene);
};

class element_control_noise : public element
{
public:
    element_control_noise (psynth::world_node& obj,
			   Ogre::SceneManager* m_scene);
};

class element_echo : public element
{
public:
    element_echo (psynth::world_node& obj,
		  Ogre::SceneManager* m_scene);
};

class element_delay : public element
{
public:
    element_delay (psynth::world_node& obj,
		   Ogre::SceneManager* m_scene);
};

#endif /* THEELEMENTS_H */
