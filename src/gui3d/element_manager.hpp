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

#include <psynth/world/world.hpp>
#include <psynth/world/world_node_creator.hpp>

#include "gui3d/element.hpp"
#include "gui3d/query_flags.hpp"
#include "gui3d/connection.hpp"

const int N_MB = OIS::MB_Button7+1;

class element_manager : public OIS::MouseListener,
                        public OIS::KeyListener,
                        public psynth::world_listener,
                        public psynth::world_patcher_listener
{
    typedef std::list<element*> elem_list;
    typedef std::map<int, element*> elem_map; /* TODO: turn back into a list? */
    typedef elem_list::iterator elem_iter;
    typedef elem_map::iterator elem_map_iter;

    psynth::world* m_world;
    psynth::world_node m_adding;
    elem_list m_clear_elems;
    elem_map m_elems;
    std::list<connection*> m_cons;
    int m_elemcount;

    int m_must_own;
    int m_modifier_1;
    int m_modifier_2;

    Ogre::Camera* m_camera;
    Ogre::SceneManager* m_scene;
    Ogre::RaySceneQuery* m_rayquery;


    bool get_world_pointer (Ogre::Vector2& res);
    element* create_element (psynth::world_node& obj);

public:
    element_manager (psynth::world* world,
		    Ogre::SceneManager* scene,
		    Ogre::Camera* camera);

    ~element_manager ();

    void update ();

    /*
    void add_element(int e_type);
    */

    void add_element (psynth::world_node_creator& obj);

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
