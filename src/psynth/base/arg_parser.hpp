/**
 *  Time-stamp:  <2011-03-08 21:05:16 raskolnikov>
 *
 *  @file        arg_parser.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Command line argument parsing facility.
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

#ifndef PSYNTH_ARG_PARSER_H
#define PSYNTH_ARG_PARSER_H

#include <sstream>
#include <cstring>
#include <string>
#include <limits>
#include <list>
#include <map>

#include <psynth/base/exception.hpp>
#include <psynth/base/iterator.hpp>
#include <psynth/base/util.hpp>

namespace psynth
{
namespace base
{

/**
 * A parsing error.
 */
PSYNTH_DECLARE_ERROR (error, arg_parser_error)
PSYNTH_DECLARE_ERROR (arg_parser_error, unknown_option_error)
PSYNTH_DECLARE_ERROR (arg_parser_error, parse_option_error)

/**
 * Utility exception. Parsers (options) must throw this in order to get
 * arg_parser to correctly wrap the message.
 */
class parse_error : public std::exception {};

namespace detail
{

/**
 * Less-than functor for @c cstrings.
 */
struct ltstr
{
    /**
     * Run operator. Returns whether the first string is less than the second.
     * @param s1 First string.
     * @param s2 Second string.
     * @return Whether @a is less than @a s2.
     */
    bool operator  () (const char *s1, const char *s2) const
    {
	return strcmp (s1, s2) < 0;
    }
};

/**
 * Functor wrapper for atoi.
 */
struct atoi_func
{
    int operator () (const char *str)
    {
	char* end = 0;
	int res = strtol (str, &end, 10);
	if (end <= str)
	    throw parse_error ();
	return res;
    }
};

/**
 * Functor wrapper for atof.
 */
struct atof_func
{
    float operator () (const char *str)
    {
	char* end = 0;
	float res = strtof (str, &end);
	
	if (end <= str)
	    throw parse_error ();
	return res;
    }
};

/**
 * Functor for convertin from a const char* to another if it has defined
 * a converter contructor.
 */
template <class T>
struct atoany_func
{
    T operator () (const char *str)
    {
	return T (str);
    }
};

} /* namespace detail */

/**
 * The interface of an option of the argument parser.
 */
class option
{
public:
    /**
     * Virtual destructor.
     */
    virtual ~option ()
    {
    }

    /**
     * Define this function to make this option receive an argument.
     * @param arg The argument being parsed.
     * @return @c true if you where able to parse the argument or @c false
     *  if you could not or the option does not receive arguments.
     */
    virtual bool parse (const char *arg)
    {
	return false;
    };

    /**
     * Define this function to make the option be a toggle.
     * @return @c true if you where able to toggle or @c false if the option
     *  must receive an argument.
     */
    virtual bool parse ()
    {
	return false;
    };
};

/**
 * Option that catches the argument and stores its value, converted to
 * any desired type.
 * Template argument @a T defines the desired type and @a ConvFunc
 * must be the proper converter function, which defaults to @c atoany_func.
 */
template <class T, class ConvFunc = detail::atoany_func <T> >
class option_generic : public option
{
public:
    /**
     * Constructor.
     * @param data Pointer to where you want to store the argument value.
     */
    option_generic (T* data)
	: _data (data)
    {}

    /**
     * Parses an argument.
     * @param arg The argument.
     * @return @c true.
     */
    bool parse (const char *arg)
    {
	*_data = ConvFunc ()(arg);
	return true;
    }

private:
    T* _data;
};

/**
 * A boolan option that is set to true if the option exists.
 */
class option_flag : public option
{
    bool* _flag;

public:
    /**
     * Constructor.
     * @param f Where to notify if it was found.
     */
    option_flag (bool* f)
	: _flag (f)
    {}

    /**
     * Sets to true.
     * @return @c true.
     */
    bool parse ()
    {
	*_flag = true;
	return true;
    }
};

/**
 * A boolan option that is set to true if the option exists.
 */
class option_not_flag : public option
{
    bool* _flag;

public:
    /**
     * Constructor.
     * @param f Where to notify if it was found.
     */
    option_not_flag (bool* f)
	: _flag (f)
    {}

    /**
     * Sets to false.
     * @return @c true.
     */
    bool parse ()
    {
	*_flag = false;
	return true;
    }
};

/**
 * Option that parses the param using a stringstream.
 * Note that template param @a T mus have defined its flow input operator.
 */
template <class T>
class option_stream : public option
{
    T *m_data;

public:
    /**
     * Constructor.
     * @param T A pointer where to store the read value.
     */
    option_stream (T * data):
	m_data (data)
    {}

    /**
     * Receives a string.
     * @param arg The string to receive.
     * @return @c true.
     */
    bool parse (const char *arg)
    {
	std::stringstream stream (std::string (arg), std::ios_base::in);
	stream.exceptions (std::stringstream::eofbit |
			   std::stringstream::failbit |
			   std::stringstream::badbit);
	try {
	    stream >> *m_data;
	} catch (...) {
	    throw parse_error ();
	}
	
	return true;
    }
};

/** An @c int option. */
typedef option_generic <int, detail::atoi_func> option_int;
/** A @c float option. */
typedef option_generic <float, detail::atof_func> option_float;
/** A @c std::string option. */
typedef option_generic <std::string> option_string;
/** A @c cstring option. */
typedef option_generic <const char*> option_cstring;

/**
 * Class for parsing program arguments. You register options that
 * individual arguments. It includes some wrapper functions to add options
 * to parse arguments of basic data types.
 *
 * @see option
 */
class arg_parser : public boost::noncopyable
{
public:
    /**
     * Iterator for the arguments that where not parsed.
     */
    typedef std::list <const char*>::iterator iterator;

    /**
     * ConstIterator for the arguments that where not parse.
     */
    typedef std::list <const char*>::const_iterator const_iterator;

    /**
     * Adds an @c int option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     */
    void add (unsigned char flag, const char* str, int* data)
    {
	add (flag, str, data ? new option_int (data) : 0);
    }

    /**
     * Adds a @c float option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add (unsigned char flag, const char* str, float* data)
    {
	add (flag, str, data ? new option_float (data) : 0);
    }

    /**
     * Adds a @c bool option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add (unsigned char flag, const char* str, bool* data)
    {
	add (flag, str, data ? new option_flag (data) : 0);
    }

    /**
     * Adds a @c string option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add (unsigned char flag, const char* str, std::string* data)
    {
	add (flag, str, data ? new option_string (data) : 0);
    }

    /**
     * Adds a @c cstring option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add (unsigned char flag, const char* str, const char** data)
    {
	add (flag, str, data ? new option_cstring (data) : 0);
    }

    /**
     * Adds an user defined option. If the option is null an empty option
     * is added -produces no result but avoids exceptions.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param op The option to associate to @a and @str.
     * @see Option
     */
    void add (unsigned char flag, const char* str, option* op);
    
    /**
     * Parse the command line arguments using the defined options. Any
     * arguments that had no option associated are added to the free
     * arguments list, which can be later iterated.
     *
     * @param argc Number of command line arguments.
     * @param argv Array of command line arguments.
     *
     * @throw A arg_parser_error if there was an option threw an exception,
     * containing the problematic option in the what ().
     */
    void parse (int argc, const char* argv[]);

    /**
     * Returns an @c iterator to the first free argument.
     */
    iterator begin ()
    {
	return _free.begin ();
    }

    /**
     * Returns a @c const_iterator to the first free argument.
     */
    const_iterator begin () const
    {
	return _free.begin ();
    }

    /**
     * Returns an @c Iterator to the end of free arguments list.
     */
    iterator end ()
    {
	return _free.end ();
    }

    /**
     * Returns a @c ConstIterator to the end of free arguments list.
     */
    const_iterator end () const
    {
	return _free.end ();
    }

    /**
     * Return the number of free arguments.
     */
    size_t free_args () const
    {
	return _free.size ();
    }

    /**
     * Returns whether there are any free arguments.
     */
    bool has_free_args () const
    {
	return !_free.empty ();
    }

private:
    typedef ptr_iterator<std::list<option*>::iterator> option_iterator;
    typedef std::list<option*> option_list;
    typedef std::list<std::unique_ptr<option> > unique_option_list;
    typedef std::map <const char*, option_list, detail::ltstr>
    option_map;

    static const unsigned char null_flag = '\0';
    
    option_list              _short [256];
    option_map               _long;
    unique_option_list       _all;
    std::list <const char *> _free;

    const char** parse_short (const char** argv, const char** argv_end);
    const char** parse_long (const char** argv, const char** argv_end);    
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_ARGPARSER_H */
