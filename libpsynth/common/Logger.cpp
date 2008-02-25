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

#include "common/Logger.h"

using namespace std;

namespace psynth
{

Log::~Log()
{
    for (list<LogSink*>::iterator it = m_dumpers.begin();
	 it != m_dumpers.end(); ++it) {
	delete *it;
    }
}

Log& Log::getPath(std::string path)
{
    string base;
    for (size_t i = 0; i != path.size(); ++i)
	if (path[i] == '/') {
	    base.assign(path, 0, i);
	    while(path[++i] = '/');
	    path.erase(0, i);
	    break;
	}

    if (base.empty()) {
	return getChild(path);
    }

    return getChild(base).getPath(path);
}

void Log::log(Log& log, int level, const string& msg)
{
    for (list<LogSink*>::iterator it = m_dumpers.begin();
	 it != m_dumpers.end(); ++it) {
	(*it)->dump(log, level, msg);
    }

    if (m_parent)
	m_parent->log(log, level, msg);
}

} /* namespace psynth */

