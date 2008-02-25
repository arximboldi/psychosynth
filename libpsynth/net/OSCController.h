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

#ifndef PSYNTH_OSCCONTROLLER_H
#define PSYNTH_OSCCONTROLLER_H

#include <map>

#include <libpsynth/table/Table.h>
#include <libpsynth/net/OSCMisc.h>
#include <libpsynth/net/OSCBroadcast.h>

namespace psynth
{

class OSCController : public TableListener,
		      public TableObjectListener,
		      public OSCBroadcast
{
    std::map<std::pair<int,int>, int> m_local_id;
    std::map<int, std::pair<int,int> > m_net_id;
    Table* m_table;
    int m_skip;
    int m_id;
    bool m_activated;
    bool m_broadcast;
    
    LO_HANDLER(OSCController, add);
    LO_HANDLER(OSCController, delete);
    LO_HANDLER(OSCController, param);
    LO_HANDLER(OSCController, activate);
    LO_HANDLER(OSCController, deactivate);

    void addToTable(Table* table) {
	table->addTableListener(this);
	table->addTableObjectListener(this);
    }

    void deleteFromTable(Table* table) {
	table->deleteTableListener(this);
	table->deleteTableObjectListener(this);
    }
    
public:
    OSCController(bool broadcast = false);
    ~OSCController();

    void setID(int id) {
	m_id = id;
    }

    void setTable(Table* table) {
	deactivate();
	m_table = table;
    }

    Table* getTable() {
	return m_table;
    }

    void activate() {
	if (!m_activated && m_table) {
	    m_activated = true;
	    addToTable(m_table);
	}
    }

    void deactivate() {
	if (m_activated && m_table) {
	    m_activated = false;
	    deleteFromTable(m_table);
	}
    }

    void clear() {
	deactivate();
	m_local_id.clear();
	m_net_id.clear();
	m_skip = 0;
	OSCBroadcast::clear();
    }

    void addMethods(lo_server s);
    
    void handleAddObject(TableObject& obj);
    void handleDeleteObject(TableObject& obj);
    void handleActivateObject(TableObject& obj);
    void handleDeactivateObject(TableObject& obj);
    void handleSetParamObject(TableObject& ob, Object::ParamID param_id);
};

} /* namespace psynth */

#endif /* PSYNTH_OSCCONTROLLER_H */
