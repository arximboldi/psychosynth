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

#ifndef SELECTORWINDOW_H
#define SELECTORWINDOW_H

#include <map>
#include <list>
#include <psynth/world/world_node_creator.hpp>

#include "gui3d/toggable_window.hpp"
#include "gui3d/element_manager.hpp"

class selector_window : public toggable_window
{
public:
    class category {
	class button {
	    element_manager* m_mgr;
	    CEGUI::Window* m_window;
	    int m_index;
	    psynth::world_node_creator m_creator;

	public:
	    button (const std::string& name, element_manager* m_mgr,
		   const psynth::world_node_creator& objcre, int index);

	    ~button () {
		//delete m_window;
		//delete m_creator;
	    }

	    CEGUI::Window* getWindow() {
		return m_window;
	    };

	    void set_position ();

	    bool on_click (const CEGUI::EventArgs &e) {
		m_mgr->add_element (m_creator);
		return true;
	    }

	    bool on_parent_resize (const CEGUI::EventArgs &e) {
		set_position ();
		return true;
	    }
	};

	std::list<button*> m_buts;
	CEGUI::Window* m_window;
	element_manager* m_mgr;
	int m_nbut;

    public:
	category (const std::string& name, element_manager* mgr);

	~category ();

	CEGUI::Window* get_window () {
	    return m_window;
	};

	void clear_buttons ();
	void add_button (const std::string& name, const psynth::world_node_creator& objcre);
    };

private:
    std::map<std::string, category*> m_cat;
    CEGUI::Window* m_window;
    CEGUI::Window* m_container;
    element_manager* m_mgr;

    CEGUI::FrameWindow* create_window ();

public:
    selector_window (element_manager* emgr);
    category* add_category (const std::string& name);
    category* find_category (const std::string& name);
    virtual ~selector_window();
};

#endif /* SELECTORWINDOW_H */
