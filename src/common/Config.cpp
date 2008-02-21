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

#include "common/Config.h"

using namespace std;

void ConfSubject::notifyConfChange(const ConfNode& source)
{
    for (list<ConfListener*>::iterator i = m_list.begin();
	 i != m_list.end();
	 ++i)
	(*i)->handleConfChange(source);
}

void ConfSubject::notifyNewChild(const ConfNode& child)
{
    for (list<ConfListener*>::iterator i = m_list.begin();
	 i != m_list.end();
	 ++i)
	(*i)->handleNewChild(child);
}

ConfNode& ConfNode::getPath(std::string path)
{
    string base;
    for (size_t i = 0; i != path.size(); ++i)
	if (path[i] == '/') {
	    base.assign(path, 0, i);
	    path.erase(0, i);
	    break;
	}

    if (base.empty()) {
	return getChild(path);
    }

    return getChild(base).getPath(path);
}
