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

#include "common/ArgParser.h"

using namespace std;

ArgParser::~ArgParser()
{
    for (list<Option*>::iterator i = m_all.begin(); i != m_all.end(); ++i)
	delete *i;
}

void ArgParser::addOption(unsigned char flag, const char* str, Option* op)
{
    if (flag != NULL_FLAG)
	m_short[flag].push_back(op);

    if (str != NULL)
	m_long[str].push_back(op);

    m_all.push_back(op);
}

void ArgParser::parseArgs(int argc, const char* argv[])
{
    int i;
    
    m_free.clear();

    for (i = 1; i < argc; i++) {
	bool skip = false;
	switch (getType(argv[i])) {
	case ARG_FREE:
	    m_free.push_back(argv[i]);
	    break;
	    
	case ARG_SHORT:
	    for (const char* s = argv[i]+1; *s != '\0'; ++s) {
		for (list<Option*>::iterator j = m_short[(size_t)*s].begin();
		     j != m_short[(size_t)*s].end(); ++j) {
		    if ((i+1 == argc) ||
			!isFree(argv[i+1]) ||
			!(*j)->parse(argv[i+1]))
			(*j)->parse();
		    else
			skip = true;		    
		}
	    }
	    break;
		
	case ARG_LONG:
	    map<const char*, list<Option*> >::iterator l = m_long.find(argv[i]+2);
	    if (l != m_long.end()) {
		for (list<Option*>::iterator j = (*l).second.begin();
		     j != (*l).second.end(); ++j) {
		    if ((i+1 == argc) ||
			!isFree(argv[i+1]) ||
			!(*j)->parse(argv[i+1]))
			(*j)->parse();
		    else
			skip = true;
		}
	    }
	    break;
	}
	
	i += skip;
    }
}


