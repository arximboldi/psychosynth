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
#include <libpsynth/table/TableObjectCreator.h>

#include "gui3d/ToggableWindow.h"
#include "gui3d/ElementManager.h"

class SelectorWindow : public ToggableWindow
{	
public:
    class Category {
	class Button {
	    ElementManager* m_mgr;
	    CEGUI::Window* m_window;
	    int m_index;
	    psynth::TableObjectCreator m_creator;
	    
	public:
	    Button(const std::string& name, ElementManager* m_mgr,
		   const psynth::TableObjectCreator& objcre, int index);
			
	    ~Button() {
		//delete m_window;
		//delete m_creator;
	    }
			
	    CEGUI::Window* getWindow() {
		return m_window;
	    };
			
	    void setPosition();
			
	    bool onClick(const CEGUI::EventArgs &e) {
		m_mgr->addElement(m_creator);
		return true;
	    }
			
	    bool onParentResize(const CEGUI::EventArgs &e) {
		setPosition();
		return true;
	    }
	};
		
	std::list<Button*> m_buts;
	CEGUI::Window* m_window;
	ElementManager* m_mgr;
	int m_nbut;
		
    public:
	Category(const std::string& name, ElementManager* mgr);
		
	~Category();
		
	CEGUI::Window* getWindow() {
	    return m_window;
	};
		
	void addButton(const std::string& name, const psynth::TableObjectCreator& objcre);
    };
	
private:	
    std::map<std::string, Category*> m_cat;
    CEGUI::Window* m_window;
    CEGUI::Window* m_container;
    ElementManager* m_mgr;
	
    CEGUI::FrameWindow* createWindow();

public:	
    SelectorWindow(ElementManager* emgr);
    Category* addCategory(const std::string& name);
    Category* findCategory(const std::string& name);
    virtual ~SelectorWindow();
};

#endif /* SELECTORWINDOW_H */
