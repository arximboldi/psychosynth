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

#include "gui3d/ToggableWindow.h"
#include "table/Table.h"

class ElementProperties;

class ElemGuiParam {
    friend class ElementProperties;

    CEGUI::Window* m_window;
    ElementProperties* m_parent;
    int m_param;
    bool changed;
    
public:
    ElemGuiParam(int param) :
	m_param(param) {};
    
    virtual ~ElemGuiParam() {};

    virtual void createGUI() = 0;
    virtual void handleParamChange(TableObject& obj, int param) = 0;

    void init(ElementProperties* parent, CEGUI::Window* window) {
	m_window = window;
	m_parent = parent;
	createGUI();
    }
    
    int getParam() {
	return m_param;
    }

    ElementProperties* getParent() {
	return m_parent;
    }

    CEGUI::Window* getParentWindow() {
	return m_window;
    };
};

class ElemGuiParamFloat : public ElemGuiParam {
    CEGUI::Window*  m_label;
    CEGUI::Spinner* m_spinner;
    float m_min_val;
    float m_max_val;
    std::string m_name;
    int m_skip;
    
public:
    ElemGuiParamFloat(int param, float min_val, float max_val, const std::string& name);

    void createGUI();
    void handleParamChange(TableObject& obj, int param);
    bool onSpinnerChange(const CEGUI::EventArgs &e);
};

class ElemGuiParamMulti : public ElemGuiParam {
    CEGUI::Window*  m_label;
    CEGUI::Combobox* m_selector;
    const char** m_op_names;
    
    int m_nval;
    std::string m_name;
    int m_skip;
    
public:
    ElemGuiParamMulti(int param, int nval, const char** names,
		      const std::string& name);

    void createGUI();
    void handleParamChange(TableObject& obj, int param);
    bool onComboboxChange(const CEGUI::EventArgs &e);
};

class ElementProperties : public ToggableWindow,
			  public TableObjectListener
{
    std::map<int, ElemGuiParam*> m_params;
    TableObject m_obj;
    
    CEGUI::Window* m_container;
    float m_y_offset;
    
    CEGUI::FrameWindow* createWindow();

public:
    ElementProperties(const TableObject& obj) :
	m_obj(obj),
	m_y_offset(0) {
	m_obj.addListener(this);
    };

    ~ElementProperties() {
	m_obj.deleteListener(this);
    }

    void addParameter(ElemGuiParam* e);
    void handleSetParamObject(TableObject& ob, int param_id);
    
    void handleMoveObject(TableObject& obj) {};
    void handleActivateObject(TableObject& obj) {};
    void handleDeactivateObject(TableObject& obj) {};
    
    TableObject& getObject() {
	return m_obj;
    }
};



#endif /* ELEMENTPROPERTIES_H */
