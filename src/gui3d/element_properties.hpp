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

#ifndef ELEMENTPROPERTIES_H
#define ELEMENTPROPERTIES_H

#include <map>

#include "gui3d/toggable_window.hpp"
#include <psynth/world/world.hpp>

class element_properties;

class elem_gui_param
{
    friend class element_properties;

    CEGUI::Window* m_window;
    element_properties* m_parent;
    int m_param;
    bool changed;

public:
    elem_gui_param (int param) :
	m_param(param) {};

    virtual ~elem_gui_param () {};

    virtual void create_gui () = 0;
    virtual void handle_param_change (psynth::world_node& obj, int id) = 0;

    void init (element_properties* parent, CEGUI::Window* window) {
	m_window = window;
	m_parent = parent;
	create_gui ();
    }

    int get_param () {
	return m_param;
    }

    element_properties* get_parent () {
	return m_parent;
    }

    CEGUI::Window* get_parent_window () {
	return m_window;
    };
};

class elem_gui_param_float : public elem_gui_param
{
    CEGUI::Window*  m_label;
    CEGUI::Spinner* m_spinner;
    float m_min_val;
    float m_max_val;
    std::string m_name;
    int m_skip;

public:
    elem_gui_param_float (int param, float min_val, float max_val,
			  const std::string& name);

    void create_gui ();
    void handle_param_change (psynth::world_node& obj, int param);
    bool on_spinner_change (const CEGUI::EventArgs &e);
};

class elem_gui_param_int : public elem_gui_param
{
    CEGUI::Window*  m_label;
    CEGUI::Spinner* m_spinner;
    int m_min_val;
    int m_max_val;
    std::string m_name;
    int m_skip;

public:
    elem_gui_param_int (int param, int min_val, int max_val,
			const std::string& name);

    void create_gui ();
    void handle_param_change (psynth::world_node& obj, int param);
    bool on_spinner_change (const CEGUI::EventArgs &e);
};

class elem_gui_param_multi : public elem_gui_param
{
    CEGUI::Window*  m_label;
    CEGUI::Combobox* m_selector;
    const char** m_op_names;

    int m_nval;
    std::string m_name;
    int m_skip;

public:
    elem_gui_param_multi (int param,
			  int nval, const char** names,
		      const std::string& name);

    void create_gui ();
    void handle_param_change (psynth::world_node& obj, int param);
    bool on_combobox_change (const CEGUI::EventArgs &e);
};

class element_properties : public toggable_window,
			   public psynth::world_node_listener
{
    std::map<int, elem_gui_param*> m_params;
    psynth::world_node m_obj;

    CEGUI::Window* m_container;
    float m_y_offset;

    CEGUI::FrameWindow* create_window ();

public:
    element_properties (const psynth::world_node& obj) :
	m_obj(obj),
	m_y_offset(0) {
	m_obj.add_listener (this);
    };

    ~element_properties () {
	m_obj.delete_listener (this);
    }

    void add_parameter (elem_gui_param* e);
    void handle_set_param_node (psynth::world_node& ob, int param_id);

    void handle_activate_node (psynth::world_node& obj) {};
    void handle_deactivate_node (psynth::world_node& obj) {};

    psynth::world_node& get_node () {
	return m_obj;
    }
};

#endif /* ELEMENTPROPERTIES_H */
