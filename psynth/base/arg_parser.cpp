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
#include <iostream>
#include <typeinfo>

#include "base/arg_parser.hpp"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_ERROR_WHERE (arg_parser_error, "arg_parser")

arg_parser::~arg_parser ()
{
    for (std::list <option*>::iterator i = m_all.begin (); i != m_all.end (); ++i)
	delete *i;
}

void arg_parser::add (unsigned char flag, const char* str, option* op)
{    
    if (flag != NULL_FLAG)
	m_short[flag].push_back (op);

    if (str != 0)
	m_long[str].push_back (op);

    m_all.push_back (op);
}

void arg_parser::parse (int argc, const char *argv[])
{
    int i;
    int skip;
    
    m_free.clear ();

    try
    {
	for (i = 1, skip = 1; i < argc; i += skip, skip = 1)
	{
	    switch (get_type (argv[i]))
	    {
	    case ARG_FREE:
		m_free.push_back (argv[i]);
		break;

	    case ARG_SHORT:
		for (const char *s = argv[i] + 1; *s != '\0'; ++s) {
		    for (option_iterator j = m_short[(size_t) *s].begin ();
			 j != m_short[(size_t) *s].end ();
			 ++j)
		    {
			if ((i + skip >= argc) ||
			    !j->parse (argv[i + skip]))
			    j->parse ();
			else
			    ++ skip;
		    }
		}
		break;

	    case ARG_LONG:
		map <const char *, list <option*> >::iterator
		    l = m_long.find (argv[i] + 2);

		if (l != m_long.end()) {
		    for (option_iterator j = l->second.begin ();
			 j != l->second.end (); ++j)
		    {
			if ((i + skip >= argc) ||
			    !j->parse (argv[i + skip]))
			    j->parse ();
			else
			    ++ skip;
		    }
		}
		break;
	    }
	}
    }
    catch (...)
    {
	throw arg_parser_error (std::string ("Error while parsing: ") + argv [i]);
    }
}

} /* namespace psynth */

