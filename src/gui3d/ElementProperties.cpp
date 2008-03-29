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

#include "gui3d/ElementProperties.h"

#define ELEMENT_EDITOR_WIDTH  200.0f
#define ELEMENT_EDITOR_HEIGHT 30.0f
#define PARAMETER_HEIGHT 20.0f

using namespace std;
using namespace CEGUI;
using namespace psynth;

ElemGuiParamMulti::ElemGuiParamMulti(int param, int nval, const char** names,
				     const std::string& name) :
    ElemGuiParam(param),
    m_op_names(names),
    m_nval(nval),
    m_name(name),
    m_skip(0)
{
}

void ElemGuiParamMulti::createGUI()
{
    WindowManager& wmgr = WindowManager::getSingleton();

    m_label = dynamic_cast<Window*>(wmgr.createWindow("TaharezLook/StaticText"));
    m_label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    m_label->setSize(UVector2(UDim(0.5, -5), UDim(0, 20)));
    m_label->setText(m_name);

    m_selector = dynamic_cast<Combobox*>(wmgr.createWindow("TaharezLook/Combobox"));
    m_selector->setPosition(UVector2(UDim(0.5, 0), UDim(0, 0)));
    m_selector->setSize(UVector2(UDim(0.5, 0), UDim(0, 30 + 18 * (m_nval+1))));
    m_selector->getDropList()->setClippedByParent(false);
    m_selector->setReadOnly(true);
    m_selector->setWantsMultiClickEvents(false);
    for (int i = 0; i < m_nval; ++i) {
	ListboxItem* item = new ListboxTextItem(m_op_names[i], i);
	m_selector->addItem(item);
    }

    m_selector->subscribeEvent(Combobox::EventListSelectionAccepted, 
			       Event::Subscriber(&ElemGuiParamMulti::onComboboxChange, this));
    
    int value;
    getParent()->getObject().getParam(getParam(), value);
    m_skip++;
    m_selector->setText(m_op_names[value]);
    m_skip--;
    
    getParentWindow()->addChildWindow(m_label);
    getParentWindow()->addChildWindow(m_selector);
}

void ElemGuiParamMulti::handleParamChange(TableObject& obj, int param)
{
    int new_val;

    obj.getParam(param, new_val);

    m_skip++;
    m_selector->setText(m_op_names[new_val]);
    m_skip--;
}

bool ElemGuiParamMulti::onComboboxChange(const CEGUI::EventArgs &e)
{
    if (!m_skip) {
	String new_val = m_selector->getText();
	
	for (int i = 0; i < m_nval; ++i)
	    if (new_val == m_op_names[i]) {
		getParent()->getObject().setParam(getParam(), i);
		break;
	    }
    }
    
    return true;
}

ElemGuiParamFloat::ElemGuiParamFloat(int param, float min_val, float max_val,
				     const std::string& name) :
    ElemGuiParam(param),
    m_min_val(min_val),
    m_max_val(max_val),
    m_name(name),
    m_skip(0)
{
}


void ElemGuiParamFloat::createGUI()
{
    WindowManager& wmgr = WindowManager::getSingleton();

    m_label = dynamic_cast<Window*>(wmgr.createWindow("TaharezLook/StaticText"));
    m_label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    m_label->setSize(UVector2(UDim(0.5, -5), UDim(0, 20)));
    m_label->setText(m_name);

    m_spinner = dynamic_cast<Spinner*>(wmgr.createWindow("TaharezLook/Spinner"));
    m_spinner->setPosition(UVector2(UDim(0.5, 0), UDim(0, 0)));
    m_spinner->setSize(UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_spinner->setMaximumValue(m_max_val);
    m_spinner->setMinimumValue(m_min_val);
    m_spinner->setTextInputMode(Spinner::FloatingPoint);
    m_spinner->subscribeEvent(Spinner::EventValueChanged, 
			      Event::Subscriber(&ElemGuiParamFloat::onSpinnerChange, this));
    
    float value;
    getParent()->getObject().getParam(getParam(), value);
    m_skip++;
    m_spinner->setCurrentValue(value);
    m_skip--;
    
    getParentWindow()->addChildWindow(m_label);
    getParentWindow()->addChildWindow(m_spinner);
}

void ElemGuiParamFloat::handleParamChange(TableObject& obj, int param)
{
    float new_val;

    obj.getParam(param, new_val);
    m_skip++;
    m_spinner->setCurrentValue(new_val);
    m_skip--;
}

bool ElemGuiParamFloat::onSpinnerChange(const CEGUI::EventArgs &e)
{
    if (!m_skip) {
	float new_val = m_spinner->getCurrentValue();
	getParent()->getObject().setParam(getParam(), new_val);
    }
    
    return true;
}

ElemGuiParamInt::ElemGuiParamInt(int param, int min_val, int max_val,
				     const std::string& name) :
    ElemGuiParam(param),
    m_min_val(min_val),
    m_max_val(max_val),
    m_name(name),
    m_skip(0)
{
}

void ElemGuiParamInt::createGUI()
{
    WindowManager& wmgr = WindowManager::getSingleton();

    m_label = dynamic_cast<Window*>(wmgr.createWindow("TaharezLook/StaticText"));
    m_label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    m_label->setSize(UVector2(UDim(0.5, -5), UDim(0, 20)));
    m_label->setText(m_name);

    m_spinner = dynamic_cast<Spinner*>(wmgr.createWindow("TaharezLook/Spinner"));
    m_spinner->setPosition(UVector2(UDim(0.5, 0), UDim(0, 0)));
    m_spinner->setSize(UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_spinner->setMaximumValue(m_max_val);
    m_spinner->setMinimumValue(m_min_val);
    m_spinner->setTextInputMode(Spinner::Integer);
    m_spinner->subscribeEvent(Spinner::EventValueChanged, 
			      Event::Subscriber(&ElemGuiParamInt::onSpinnerChange, this));
    
    int value;
    getParent()->getObject().getParam(getParam(), value);
    m_skip++;
    m_spinner->setCurrentValue(value);
    m_skip--;
    
    getParentWindow()->addChildWindow(m_label);
    getParentWindow()->addChildWindow(m_spinner);
}

void ElemGuiParamInt::handleParamChange(TableObject& obj, int param)
{
    int new_val;

    obj.getParam(param, new_val);
    m_skip++;
    m_spinner->setCurrentValue(new_val);
    m_skip--;
}

bool ElemGuiParamInt::onSpinnerChange(const CEGUI::EventArgs &e)
{
    if (!m_skip) {
	int new_val = m_spinner->getCurrentValue();
	getParent()->getObject().setParam(getParam(), new_val);
    }
    
    return true;
}

void ElementProperties::handleSetParamObject(TableObject& obj, int param_id)
{
    if (m_params.find(param_id) != m_params.end())
	m_params[param_id]->handleParamChange(obj, param_id);
}

FrameWindow* ElementProperties::createWindow()
{
    WindowManager& wmgr = WindowManager::getSingleton();

    FrameWindow* window;
    window = dynamic_cast<FrameWindow*>(wmgr.createWindow("TaharezLook/FrameWindow"));
    
    window->setPosition(UVector2(UDim(0.06, 0), UDim(0.2, 0)));
    window->setSize(UVector2(UDim(0, 200), UDim(0, 40)));
    window->setText("Properties");

    m_container = dynamic_cast<Window*>(wmgr.createWindow("TaharezLook/ScrollablePane"));

    m_container->setPosition(UVector2(UDim(0, 20), UDim(0, 35)));
    m_container->setSize(UVector2(UDim(1, -30), UDim(1, -40)));

    window->addChildWindow(m_container);

    return window;
}

void ElementProperties::addParameter(ElemGuiParam* e)
{
    WindowManager& wmgr = WindowManager::getSingleton();

    Window *window;
    window = dynamic_cast<Window*>(wmgr.createWindow("DefaultGUISheet"));
    window->setPosition(UVector2(UDim(0, 0), UDim(0, m_y_offset)));
    window->setSize(UVector2(UDim(1, -10), UDim(1, - m_y_offset)));
    m_y_offset += 25;

    getWindow()->setHeight(UDim(0, 40 + m_y_offset));
    
    m_container->addChildWindow(window);
    
    e->init(this, window);
    m_params[e->getParam()] = e;
}
