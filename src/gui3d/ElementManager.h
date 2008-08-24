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

#include <libpsynth/world/world.h>
#include <libpsynth/world/world_node_creator.h>

#include "gui3d/Element.h"
#include "gui3d/QueryFlags.h"
#include "gui3d/Connection.h"

const int N_MB = OIS::MB_Button7+1;

class ElementManager : public OIS::MouseListener,
		       public OIS::KeyListener,
		       public psynth::world_listener,
		       public psynth::world_patcher_listener
{
    typedef std::list<Element*> ElemList;
    typedef std::map<int, Element*> ElemMap; /* TODO: turn back into a list? */
    typedef ElemList::iterator ElemIter;
    typedef ElemMap::iterator ElemMapIter;
    
    psynth::world* m_world;
    psynth::world_node m_adding;
    ElemList m_clear_elems;
    ElemMap m_elems;
    std::list<Connection*> m_cons;
    int m_elemcount;

    int m_must_own;
    int m_modifier_1;
    int m_modifier_2;
    
    Ogre::Camera* m_camera;
    Ogre::SceneManager* m_scene;
    Ogre::RaySceneQuery* m_rayquery;

    
    bool getWorldPointer(Ogre::Vector2& res);
    Element* createElement(psynth::world_node& obj);
    
public:
    ElementManager (psynth::world* world,
		    Ogre::SceneManager* scene,
		    Ogre::Camera* camera);
    
    ~ElementManager();
    
    void update();

    /*
    void addElement(int e_type);
    */

    void addElement(psynth::world_node_creator& obj);
    
    bool mouseMoved(const OIS::MouseEvent& e);
    bool mousePressed(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    bool mouseReleased(const OIS::MouseEvent &e, OIS::MouseButtonID id);
    
    bool keyPressed(const OIS::KeyEvent &e);
    bool keyReleased(const OIS::KeyEvent &e);

    void handle_add_node (psynth::world_node& obj);
    void handle_delete_node (psynth::world_node& obj);

    void handle_link_added (const psynth::world_patcher_event& ev);
    void handle_link_deleted (const psynth::world_patcher_event& ev);
};

#endif /* ELEMENTMANAGER_H */
