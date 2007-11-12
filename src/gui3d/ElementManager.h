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

#ifndef ELEMENTMANAGER_H
#define ELEMENTMANAGER_H

#include <map>
#include <OIS/OIS.h>
#include "gui3d/Element.h"
#include "gui3d/QueryFlags.h"
#include "table/Table.h"

const int N_MB = OIS::MB_Button7+1;

class ElementManager : public OIS::MouseListener, public OIS::KeyListener
{
	typedef std::map<std::string,Element*> ElemMap;
	typedef ElemMap::iterator  ElemIter;
	
	Table* m_table;
	ElemMap m_elems;
	ElemIter m_addelem;
	ElemIter m_selected;
	
	int m_elemcount;
	
	Ogre::Camera* m_camera;
	
	Ogre::SceneManager* m_scene;
	Ogre::RaySceneQuery* m_rayquery;
	
	bool m_pressed[N_MB];
	
	void stopAdding();
	void add();
	void select(const std::string& name);
	void unselect();
	bool isAdding() { return m_addelem != m_elems.end(); }
	bool isSelected() { return m_selected != m_elems.end(); }
	void move(Element* elem);

public:
	ElementManager(Table* table, Ogre::SceneManager* scene, Ogre::Camera* camera);
	
	void addElement(int e_type);
	
	bool mouseMoved(const OIS::MouseEvent& e);
	bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
	bool keyPressed(const OIS::KeyEvent &e);
	bool keyReleased(const OIS::KeyEvent &e);
};

#endif /* ELEMENTMANAGER_H */
