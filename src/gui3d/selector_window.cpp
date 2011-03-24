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

#include <algorithm>
#include "gui3d/selector_window.hpp"

using namespace std;
using namespace CEGUI;

const float BUT_XGAP = 8;
const float BUT_YGAP = 8;
const float BUT_WIDTH = 100;
const float BUT_HEIGHT = 20;

selector_window::category::button::button (const std::string& name, 
					   element_manager* mgr,
					   const psynth::world_node_creator& objcre,
					   int index) :
    m_mgr(mgr),
    m_index(index),
    m_creator(objcre)
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    m_window = wmgr.createWindow("TaharezLook/Button");
    m_window->setText (name);
    m_window->setSize ( UVector2(CEGUI::UDim(0, BUT_WIDTH),
                                 CEGUI::UDim(0, BUT_HEIGHT)));

    m_window->setMousePassThroughEnabled (false);
    //m_window->setWantsMultiClickEvents(false);
    m_window->setEnabled(true);
    m_window->subscribeEvent(
	PushButton::EventClicked,
	Event::Subscriber(&selector_window::category::button::on_click, this));
    m_window->subscribeEvent(
	Window::EventParentSized,
	Event::Subscriber(&selector_window::category::button::on_parent_resize, this));
}

void selector_window::category::button::set_position ()
{
    float x, y;
    int xindex, yindex, butsinrow;
    float maxw = m_window->getParentPixelWidth() - 5;
    
    butsinrow = int(maxw) / int(BUT_WIDTH + BUT_XGAP);
    if (butsinrow < 1)
	butsinrow = 1;
		
    yindex = m_index / butsinrow;
    xindex = m_index % butsinrow;
	
    y = (yindex) * BUT_YGAP + yindex * BUT_HEIGHT;
    x = (xindex) * BUT_XGAP + xindex * BUT_WIDTH;

    m_window->setPosition(UVector2(UDim(0, x), UDim(0, y)));
}

selector_window::category::category (const std::string& name, element_manager* mgr) :
    m_mgr(mgr),
    m_nbut(0)
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    m_window = wmgr.createWindow("TaharezLook/ScrollablePane");
    m_window->setPosition( UVector2(UDim(0, BUT_XGAP),    UDim(0, BUT_YGAP)) );
    m_window->setSize    ( UVector2(UDim(1, -2*BUT_XGAP), UDim(1, -2*BUT_YGAP)) );
    m_window->setText    (name);
    m_window->setWantsMultiClickEvents(false);
}

selector_window::category::~category ()
{
    list<button*>::iterator i;
    for (i = m_buts.begin(); i != m_buts.end(); i++)
	delete *i;
	
    //delete m_window;
}

void selector_window::category::clear_buttons ()
{
    list<button*>::iterator it;
    for (it = m_buts.begin(); it != m_buts.end(); ++it) {
	m_window->removeChildWindow((*it)->getWindow());
	delete *it;
    }
    
    m_buts.clear();
    m_nbut = 0;
}

void selector_window::category::add_button (const std::string& name,
					    const psynth::world_node_creator& objcre)
{
    button* but = new button (name, m_mgr, objcre, m_nbut++);
    m_buts.push_back(but);

    m_window->addChildWindow( but->getWindow() );
	
    but->set_position ();
}

CEGUI::FrameWindow* selector_window::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
	
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow", "window_selector"));
	
    //window->setPosition(UVector2(UDim(0, 10), UDim(0, 10)) );
    //window->setSize    ( UVector2(UDim(1, -20),UDim(0, 100)) );
    window->setPosition( UVector2(UDim(0, 10), UDim(0, 10)) );
    window->setSize    ( UVector2(UDim(1, -20),UDim(0, 134)) );
    window->setText("Object Selector");
	
    m_container = wmgr.createWindow("TaharezLook/TabControl", "container_selector");
    m_container->setPosition( UVector2(UDim(0, 10), UDim(0, 30)) );
    m_container->setSize    ( UVector2(UDim(1, -20),     UDim(1, -40)) );
	
    window->addChildWindow(m_container);
	
    return window;
}

selector_window::category* selector_window::add_category (const std::string& name)
{
    if (m_cat[name] != NULL)
	return NULL;

    category* newcat = new category (name, m_mgr);
    m_container->addChildWindow (newcat->get_window ());

    m_cat[name] = newcat;
	
    return newcat;
}

selector_window::category* selector_window::find_category (const std::string& name)
{
    map<string,category*>::iterator i = m_cat.find(name);
	
    if (i == m_cat.end())
	return NULL;
	
    return (*i).second;
}

selector_window::selector_window (element_manager* emgr) :
    m_mgr(emgr)
{
    set_active (false); // Force creation of the window;
}

selector_window::~selector_window ()
{
    map<string,category*>::iterator i;
    for (i = m_cat.begin(); i != m_cat.end(); i++)
	delete (*i).second;
}
