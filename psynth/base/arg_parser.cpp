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
PSYNTH_DEFINE_ERROR_WHAT  (unknown_option_error, "Unknown option.")
PSYNTH_DEFINE_ERROR_WHAT  (parse_option_error,   "Error while parsing option parameter.")

arg_parser::~arg_parser ()
{
    for (std::list <option*>::iterator i = m_all.begin (); i != m_all.end (); ++i)
	delete *i;
}

void arg_parser::add (unsigned char flag, const char* str, option* op)
{
    if (op == 0)
	op = new option;
    
    if (flag != NULL_FLAG)
	m_short[flag].push_back (op);

    if (str != 0)
	m_long[str].push_back (op);

    m_all.push_back (op);
}

void arg_parser::parse (int argc, const char *argv[])
{
    const char** argv_end = argv++ + argc;
    
    m_free.clear ();

    try {
	while (argv < argv_end) {
	    switch (get_type (*argv))
	    {
	    case arg_free:
		m_free.push_back (*argv++);
		break;

	    case arg_short:
		argv = parse_short (argv, argv_end);
		break;

	    case arg_long:
		argv = parse_long (argv, argv_end);
		break;
	    }
	}
    } catch (parse_error)
    {
	throw parse_option_error (std::string ("Error while parsing option: ")
				  + *argv);
    }
}

const char** arg_parser::parse_short (const char** argv, const char** argv_end)
{
    const char** argv_next = argv + 1;

    /* Several options can be grouped like '-xzvf' */
    for (const char *s = *argv + 1; *s != '\0'; ++s)
    {
	option_iterator iter = m_short [(size_t) *s].begin ();
	if (iter == m_short [(size_t) *s].end ())
	    throw unknown_option_error (std::string ("Unknown option: ") + *s);
	
	for (; iter != m_short [(size_t) *s].end (); ++iter)
	{
	    if (argv_next >= argv_end ||
		!iter->parse (*argv_next))
		iter->parse ();
	    else
		++ argv_next;
	}
    }

    return argv_next;
}

const char** arg_parser::parse_long (const char** argv, const char** argv_end)
{
    const char** argv_next = argv + 1;

    map <const char *, list <option*> >::iterator opt
	= m_long.find (*argv + 2);

    if (opt == m_long.end ())
	throw unknown_option_error (std::string ("Unknown option: ") +
				    (*argv + 2));
    
    for (option_iterator iter = opt->second.begin ();
	 iter != opt->second.end ();
	 ++iter)
    {
	if (argv_next >= argv_end ||
	    !iter->parse (*argv_next))
	    iter->parse ();
	else
	    ++ argv_next;
    }

    return argv_next;
}

} /* namespace psynth */

