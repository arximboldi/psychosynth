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
#include <OIS/OIS.h>

#include <libpsynth/world/world.h>

#include "gui3d/flat_ring.h"
#include "gui3d/element_properties.h"


class element;

class elem_component
{
    element* m_parent;
    Ogre::SceneNode* m_node;
    
public:    
    virtual ~elem_component() {};
    virtual void init() = 0;
    
    virtual void handle_param_change (psynth::world_node& obj, int id) = 0;
    virtual bool handle_pointer_move (Ogre::Vector2 pos) = 0;
    virtual bool handle_pointer_click (Ogre::Vector2 pos, OIS::MouseButtonID id) = 0;
    virtual bool handle_pointer_release (Ogre::Vector2 pos, OIS::MouseButtonID id) = 0;
    
    void set_scene_node (Ogre::SceneNode* node) {
	m_node = node;
    }

    inline void update_visibility ();
    
    Ogre::SceneNode* get_scene_node() {
	return m_node;
    }

    void set_parent (element* parent) {
	m_parent = parent;
    }
    
    element* get_parent () {
	return m_parent;
    };
};

class element : public psynth::world_node_listener
{
    typedef std::list<elem_component*>::iterator elem_component_iter;
    std::list<elem_component*> m_comp;

    /*
      std::list<Connection*> m_src_con;
      std::list<Connection*> m_dest_con;
    */
    
    psynth::world_node m_obj;
    psynth::world_node m_target;
    
    Ogre::ColourValue   m_col_ghost;
    Ogre::ColourValue   m_col_selected;
    Ogre::ColourValue   m_col_normal;
    Ogre::SceneManager* m_scene;
    Ogre::SceneNode*    m_node;

    flat_ring*          m_base;
    
    Ogre::Vector3 m_aimpoint;
    Ogre::Vector2 m_click_diff;
    Ogre::Vector2 m_pos;

    bool m_owned;
    bool m_ghost;
    bool m_selected;
    bool m_moving;

    int m_modifier_1;
    int m_modifier_2;
    
    element_properties m_gui_prop;

    void node_moved (psynth::world_node& pos,
		     psynth::vector_2f& dest);

public:
    static const Ogre::Real RADIOUS = 1.0f;
    static const Ogre::Real Z_POS = 0.001f;
    
    element (psynth::world_node& obj, Ogre::SceneManager* scene);
    
    virtual ~element();

    void add_component (elem_component* comp);

    void set_target (const psynth::world_node& obj);
    void clear_target (const psynth::world_node& obj);
    void set_ghost (bool ghost);
    void set_selected (bool selected);
    void set_position (const Ogre::Vector2& pos);

    void set_first_modifier (int val) {
	m_modifier_1 = val;
    }

    void set_second_modifier (int val) {
	m_modifier_2 = val;
    }
    
    bool pointer_clicked (const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointer_released (const Ogre::Vector2& pos, OIS::MouseButtonID id);
    bool pointer_moved (const Ogre::Vector2& pos);
    bool key_pressed (const OIS::KeyEvent& e);
    bool key_released (const OIS::KeyEvent& e);
    
    void handle_activate_node (psynth::world_node& obj);
    void handle_deactivate_node (psynth::world_node& obj);
    void handle_set_param_node (psynth::world_node& ob,
				int param_id);
    
    bool is_ghost () const {
	return m_ghost;
    };
	
    bool is_selected () const {
	return m_selected;
    };

    bool is_owned () const {
	return m_owned;
    };
    
    Ogre::Vector2 get_position () {
	return m_pos;
    }

    void set_owned (bool owned) {
	m_owned = owned;
    }
    
    Ogre::SceneManager* get_scene () {
	return m_scene;
    }

    psynth::world_node& get_node () {
	return m_obj;
    }

    element_properties& get_gui_properties() {
	return m_gui_prop;
    }
    /*
      void addSourceConnection(Connection* con) {
      m_src_con.push_back(con);
      };

      void addDestinyConnection(Connection* con) {
      m_dest_con.push_back(con);
      };

      void removeSourceConnection(Connection* con) {
      m_src_con.remove(con);
      }

      void removeDestinyConnection(Connection* con) {
      m_dest_con.remove(con);
      }
    */
};

inline void elem_component::update_visibility ()
{
    m_node->setVisible (!m_parent->is_ghost ());
}

#endif /* ELEMENT_H */
