/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#include "table/Table.h"

#include "node/node_output.h"
#include "node/node_audio_mixer.h"
#include "node/node_control_mixer.h"
#include "node/node_audio_oscillator.h"
#include "node/node_lfo.h"
#include "node/node_filter.h"
#include "node/node_sampler.h"
#include "node/node_step_seq.h"
#include "node/node_audio_noise.h"
#include "node/node_control_noise.h"
#include "node/node_echo.h"
#include "node/node_delay.h"

using namespace std;

namespace psynth
{

void TableSubject::notifyAddObject(TableObject& obj)
{
    m_obj_listeners[obj.getID()];
    for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	(*i)->handleAddObject(obj);
}

void TableSubject::notifyDeleteObject(TableObject& obj)
{
    m_obj_listeners.erase(obj.getID());
    for (ListenerIter i = m_listeners.begin(); i != m_listeners.end(); i++)
	(*i)->handleDeleteObject(obj);
}
    
void TableSubject::notifyActivateObject(TableObject& obj)
{
    for (ObjectListenerIter i = m_all_obj_list.begin(); i != m_all_obj_list.end(); i++)
	(*i)->handleActivateObject(obj);
    
    std::map<int, std::list<TableObjectListener*> >::iterator it;
    if ((it = m_obj_listeners.find(obj.getID())) != m_obj_listeners.end()) {
	for (ObjectListenerIter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handleActivateObject(obj);
    }
}
    
void TableSubject::notifyDeactivateObject(TableObject& obj)
{
    for (ObjectListenerIter i = m_all_obj_list.begin(); i != m_all_obj_list.end(); i++)
	(*i)->handleDeactivateObject(obj);
    
    std::map<int, std::list<TableObjectListener*> >::iterator it;
    if ((it = m_obj_listeners.find(obj.getID())) != m_obj_listeners.end()) {
	for (ObjectListenerIter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handleDeactivateObject(obj);
    }
}

void TableSubject::notifySetParamObject(TableObject& obj, int param_id)
{
    for (ObjectListenerIter i = m_all_obj_list.begin(); i != m_all_obj_list.end(); i++)
	(*i)->handleSetParamObject(obj, param_id);
    
    std::map<int, std::list<TableObjectListener*> >::iterator it;
    if ((it = m_obj_listeners.find(obj.getID())) != m_obj_listeners.end()) {
	for (ObjectListenerIter i = it->second.begin(); i != it->second.end(); i++)
	    (*i)->handleSetParamObject(obj, param_id);	
    }
}

void TableSubject::notifyLinkAdded(const TablePatcherEvent& ev)
{
    for (list<TablePatcherListener*>::iterator i = m_patch_list.begin();
	 i != m_patch_list.end();)
	(*i++)->handleLinkAdded(ev);
}

void TableSubject::notifyLinkDeleted(const TablePatcherEvent& ev)
{
    for (list<TablePatcherListener*>::iterator i = m_patch_list.begin();
	 i != m_patch_list.end();)
	(*i++)->handleLinkDeleted(ev);
}

Table::Table(const audio_info& info) :
    m_info(info),
    m_patcher(NULL),
    m_last_id(MIN_USER_ID)
{
    m_output = new node_output(m_info);
    m_mixer = new node_audio_mixer(m_info, MIXER_CHANNELS);

    m_mixer->param("amplitude").set(0.5f);

    m_node_mgr.attach_node (m_output, OUTPUT_ID);
    m_node_mgr.attach_node (m_mixer, MIXER_ID);

    registerDefaultObjectFactory();
}

void Table::registerDefaultObjectFactory()
{
    registerObjectFactory(get_node_audio_mixer_factory());
    registerObjectFactory(get_node_control_mixer_factory());
    registerObjectFactory(get_node_audio_oscillator_factory());
    registerObjectFactory(get_node_lfo_factory());
    registerObjectFactory(get_node_output_factory());
    registerObjectFactory(get_node_filter_factory());
    registerObjectFactory(get_node_sampler_factory());
    registerObjectFactory(get_node_step_seq_factory());
    registerObjectFactory(get_node_audio_noise_factory());
    registerObjectFactory(get_node_control_noise_factory());
    registerObjectFactory(get_node_echo_factory());
    registerObjectFactory(get_node_delay_factory());
}

Table::~Table()
{
    delete m_patcher;
}

void
Table::clear()
{
    for (node_manager::iterator it = m_node_mgr.begin();
	 it != m_node_mgr.end();)
	if ((*it)->get_id () >= MIN_USER_ID) {
	    if (m_patcher)
		m_patcher->deleteNode(*it);
	    TableObject obj(*it, this);
	    notifyDeleteObject(obj);
	    //m_node_mgr.remove(it++);
	    m_node_mgr.delete_node (it);
	} else
	    ++it;
}

TableObject Table::findObject(int id)
{
    node_manager::iterator i = m_node_mgr.find (id);
    if (i == m_node_mgr.end())
	return TableObject(NULL, NULL);
    else
	return TableObject(*i, this);
}

#if 0
TableObject Table::addObject(int type)
{
    node* nod;

    switch (type) {
    case OBJ_OSCILLATOR:
	obj = new node_audio_oscillator(m_info);
	break;
    case OBJ_LFO:
	obj = new node_lfo(m_info);
	break;
    case OBJ_FILTER:
	obj = new node_filter(m_info);
	break;
    case OBJ_MIXER:
	obj = new node_audio_mixer(m_info);
	break;
    case OBJ_CONTROLMIXER:
	obj = new node_control_mixer (m_info);
	break;
    case OBJ_SAMPLER:
	obj = new node_sampler(m_info);
	break;
    default:
	obj = NULL;
	return TableObject(NULL, NULL);
    }
    
    if (!m_node_mgr.attachObject(obj, m_last_id++))
	return TableObject(NULL, NULL);

    TableObject tobj(obj, this);
    notifyAddObject(tobj);
    return tobj;
}
#endif

TableObject Table::addObject(const std::string& name)
{
    node* obj;
    TableObject tobj;
	
    obj = m_objfact.create(name, m_info);

    if (obj) {
	if (!m_node_mgr.attach_node (obj, m_last_id++))
	    return TableObject(NULL, NULL);
    
	tobj = TableObject(obj, this);
	notifyAddObject(tobj);
    }
    
    return tobj;
}

void Table::deleteObject(TableObject& obj)
{
    if (m_patcher)
	m_patcher->deleteNode(obj.m_obj);
    notifyDeleteObject(obj);
/*
  m_node_mgr.detachObject(obj.m_obj->get_id ());
  delete obj.m_obj;
*/
    m_node_mgr.delete_node (obj.m_obj->get_id ());
}

void Table::activateObject(TableObject& obj)
{
    /* HACK */
    obj.m_obj->update_params_in ();
     
    if (m_patcher)
	m_patcher->addNode (obj.m_obj);
    notifyActivateObject (obj);
}

void Table::deactivateObject(TableObject& obj)
{
    if (m_patcher)
	m_patcher->deleteNode (obj.m_obj);
    notifyDeactivateObject(obj);
}

void Table::attachPatcher(Patcher* pat)
{
    detachPatcher();
    m_patcher = pat;
    pat->addListener(this);
    for (node_manager::iterator it = m_node_mgr.begin(); it != m_node_mgr.end(); ++it)
	m_patcher->addNode (*it);
    m_patcher->update();
}

void Table::detachPatcher()
{
    if (m_patcher) {
	m_patcher->deleteListener(this);
	m_patcher->clear();
	m_patcher = NULL;
    }
}

} /* namespace psynth */
