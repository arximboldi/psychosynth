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

#include "table/Table.h"
#include "output/OutputAlsa.h"
#include "output/OutputOss.h"
#include "common/Error.h"

#include "object/ObjectOutput.h"
#include "object/ObjectMixer.h"
#include "object/ObjectOscillator.h"
#include "object/ObjectLFO.h"
#include "object/ObjectFilter.h"

using namespace std;

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

void TableSubject::notifySetParamObject(TableObject& obj, Object::ParamID param_id)
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

Table::Table(const AudioInfo& info) :
    m_info(info),
    m_patcher(NULL),
    m_last_id(MIN_USER_ID)
{
    m_output = new ObjectOutput(m_info);
    m_mixer = new ObjectMixer(m_info, MIXER_CHANNELS);
    m_objmgr.attachObject(m_output, OUTPUT_ID);
    m_objmgr.attachObject(m_mixer, MIXER_ID);
}

Table::~Table()
{
    delete m_patcher;
}

void
Table::clear()
{
    for (ObjectManager::Iterator it = m_objmgr.begin();
	 it != m_objmgr.end();)
	if ((*it)->getID() >= MIN_USER_ID) {
	    if (m_patcher)
		m_patcher->deleteObject(*it);
	    TableObject obj(*it, this);
	    notifyDeleteObject(obj);
	    m_objmgr.remove(it++);
	} else
	    ++it;
}

TableObject Table::findObject(int id)
{
    ObjectManager::Iterator i = m_objmgr.find(id);
    if (i == m_objmgr.end())
	return TableObject(NULL, NULL);
    else
	return TableObject(*i, this);
}

TableObject Table::addObject(int type)
{
    Object* obj;

    switch (type) {
    case OBJ_OSCILLATOR:
	obj = new ObjectOscillator(m_info);
	break;
    case OBJ_LFO:
	obj = new ObjectLFO(m_info);
	break;
    case OBJ_FILTER:
	obj = new ObjectFilter(m_info);
	break;
    case OBJ_MIXER:
	obj = new ObjectMixer(m_info);
	break;
    default:
	obj = NULL;
	return TableObject(NULL, NULL);
    }
    
    if (!m_objmgr.attachObject(obj, m_last_id++))
	return TableObject(NULL, NULL);

    TableObject tobj(obj, this);
    notifyAddObject(tobj);
    return tobj;
}

void Table::deleteObject(TableObject& obj)
{
    if (m_patcher)
	m_patcher->deleteObject(obj.m_obj);
    notifyDeleteObject(obj);
    m_objmgr.detachObject(obj.m_obj->getID());
    delete obj.m_obj;
}

void Table::activateObject(TableObject& obj)
{
    obj.m_obj->update(NULL, -1, -1);
    obj.m_obj->advance();
    
    if (m_patcher)
	m_patcher->addObject(obj.m_obj);
    notifyActivateObject(obj);
}

void Table::deactivateObject(TableObject& obj)
{
    if (m_patcher)
	m_patcher->deleteObject(obj.m_obj);
    notifyDeactivateObject(obj);
}

void Table::attachPatcher(Patcher* pat)
{
    dattachPatcher();
    m_patcher = pat;
    pat->addListener(this);
    for (ObjectManager::Iterator it = m_objmgr.begin(); it != m_objmgr.end(); ++it)
	m_patcher->addObject(*it);
}

void Table::dattachPatcher()
{
    if (m_patcher) {
	m_patcher->deleteListener(this);
	m_patcher->clear();
	m_patcher = NULL;
    }
}

