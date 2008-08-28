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

#include <cstring>
#include <cstdlib>
#include "gui3d/conf_window.h"

using namespace psynth;
using namespace CEGUI;
using namespace std;

#define CW_WIDTH  300
#define CW_HEIGHT 250

#define PW_WIDTH  400
#define PW_HEIGHT 200

bool path_conf_window::on_generic (const CEGUI::EventArgs &e)
{
    return true;
}

bool path_conf_window::on_delete (const CEGUI::EventArgs &e)
{
    ItemEntry* item = m_listbox->getFirstSelectedItem();
    if (item)
	m_listbox->removeItem(item);
}

bool path_conf_window::on_add (const CEGUI::EventArgs &e)
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    ItemEntry* item = static_cast<ItemEntry*>(wmgr.createWindow("TaharezLook/ListboxItem"));
    item->setText(m_editbox->getText());
    item->setWantsMultiClickEvents(false);
    item->subscribeEvent(ItemEntry::EventMouseButtonDown,
			 Event::Subscriber(&path_conf_window::on_generic, this));
    
    m_listbox->addItem(item);
}

bool path_conf_window::on_change (const CEGUI::EventArgs &e)
{
    ItemEntry* item = m_listbox->getFirstSelectedItem();
    if (item)
	item->setText(m_editbox->getText());
}

bool path_conf_window::on_apply (const CEGUI::EventArgs &e)
{
    m_conf.clear_childs();

    for (int i = 0; i < m_listbox->getItemCount(); ++i) {
	string val = m_listbox->getItemFromIndex(i)->getText().c_str();
	m_conf.get_child(string("path") + itoa(i, 10)).set(val);
    }
    
    m_conf.nudge();
}

bool path_conf_window::on_selection (const CEGUI::EventArgs &e)
{
    ItemEntry* item = m_listbox->getFirstSelectedItem();
    if (item)
	m_editbox->setText(item->getText());
    return true;
}

void path_conf_window::populate ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    ItemEntry* item;
    std::string val;
    
    for (conf_node::iterator iter = m_conf.begin(); iter != m_conf.end(); ++iter) {
	(*iter)->get(val);
	item = static_cast<ItemEntry*>(wmgr.createWindow("TaharezLook/ListboxItem"));
	item->setText(val);
	item->setWantsMultiClickEvents(false);
	item->subscribeEvent(ItemEntry::EventMouseButtonDown,
			     Event::Subscriber(&path_conf_window::on_generic, this));
	
	m_listbox->addItem(item);
    }
}

CEGUI::FrameWindow* path_conf_window::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow"));
    window->setSize(UVector2(UDim(0, PW_WIDTH), UDim(0, PW_HEIGHT)));
    window->setPosition(UVector2(UDim(0.5, -PW_WIDTH/2.0), UDim(0.5, -PW_HEIGHT/2.0)));

    m_editbox = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    window->addChildWindow(m_editbox);
    m_editbox->setPosition(UVector2(UDim(0, 15), UDim(0, 33)));
    m_editbox->setSize(UVector2(UDim(1, -135), UDim(0, 20)));
    
    m_listbox = static_cast<ItemListbox*>(wmgr.createWindow("TaharezLook/ItemListbox"));
    window->addChildWindow(m_listbox);
    m_listbox->setPosition(UVector2(UDim(0, 15), UDim(0, 63)));
    m_listbox->setSize(UVector2(UDim(1, -135), UDim(1, -78)));
    m_listbox->subscribeEvent(ItemListbox::EventSelectionChanged,
			      Event::Subscriber(&path_conf_window::on_selection, this));

    Window* but = wmgr.createWindow("TaharezLook/Button");
    window->addChildWindow(but);
    but->setText("Add");
    but->setPosition(UVector2(UDim(1, -110), UDim(0, 33)));
    but->setSize(UVector2(UDim(0, 95), UDim(0, 20)));
    but->setWantsMultiClickEvents(false);
    but->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&path_conf_window::on_add, this));

    but = wmgr.createWindow("TaharezLook/Button");
    window->addChildWindow(but);
    but->setText("Change");
    but->setPosition(UVector2(UDim(1, -110), UDim(0, 63)));
    but->setSize(UVector2(UDim(0, 95), UDim(0, 20)));
    but->setWantsMultiClickEvents(false);
    but->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&path_conf_window::on_change, this));

    but = wmgr.createWindow("TaharezLook/Button");
    window->addChildWindow(but);
    but->setText("Delete");
    but->setPosition(UVector2(UDim(1, -110), UDim(0, 93)));
    but->setSize(UVector2(UDim(0, 95), UDim(0, 20)));
    but->setWantsMultiClickEvents(false);
    but->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&path_conf_window::on_delete, this));

    but = wmgr.createWindow("TaharezLook/Button");
    window->addChildWindow(but);
    but->setText("Refresh");
    but->setPosition(UVector2(UDim(1, -110), UDim(0, 123)));
    but->setSize(UVector2(UDim(0, 95), UDim(0, 20)));
    but->setWantsMultiClickEvents(false);
    but->subscribeEvent(PushButton::EventClicked,
			Event::Subscriber(&path_conf_window::on_apply, this));

    populate();
#if 0
 
#endif

    return window;
}

Window* output_conf_window_simple::create_window (psynth::conf_node& node)
{
    m_node = &node;
    
    WindowManager& wmgr = WindowManager::getSingleton();
    
    Window* window = wmgr.createWindow("DefaultGUISheet");

    window->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    window->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    Window* label = wmgr.createWindow("TaharezLook/StaticText");
    window->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    label->setSize(UVector2(UDim(0.5, -3), UDim(0, 20)));
    label->setText(m_name);

    string str_val;
    node.get_path(m_conf_path).get(str_val);

/*
  cout << "new device val ("<< node.getPath(m_conf_path).getPathName() << ": " << str_val << endl;
*/

    m_value = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    window->addChildWindow(m_value);
    m_value->setText(str_val);
    m_value->setPosition(UVector2(UDim(0.5, 3), UDim(0, 0)));
    m_value->setSize(UVector2(UDim(0.5, -3), UDim(0, 20)));

    return window;
}

void conf_window::start_output_conf_window (const std::string& out)
{
    ocwf_map::iterator i;
    i = m_map.find(out);

    if (i != m_map.end()) {
	m_out_win = i->second->create_output_conf_window ();
	m_out_cegui_win = m_out_win->create_window (m_psynth_conf.get_child (out));
    }
}

void conf_window::stop_output_conf_window()
{
    delete m_out_win;
    delete m_out_cegui_win;
}

CEGUI::Window* conf_window::create_audio_settings_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    Window* window = wmgr.createWindow("DefaultGUISheet");

    window->setText("Audio");
    window->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    window->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    Window* st = wmgr.createWindow("TaharezLook/StaticText");
    window->addChildWindow(st);
    st->setPosition(UVector2(UDim(0,10), UDim(0,10)));
    st->setSize(UVector2(UDim(1,-20), UDim(0,80)));
    st->setProperty("FrameEnabled", "true");
    st->setProperty("VertFormatting", "TopAligned");

    Window* label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 6), UDim(0, 6)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Sample rate:");

    int int_val;
    m_psynth_conf.get_child ("sample_rate").get(int_val);
    m_srate = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    st->addChildWindow(m_srate);
    m_srate->setPosition(UVector2(UDim(0.5, 3), UDim(0, 6)));
    m_srate->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    m_srate->setValidationString("\\d*");
    m_srate->setText(itoa(int_val, 10));
    
    label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 6), UDim(0, 30)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Buffer size:");

    m_psynth_conf.get_child ("block_size").get(int_val);
    m_bufsize = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    st->addChildWindow(m_bufsize);
    m_bufsize->setPosition(UVector2(UDim(0.5, 3), UDim(0, 30)));
    m_bufsize->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    m_bufsize->setValidationString("\\d*");
    m_bufsize->setText(itoa(int_val, 10));

    label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 6), UDim(0, 54)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Channels:");

    m_psynth_conf.get_child ("num_channels").get(int_val);
    m_channels = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    st->addChildWindow(m_channels);
    m_channels->setPosition(UVector2(UDim(0.5, 3), UDim(0, 54)));
    m_channels->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    m_channels->setValidationString("\\d*");
    m_channels->setText(itoa(int_val, 10));
 
    st = wmgr.createWindow("TaharezLook/StaticText");
    window->addChildWindow(st);
    st->setPosition(UVector2(UDim(0,10), UDim(0,98)));
    st->setSize(UVector2(UDim(1,-20), UDim(0,56)));
    st->setProperty("FrameEnabled", "true");
    st->setProperty("VertFormatting", "TopAligned");

    label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 6), UDim(0, 6)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Output:");

    string str_val;
    m_psynth_conf.get_child ("output").get(str_val);
    m_output = dynamic_cast<Combobox*>(wmgr.createWindow("TaharezLook/Combobox"));
    m_output->setPosition(UVector2(UDim(0.5, 3), UDim(0, 6)));
    m_output->setSize(UVector2(UDim(0.5, -9), UDim(0, 90)));
    m_output->setReadOnly(true);
    m_output->getDropList()->setClippedByParent(false);
    int i = 0;
    for (ocwf_map::iterator iter = m_map.begin();
	 iter != m_map.end(); ++iter) {
	ListboxItem* item = new ListboxTextItem(iter->first, i);
	m_output->addItem(item);
    }
    m_output->subscribeEvent(Combobox::EventListSelectionAccepted,
			     Event::Subscriber(&conf_window::on_output_change, this));
    m_output->setText(str_val);
    st->addChildWindow(m_output);
    m_output->setWantsMultiClickEvents(false);
    
    start_output_conf_window (str_val);
    if (m_out_cegui_win) {
	m_out_cegui_win->setPosition(UVector2(UDim(0, 6), UDim(0, 30)));
	m_out_cegui_win->setSize(UVector2(UDim(1, -12), UDim(0, 20)));
	st->addChildWindow(m_out_cegui_win);
    }
    
/*
  label = wmgr.createWindow("TaharezLook/StaticText");
  st->addChildWindow(label);
  label->setProperty("FrameEnabled", "false");
  label->setProperty("BackgroundEnabled", "false");
  label->setPosition(UVector2(UDim(0, 6), UDim(0, 30)));
  label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
  label->setText("Device:");
    
  m_bufsize = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
  st->addChildWindow(m_bufsize);
  m_bufsize->setPosition(UVector2(UDim(0.5, 3), UDim(0, 30)));
  m_bufsize->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
*/

    Window* but = wmgr.createWindow("TaharezLook/Button");
    window->addChildWindow(but);
    but->setText("Apply");
    but->setPosition(UVector2(UDim(1, -90), UDim(0, 160)));
    but->setSize(UVector2(UDim(0, 80), UDim(0, 20)));
    but->setWantsMultiClickEvents(false);

    but->subscribeEvent(
	PushButton::EventClicked, 
	Event::Subscriber(&conf_window::on_audio_apply_press, this));
    
    return window;
}

CEGUI::Window* conf_window::create_video_settings_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    Window* window = wmgr.createWindow("DefaultGUISheet");

    window->setText("Video");
    window->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    window->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    Window* st = wmgr.createWindow("TaharezLook/StaticText");
    window->addChildWindow(st);
    st->setPosition(UVector2(UDim(0,10), UDim(0,10)));
    st->setSize(UVector2(UDim(1,-20), UDim(0,78)));
    st->setProperty("FrameEnabled", "true");
    st->setProperty("VertFormatting", "TopAligned");

    Window* label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 6), UDim(0, 6)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Width:");

    int int_val;
    m_gui_conf.get_child ("screen_width").get(int_val);
    m_sc_width = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    st->addChildWindow(m_sc_width);
    m_sc_width->setPosition(UVector2(UDim(0, 6), UDim(0, 25)));
    m_sc_width->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    m_sc_width->setValidationString("\\d*");
    m_sc_width->setText(itoa(int_val, 10));
    
    label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0.5, 3), UDim(0, 6)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Height:");

    m_gui_conf.get_child ("screen_height").get(int_val);
    m_sc_height = static_cast<Editbox*>(wmgr.createWindow("TaharezLook/Editbox"));
    st->addChildWindow(m_sc_height);
    m_sc_height->setPosition(UVector2(UDim(0.5, 3), UDim(0, 25)));
    m_sc_height->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    m_sc_height->setValidationString("\\d*");
    m_sc_height->setText(itoa(int_val, 10));

    m_gui_conf.get_child ("fullscreen").get(int_val);
    m_fullscreen = static_cast<Checkbox*>(wmgr.createWindow("TaharezLook/Checkbox"));
    st->addChildWindow(m_fullscreen);
    m_fullscreen->setText("Fullscreen");
    m_fullscreen->setPosition(UVector2(UDim(0, 6), UDim(0, 52)));
    m_fullscreen->setSize(UVector2(UDim(0.5, 0), UDim(0, 20)));
    m_fullscreen->setWantsMultiClickEvents(false);
    m_fullscreen->setSelected(int_val);
    
    Window* but = wmgr.createWindow("TaharezLook/Button");
    st->addChildWindow(but);
    but->setText("Apply");
    but->setPosition(UVector2(UDim(1, -80), UDim(0, 52)));
    but->setSize(UVector2(UDim(0, 74), UDim(0, 20)));
    but->setWantsMultiClickEvents(false);
    but->subscribeEvent(PushButton::EventClicked, 
			Event::Subscriber(&conf_window::on_window_apply_press, this));

    m_gui_conf.get_child ("fps").get(int_val);
    m_fpslabel = wmgr.createWindow("TaharezLook/StaticText");
    window->addChildWindow(m_fpslabel);
    m_fpslabel->setProperty("FrameEnabled", "false");
    m_fpslabel->setProperty("BackgroundEnabled", "false");
    m_fpslabel->setPosition(UVector2(UDim(0, 10), UDim(0, 92)));
    m_fpslabel->setSize(UVector2(UDim(1, -20), UDim(0, 20)));
    m_fpslabel->setText(CEGUI::String("FPS: ") + itoa(int_val, 10));
    
    m_fps = static_cast<Scrollbar*>(wmgr.createWindow("TaharezLook/HorizontalScrollbar"));
    window->addChildWindow(m_fps);
    m_fps->setPosition(UVector2(UDim(0, 10), UDim(0, 116)));
    m_fps->setSize(UVector2(UDim(1, -20), UDim(0, 10)));
    m_fps->setWantsMultiClickEvents(false);
    m_fps->setDocumentSize(200);
    m_fps->setOverlapSize(0);
    m_fps->setPageSize(1);
    m_fps->setScrollPosition(int_val-20);
    m_fps->subscribeEvent(
	Scrollbar::EventScrollPositionChanged, 
	Event::Subscriber(&conf_window::on_fps_change, this));
    
    return window;
}

CEGUI::Window* conf_window::create_paths_settings_window()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    Window* window = wmgr.createWindow("DefaultGUISheet");

    window->setText("Paths");
    window->setPosition(UVector2(UDim(0, 0), UDim(0, 0)));
    window->setSize(UVector2(UDim(1, 0), UDim(1, 0)));

    Window* st = wmgr.createWindow("TaharezLook/StaticText");
    window->addChildWindow(st);
    st->setPosition(UVector2(UDim(0,10), UDim(0,10)));
    st->setSize(UVector2(UDim(1,-20), UDim(0,32)));
    st->setProperty("FrameEnabled", "true");
    st->setProperty("VertFormatting", "TopAligned");

    Window* label = wmgr.createWindow("TaharezLook/StaticText");
    st->addChildWindow(label);
    label->setProperty("FrameEnabled", "false");
    label->setProperty("BackgroundEnabled", "false");
    label->setPosition(UVector2(UDim(0, 6), UDim(0, 6)));
    label->setSize(UVector2(UDim(0.5, -9), UDim(0, 20)));
    label->setText("Samples:");
    
    Window* but = wmgr.createWindow("TaharezLook/Button");
    st->addChildWindow(but);
    but->setText("Edit");
    but->setPosition(UVector2(UDim(1, -80), UDim(0, 6)));
    but->setSize(UVector2(UDim(0, 74), UDim(0, 20)));
    //but->setWantsMultiClickEvents(false);
    but->subscribeEvent(
	PushButton::EventClicked,
	Event::Subscriber(&conf_window::on_samples_paths_press, this));
    
    return window;
}

bool conf_window::on_samples_paths_press (const CEGUI::EventArgs &e)
{
    m_samples_win.set_active (true);
    return true;
}

bool conf_window::on_generic (const CEGUI::EventArgs &e)
{
    return true;
}

bool conf_window::on_output_change (const CEGUI::EventArgs &e)
{
    string new_val = m_output->getText().c_str();
    m_psynth_conf.get_child ("output").set(new_val);

    Window* st = NULL;
    if (m_out_cegui_win) {
        st = m_out_cegui_win->getParent();
	st->removeChildWindow(m_out_cegui_win);
	delete m_out_cegui_win;
	delete m_out_win;
    }
    
    start_output_conf_window (new_val);
    if (m_out_cegui_win && st) {
	m_out_cegui_win->setPosition(UVector2(UDim(0, 6), UDim(0, 30)));
	m_out_cegui_win->setSize(UVector2(UDim(1, -12), UDim(0, 20)));
	st->addChildWindow(m_out_cegui_win);
    }
    
    return true;
}


bool conf_window::on_audio_apply_press (const CEGUI::EventArgs &e)
{
    int sr = atoi(m_srate->getText().c_str());
    int bs = atoi(m_bufsize->getText().c_str());
    int nc = atoi(m_channels->getText().c_str());
    string out = m_output->getText().c_str();
    
    m_psynth_conf.get_child ("sample_rate").set(sr);
    m_psynth_conf.get_child ("block_size").set(bs);
    m_psynth_conf.get_child ("num_channels").set(nc);
    m_psynth_conf.get_child ("output").set(out);
    if (m_out_win)
	m_out_win->apply();
    
    m_psynth_conf.nudge();
    
    return true;
}

bool conf_window::on_fps_change (const CEGUI::EventArgs &e)
{
    int val = m_fps->getScrollPosition() + 20;
    
    m_gui_conf.get_child ("fps").set(val);
    m_fpslabel->setText(CEGUI::String("FPS: ") + itoa(val, 10));
    
    return true;
}

bool conf_window::on_window_apply_press (const CEGUI::EventArgs &e)
{
    int sw = atoi(m_sc_width->getText().c_str());
    int sh = atoi(m_sc_height->getText().c_str());
    int fs = m_fullscreen->isSelected();

    m_gui_conf.get_child ("screen_width").set(sw);
    m_gui_conf.get_child ("screen_height").set(sh);
    m_gui_conf.get_child ("fullscreen").set(fs);

    m_gui_conf.nudge();
    
    return true;
}

CEGUI::FrameWindow* conf_window::create_window ()
{
    WindowManager& wmgr = WindowManager::getSingleton();
    
    FrameWindow* window = dynamic_cast<FrameWindow*>
	(wmgr.createWindow("TaharezLook/FrameWindow"));
	
    window->setPosition(UVector2(UDim(0.5, -CW_WIDTH/2), UDim(0.5, -CW_HEIGHT/2)));
    window->setSize    (UVector2(UDim(0, CW_WIDTH),UDim(0, CW_HEIGHT)));
    window->setText("Settings");
    
    Window* container = wmgr.createWindow("TaharezLook/TabControl");
    container->setPosition(UVector2(UDim(0, 10),  UDim(0, 30)));
    container->setSize    (UVector2(UDim(1, -20), UDim(1, -40)));

    window->addChildWindow (container);
    container->addChildWindow (create_audio_settings_window());
    container->addChildWindow (create_video_settings_window());
    container->addChildWindow (create_paths_settings_window());

    return window;
}

conf_window::conf_window (psynth::conf_node& gui_conf,
			  psynth::conf_node& psynth_conf) :
    m_samples_win(psynth_conf.get_path ("file_manager/samples")),
    m_gui_conf(gui_conf),
    m_psynth_conf(psynth_conf)
{
#ifdef PSYNTH_HAVE_ALSA
    attach_output_conf_window_factory (new output_conf_window_factory_alsa);
#endif
#ifdef PSYNTH_HAVE_OSS
    attach_output_conf_window_factory (new output_conf_window_factory_oss);
#endif
#ifdef PSYNTH_HAVE_JACK
    attach_output_conf_window_factory (new output_conf_window_factory_jack);
#endif
}

conf_window::~conf_window()
{
    stop_output_conf_window ();
    for (ocwf_map::iterator i = m_map.begin();
	 i != m_map.end(); ++i)
	delete i->second;
}
