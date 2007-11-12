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

using namespace std;

Table::Table(const AudioInfo& info) :
    m_info(info)
{
    m_output = new ObjectOutput(m_info);
    m_mixer = new ObjectMixer(m_info, MIXER_CHANNELS);
    m_objmgr.attachObject(m_output, OUTPUT_ID);
    m_objmgr.attachObject(m_mixer, MIXER_ID);
    m_patcher.addObject(m_output);
    m_patcher.addObject(m_mixer);
}

Table::~Table()
{
}

TableObject Table::findObject(int id)
{
    ObjectManager::Iterator i = m_objmgr.find(id);
    if (i == m_objmgr.end())
	return TableObject(NULL, NULL);
    else
	return TableObject(*i, this);
}

TableObject Table::addObject(int type, int id)
{
    Object* obj;

    switch (type) {
    case OBJ_OSCILLATOR:
	obj = new ObjectOscillator(m_info);
	break;
    case OBJ_MIXER:
	obj = new ObjectMixer(m_info);
	break;
    default:
	obj = NULL;
	return TableObject(NULL, NULL);
    }

    if (!m_objmgr.attachObject(obj, id))
	return TableObject(NULL, NULL);

    TableObject tobj(obj, this);
    notifyAddObject(tobj);
    return tobj;
}

void Table::moveObject(TableObject& obj, Real x, Real y)
{
    obj.m_obj->setXY(x, y);
    m_patcher.moveObject(obj.m_obj);
    notifyMoveObject(obj);
}

void Table::deleteObject(TableObject& obj)
{
    m_patcher.deleteObject(obj.m_obj);
    notifyDeleteObject(obj);
    m_objmgr.detachObject(obj.m_obj->getID());
    delete obj.m_obj;
}

void Table::activateObject(TableObject& obj)
{
    m_patcher.addObject(obj.m_obj);
    notifyActivateObject(obj);
}

void Table::deactivateObject(TableObject& obj)
{
    m_patcher.deleteObject(obj.m_obj);
    notifyDeactivateObject(obj);
}
