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

#include <libpsynth/table/Table.h>

#include "gui3d/Element.h"
#include "gui3d/QueryFlags.h"
#include "gui3d/Connection.h"

const int N_MB = OIS::MB_Button7+1;

class ElementManager : public OIS::MouseListener,
		       public OIS::KeyListener,
		       public psynth::TableListener,
		       public psynth::TablePatcherListener
{
    typedef std::list<Element*> ElemList;
    typedef std::map<int, Element*> ElemMap; /* TODO: turn back into a list? */
    typedef ElemList::iterator ElemIter;
    typedef ElemMap::iterator ElemMapIter;
    
    psynth::Table* m_table;
    ElemList m_clear_elems;
    ElemMap m_elems;
    std::list<Connection*> m_cons;
    int m_elemcount;

    int m_must_own;
    
    Ogre::Camera* m_camera;
    Ogre::SceneManager* m_scene;
    Ogre::RaySceneQuery* m_rayquery;

    
    bool getTablePointer(Ogre::Vector2& res);
    Element* createElement(psynth::TableObject& obj);
    
public:
    ElementManager(psynth::Table* table,
		   Ogre::SceneManager* scene,
		   Ogre::Camera* camera);
    
    ~ElementManager();
    
    void update();
    
    void addElement(int e_type);
	
    bool mouseMoved(const OIS::MouseEvent& e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

    void handleAddObject(psynth::TableObject& obj);
    void handleDeleteObject(psynth::TableObject& obj);

    void handleLinkAdded(const psynth::TablePatcherEvent& ev);
    void handleLinkDeleted(const psynth::TablePatcherEvent& ev);
};

#endif /* ELEMENTMANAGER_H */
