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
 
/***************************************************************************
 *                                                                         *
 *   Most of the code in this file was taken from libassa.                 *
 *                                                                         *
 *   LIBASSA                                                               *
 *   =======                                                               *
 *                                                                         *
 *   Copyright (C) 1997-2002,2005  Vladislav Grinchenko                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Library General Public           *
 *   License as published by the Free Software Foundation; either          *
 *   version 2 of the License, or (at your option) any later version.      *
 *                                                                         *
 ***************************************************************************/

#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sstream>
#include <iomanip>

#include "common/Error.h"
#include "common/CmdLineOpts.h"

using namespace std;

void
Option::dump () const 
{
    std::ostringstream msg;

    if (m_short_name != 0) {
		msg << "-" << m_short_name << ", ";
    }
    else {
		msg << "    ";
    }

    if (m_long_name.size ()) { 
		msg << "--" << std::setiosflags (std::ios::left)
			<< std::setw(14) << m_long_name.c_str () << ' ';
    }
    else {
		msg << std::setiosflags (std::ios::left) << std::setw (14) << "    ";
    }
    msg << '[';

    switch (m_type) 
    {
    case Option::string_t: 
		msg << std::setiosflags (std::ios::left) << std::setw(7) << "string";
		msg << "] = '" << *(string*) m_val << "'";
		break;

    case Option::int_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "int";
		msg << "] = " << *(int*) m_val; 
		break;

    case Option::uint_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "u_int";
		msg << "] = " << *(int*) m_val; 
		break;

    case Option::long_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "long";
		msg << "] = " << *(long*) m_val; 
		break;

    case Option::ulong_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "u_long";
		msg << "] = " << *(long*) m_val; 
		break;

    case Option::double_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "double";
		msg << "] = " << *(double*) m_val;
		break;

    case Option::float_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "float";
		msg << "] = " << *(float*) m_val;
		break;

    case Option::flag_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "bool";
		msg << "] = " << (*(bool*) m_val ? "true" : "false"); 
		break;

    case Option::func_t: 
		msg << std::setiosflags(std::ios::left) 
			<< std::setw(7) << "function ()"; 
		msg << ']';
		break;

    case Option::func_one_t: 
		msg << std::setiosflags(std::ios::left) 
			<< std::setw(7) << "function (opt)";
		msg << ']';
		break;

    case Option::none_t: 
		msg << std::setiosflags(std::ios::left) << std::setw(7) << "none"; 
		msg << ']';
		break;

    default: 
		msg << std::setiosflags(std::ios::left) 
		<< std::setw(7) << "--undef--";
		msg << ']';
    }
    msg << std::ends;
}

const char*
Option::type_c_str ()
{
	const char* ret;

    switch (m_type) 
    {
    case Option::string_t:   ret = "string";    break;
    case Option::int_t:      ret = "int";       break;
    case Option::uint_t:     ret = "u_int";     break;
    case Option::long_t:     ret = "long";      break;
    case Option::ulong_t:    ret = "u_long";    break;
    case Option::double_t:   ret = "double";    break;
    case Option::float_t:    ret = "float";     break;
    case Option::flag_t:     ret = "bool";      break;
    case Option::func_t:     ret = "func()";    break;
    case Option::func_one_t: ret = "func(opt)"; break;
    case Option::none_t:     ret = "none";      break;
    default:                 ret = "--undef--"; 
    }
	return (ret);
}

/*----------------------------------------------------------------------------*/
bool 
CmdLineOpts::is_valid (const char sopt_, const string& lopt_)
{
    set_error_none ();
    OptionSet::const_iterator i;

    for (i = m_opts_set.begin (); i != m_opts_set.end (); i++) {
		if (sopt_ == '\0' && lopt_.empty ()) {
			m_error = "Ignore empty option";
			return (false);
		}
		else if (sopt_ != '\0' && i->m_short_name == sopt_) {
			m_error = "Ignored multiple option '-";
			m_error += sopt_ + string ("'");
			return (false);
		}
		else if (!lopt_.empty () && i->m_long_name == lopt_) {
			m_error = "Ignore multiple option '--";
			m_error += lopt_ + string ("'");
			return (false);
		}
    }
    return (true);
}

Option*
CmdLineOpts::find_option (const char* str_)
{
    OptionSet::iterator i;

    for ( i = m_opts_set.begin (); i != m_opts_set.end (); i++) 
    {
		if (i->m_long_name == str_) {
			return &(*i);
		}
    }
    return (NULL);
}

Option*
CmdLineOpts::find_option (const char letter_)
{
    OptionSet::iterator i;

    for (i = m_opts_set.begin (); i != m_opts_set.end (); i++) 
    {
		if (i->m_short_name == letter_) 
			return &(*i);
    }
    return (NULL);
}

bool 
CmdLineOpts::addFlagOpt (const char sopt_, const string& lopt_, bool* v_)
{
    if (!is_valid (sopt_, lopt_)) 
		return (false);

    Option o (sopt_, lopt_, Option::flag_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, string* v_)
{
    if (!is_valid (sopt_, lopt_)) 
		return (false);

    Option o (sopt_, lopt_, Option::string_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}
	
bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, int* v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::int_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, unsigned int* v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::uint_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, long* v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::long_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, unsigned long* v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::long_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, double* v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::double_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, float* v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::float_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, OPTS_FUNC v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::func_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool 
CmdLineOpts::addOpt (const char sopt_, const string& lopt_, OPTS_FUNC_ONE v_)
{
    if (!is_valid (sopt_, lopt_)) {
		return (false);
    }
    
    Option o (sopt_, lopt_, Option::func_one_t, (void*) v_);
    m_opts_set.push_back (o);
    return (true);
}

bool
CmdLineOpts::delOpt (const char sopt_, const string& lopt_)
{
    OptionSet::iterator i;

    for (i = m_opts_set.begin (); i != m_opts_set.end (); i++) 
    {
		if (i->m_short_name == sopt_ || i->m_long_name == lopt_) 
		{
			m_opts_set.erase (i);
			return (true);
		}
    }
    return (false);
}
	
bool 
CmdLineOpts::parseArgs (const char* argv_[])
{

    register int skip = 1;
    bool pos_args_started = false;
    string param ("");
    string token ("");
    set_error_none ();
    Option* node = (Option*) NULL;

    for (argv_++; argv_[0]; argv_ += skip) {
		if (skip != 0) {
			token = argv_[0];
		}

		if (pos_args_started) {
			if (token[0] == '-' && token.size () != 1) {
				m_error = "Invalid order of arguments: '";
				m_error += token + "'.";
				goto done;
			}
			pos_arg (token.c_str ());
			continue;
		}
		skip = 1;

		if (token[0] == '-' && token.size () > 1 && token[1] != '-') {
			if (token.size () == 1 && !pos_args_started) {
				pos_arg (token.c_str ());
				pos_args_started = true;
				continue;
			}
				
			if ((node = find_option (token[1])) != NULL) {
				if (token.size () > 2) {
					if (node->m_type == Option::flag_t ||
						node->m_type == Option::func_t) 
					{
						token.erase (1, 1);
						skip = 0;
					}
					else {
						param = token.substr (2);
					} 
				} // if (token.size()>2)
			} // if ((node = find_option ())
		} 
		else {	
			if (token.size () > 1 && token[1] == '-') {
				string op = token.substr (2);
				size_t pos;

				if ((pos = op.find ("=")) != (size_t)-1) {
					param = op.substr (pos+1, op.length ());
					op.replace (pos, op.length() - pos, "");
				}
				node = find_option (op.c_str ());
			}
			else {	
				pos_arg (token.c_str ());
				pos_args_started = true;
				continue;
			} 
		} // if (token[0] == '-' && token[1] != '-') 

		if (!node) {
			m_error = "Invalid option '" + token + "'.";
			goto done;
		}

		if (node->m_type != Option::flag_t &&
			node->m_type != Option::func_t) 
		{
			if (param.empty ()) {
				if (!argv_[1]) {
					m_error = "Expecting parameter after '"
						+ string (argv_[0]) + "'.";
					goto done;
				}
				param = argv_[1];
				skip = 2;
			}
		}
		/*--- 
		 * if positional arguments only 
		 ---*/
		if (!node) {
			goto done;
		}

		if (param.empty ()) {
			if (!assign (node, argv_[1])) {
				return (false);
			}
		}
		else {
			const char* str = param.c_str ();
			if (!assign (node, str)) {
				return (false);
			}
			param = "";
		}
    } // for (argv_++; argv_[0]; argv_ += skip) 

 done:
    return !m_error.empty () ? false : true;
}

#if 0
/**
 * An Option object keeps long names the way they were assigned
 * by the add_opt() call (i.e. "mac-address").
 * IniFile has same options in their normalized format (i.e. "mac_address").
 *
 * Iterate throught the set of all registered Options.  For each in the set,
 * normalize its name and lookup in IniFile's [option] section for the value.
 */
int
CmdLineOpts::parseConfigFile (IniFile& inifile_)
{
	unsigned int count = 0;
	string v;
	string s;
	OptionSet::iterator pos = m_opts_set.begin ();

	if (inifile_.find_section ("options") == inifile_.sect_end ()) {
		m_error = "Missing [options] section in INI file!";
		return -1;
	}

	while (pos != m_opts_set.end ()) {
		if (pos->m_long_name.size ()) {
			s = pos->m_long_name;
			ASSA::Utils::find_and_replace_char (s, '-', '_');
			DL ((CMDLINEOPTS, "trying option \"%s\"\n", s.c_str ()));
			v = inifile_.get_value ("options", s);
			if (v.size ()) {
				if (assign (&(*pos), v.c_str ())) {
					count++;
				}
			}
		}
		pos++;
	}

	return (count);
}
#endif

bool 
CmdLineOpts::assign (Option* node_, const char* op_)
{
    long l;
    double d;

	if (node_ && op_) {

	}

    /*---
      From strtol(3C) man page:

      "Because 0 is returned on error and is also a valid return on
      success, an application wishing to check for error situations 
      should set 'errno' to 0, then call strtol(3C), then check 'errno'
      and if it is non-zero, assume an error has occured."
      ---*/
	  
    switch (node_->m_type) {
    case Option::string_t:
		*(string*) node_->m_val = op_;
		break;

    case Option::int_t:
    case Option::long_t:
		errno = 0;
		l = strtol (op_, NULL, 0);

		if (errno != 0) {
			m_error = "Error: '" + string (strerror (errno)) + "',";
			m_error += " in converting to integer from '";
			m_error += string (op_) + "'.";
			return (false);
		}

		if (node_->m_type == Option::int_t) {
			*(int*) node_->m_val = int (l);
		}
		else {
			*(long*) node_->m_val = l;
		}
		break;

    case Option::uint_t:
    case Option::ulong_t:
		errno = 0;
		l = strtol (op_, NULL, 0);

		if (errno != 0) {
			m_error = "Error: '" + string (strerror (errno)) + "',";
			m_error += " in converting to unsinged integer from '";
			m_error += string (op_) + "'.";
			return (false);
		}

		if (node_->m_type == Option::uint_t) {
			*(unsigned int*) node_->m_val = int (l);
		}
		else {
			*(unsigned long*) node_->m_val = l;
		}
		break;

    case Option::double_t:
    case Option::float_t:
		errno = 0;
		d = strtod (op_, NULL);

		if (errno != 0) {
			m_error = "Error: '" + string (strerror (errno)) + "',";
			m_error += " in converting to double/float from '";
			m_error += string (op_) + "'.";
			return (false);
		}

		if (node_->m_type == Option::double_t) {
			*(double*) node_->m_val = d;
		}
		else {
			*(float*) node_->m_val = float (d);
		}
		break;

    case Option::flag_t:
		*(bool*) node_->m_val = true; // no more flipping!
		break;
		
    case Option::func_t:
		(*(OPTS_FUNC)(node_->m_val)) ();
		break;

    case Option::func_one_t:
		(*(OPTS_FUNC_ONE)(node_->m_val)) (op_);
		break;

    case Option::none_t:
    default:
		m_error = "Undefined type for option '"+string (op_)+"'.";
		return (false);
    } /*-- switch () --*/

    return (true);
}

void
CmdLineOpts::dump() const
{
    OptionSet::const_iterator i;

    for (i = m_opts_set.begin (); i != m_opts_set.end (); i++) {
		i->dump ();
    }

    if (!m_error.empty ()) {

    }
}

void
CmdLineOpts::str2argv (const string& src_, int& argc_, char**& argv_)
{
    std::vector<string> vs;
    std::istringstream input (src_);
    std::string token;

    while (input >> token) {
		vs.push_back (token);
		token = "";
    }
    int i = 0;
    char* p;

    if (vs.size ()) {
		argv_ = new char* [vs.size() + 1];
		std::vector<string>::iterator it;

		for (it = vs.begin (); it != vs.end (); it++, i++) {
			p = new char [it->size() + 1];
			strcpy (p, it->c_str ());
			p[it->size()] = '\0';
			argv_[i] = p;
		}
		argv_[i] = NULL;
    }
    argc_ = i;
}

void
CmdLineOpts::freeArgv (char**& argv_)
{
	/* If argument is empty (which should never be the case),
	 * then freeing the memory would core dump application.
	 */
	if (argv_ == NULL) {
		return;
	}

    for (int i = 0; argv_[i]; i++) {
		delete [] argv_[i];
    }
    delete [] argv_;
    argv_ = NULL;
}

