/**
 *  Time-stamp:  <2011-03-08 21:11:26 raskolnikov>
 *
 *  @file        arg_parser.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Command line argument parsing facility. Implementation.
 */

/*
 *  Copyright (C) 2007, 2009 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define PSYNTH_MODULE_NAME "psynth.base.arg_parser"

#include <cstring>
#include <cstdlib>
#include <iostream>
#include <typeinfo>

#include "base/arg_parser.hpp"

using namespace std;

namespace psynth
{
namespace base
{

PSYNTH_DEFINE_ERROR (arg_parser_error)
PSYNTH_DEFINE_ERROR_WHAT (unknown_option_error, "Unknown option.")
PSYNTH_DEFINE_ERROR_WHAT (parse_option_error,
			  "Error while parsing option parameter.")

namespace
{

enum arg_type
{
    arg_short,
    arg_long,
    arg_free
};

arg_type get_type (const char *arg)
{
    if (arg[0] == '-') {
	if (arg[1] == '-')
	    return arg_long;
	else
	    return arg_short;
    }
	
    return arg_free;
}
    
bool is_free (const char *arg)
{
    return arg [0] != '-';
}

}

void arg_parser::add (unsigned char flag, const char* str, option* op)
{
    // FIXME: Potential memory leak. push_back can throw.
    
    if (op == 0)
	op = new option;
    
    if (flag != null_flag)
	_short[flag].push_back (op);

    if (str != 0)
	_long[str].push_back (op);

    _all.push_back (std::unique_ptr<option> (op));
}

void arg_parser::parse (int argc, const char *argv[])
{
    const char** argv_end = argv++ + argc;
    
    _free.clear ();

    try {
	while (argv < argv_end) {
	    switch (get_type (*argv))
	    {
	    case arg_free:
		_free.push_back (*argv++);
		break;

	    case arg_short:
		argv = parse_short (argv, argv_end);
		break;

	    case arg_long:
		argv = parse_long (argv, argv_end);
		break;
	    }
	}
    }
    catch (parse_error)
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
	option_iterator iter = _short [(size_t) *s].begin ();
	if (iter == _short [(size_t) *s].end ())
	    throw unknown_option_error (std::string ("Unknown option: ") + *s);
	
	for (; iter != _short [(size_t) *s].end (); ++iter)
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

    auto opt = _long.find (*argv + 2);

    if (opt == _long.end ())
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

} /* namespace base */
} /* namespace psynth */

