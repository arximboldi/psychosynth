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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <OGRE/Ogre.h>

class Element {
	std::string m_id;
	
	Ogre::Entity* m_entity;
	Ogre::SceneNode* m_node;
	
	Ogre::Vector2 m_pos;
	Ogre::Degree m_angle;
	
	bool m_ghost;
	bool m_selected;
public:
	Element(Ogre::SceneManager* scene, std::string id, std::string mesh, Ogre::Vector2 pos,
			Ogre::Degree angle, bool phantom);
	virtual ~Element();
	
	bool isGhost() {
		return m_ghost;
	};
	
	bool isSelected() {
		return m_selected;
	};
	
	void setGhost(bool ghost);
	void setSelected(bool selected);
	
	Ogre::Vector2 getPosition() {
		return m_pos;
	}
	
	void setPosition(const Ogre::Vector2& pos) {
		m_pos = pos;
		m_node->setPosition(Ogre::Vector3(pos.x, 0, pos.y));
	};
	
	void rotate(Ogre::Degree angle) {
		m_angle += angle;
		m_node->yaw(angle);
	};
};

#endif /* ELEMENT_H */
