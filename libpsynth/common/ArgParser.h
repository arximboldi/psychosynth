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

#ifndef PSYNTH_ARGPARSER_H
#define PSYNTH_ARGPARSER_H

#include <string>	
#include <limits>
#include <list>
#include <map>
#include <ios>

namespace psynth
{

/**
 * The interface of an option of the argument parser.
 */
class Option {
public:
    /**
     * Virtual destructor.
     */
    virtual ~Option() {}

    /**
     * Define this function to make this option receive an argument.
     * @param arg The argument being parsed.
     * @return @c true if you where able to parse the argument or @c false
     *  if you could not or the option does not receive arguments.
     */
    virtual bool parse(const char* arg) {
	return false;
    };

    /**
     * Define this function to make the option be a toggle.
     * @return @c true if you where able to toggle or @c false if the option
     *  must receive an argument.
     */
    virtual bool parse() {
	return false;
    };
};

/**
 * Functor wrapper for atoi.
 */
struct atoi_func
{
    int operator() (const char* str) {
	return atoi(str);
    }
};

/**
 * Functor wrapper for atof.
 */
struct atof_func
{
    float operator() (const char* str) {
	return atof(str);
    }
};

/**
 * Functor for convertin from a const char* to another if it has defined
 * a converter contructor.
 */
template <class T>
struct atoany_func
{
    T operator() (const char* str) {
	return T(str);
    }
};

/**
 * Option that catches the argument and stores its value, converted to
 * any desired type.
 * Template argument @a T defines the desired type and @a ConvFunc
 * must be the proper converter function, which defaults to @c atoany_func.
 */
template <class T, class ConvFunc = atoany_func<T> >
class OptionGeneric : public Option
{
    T* m_data;
public:

    /**
     * Constructor.
     * @param data Pointer to where you want to store the argument value.
     */
    OptionGeneric(T* data) :
	m_data(data) {}

    /**
     * Parses an argument.
     * @param arg The argument.
     * @return @c true.
     */
    bool parse(const char* arg) {
	*m_data = ConvFunc()(arg);
	return true;
    }
};

/**
 * A boolan option that is set to true if the option exists.
 */
class OptionFlag : public Option
{
    bool* m_flag;
public:
    /**
     * Constructor.
     * @param f Where to notify if it was found.
     */
    OptionFlag(bool* f) :
	m_flag(f) {}

    /**
     * Sets to true.
     * @return @c true.
     */
    bool parse() {
	*m_flag = true;
	return true;
    }
};

/**
 * Option that parses the param using a stringstream.
 * Note that template param @a T mus have defined its flow input operator.
 */
template <class T>
class OptionStream : public Option
{
    T* m_data;
public:
    /**
     * Constructor.
     * @param T A pointer where to store the read value.
     */
    OptionStream(T* data) :
	m_data(data) {}

    /**
     * Receives a string.
     * @param arg The string to receive.
     * @return @c true.
     */
    bool parse(const char* arg) {
	std::stringstream stream(std::string(arg), std::ios_base::in);
	stream >> *m_data;
	return true;
    }
};

/** An @c int option. */
typedef OptionGeneric<int, atoi_func> OptionInt;
/** A @c float option. */
typedef OptionGeneric<float, atof_func> OptionFloat;
/** A @c std::string option. */
typedef OptionGeneric<std::string> OptionString;
/** A @c cstring option. */
typedef OptionGeneric<const char*> OptionCString;

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
    bool operator()(const char* s1, const char* s2) const {
	return strcmp(s1, s2) < 0;
    }
};

/**
 * Class for parsing program arguments. You register options that
 * individual arguments. It includes some wrapper functions to add options
 * to parse arguments of basic data types.
 *
 * @see option
 */
class ArgParser
{    
    static const unsigned char NULL_FLAG = '\0';
    std::list<Option*> m_short[256];
    std::map<const char*, std::list<Option*>, ltstr> m_long;
    std::list<Option*> m_all;
    std::list<const char*> m_free;

    enum ArgType {
	ARG_SHORT,
	ARG_LONG,
	ARG_FREE
    };
    
    int getType(const char* arg) {
	if (arg[0] == '-')
	    if (arg[1] == '-')
		return ARG_LONG;
	    else
		return ARG_SHORT;
	return ARG_FREE;
    }
    
    bool isFree(const char* arg) {
	return arg[0] != '-';
    };
    
    ArgParser(const ArgParser& arg);
    ArgParser& operator=(const ArgParser& arg);
    
public:
    /**
     * Iterator for the arguments that where not parsed.
     */
    typedef std::list<const char*>::iterator Iterator;

    /**
     * ConstIterator for the arguments that where not parse.
     */
    typedef std::list<const char*>::const_iterator ConstIterator;

    ArgParser() {}
    ~ArgParser();

    /**
     * Adds an @c int option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     */
    void add(unsigned char flag, const char* str, int* data) {
	add(flag, str, new OptionInt(data));
    }

    /**
     * Adds a @c float option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add(unsigned char flag, const char* str, float* data) {
	add(flag, str, new OptionFloat(data));
    }

    /**
     * Adds a @c bool option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add(unsigned char flag, const char* str, bool* data) {
	add(flag, str, new OptionFlag(data));
    }

    /**
     * Adds a @c string option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add(unsigned char flag, const char* str, std::string* data) {
	add(flag, str, new OptionString(data));
    }

    /**
     * Adds a @c cstring option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param data Where to store the passed value.
     */
    void add(unsigned char flag, const char* str, const char** data) {
	add(flag, str, new OptionCString(data));
    }

    /**
     * Adds an user defined option.
     * @param flag Short version of the option.
     * @param str Long version of the option.
     * @param op The option to associate to @a and @str.
     * @see Option
     */
    void add(unsigned char flag, const char* str, Option* op);

    /**
     * Parse the command line arguments using the defined options. Any
     * arguments that had no option associated are added to the free
     * arguments list, which can be later iterated.
     * @param argc Number of command line arguments.
     * @param argv Array of command line arguments.
     */
    void parse(int argc, const char* argv[]);

    /**
     * Returns an @c Iterator to the first free argument.
     */
    Iterator begin() {
	return m_free.begin();
    }

    /**
     * Returns a @c ConstIterator to the first free argument.
     */
    ConstIterator begin() const {
	return m_free.begin();
    }

    /**
     * Returns an @c Iterator to the end of free arguments list.
     */
    Iterator end() {
	return m_free.end();
    }

    /**
     * Returns a @c ConstIterator to the end of free arguments list.
     */
    ConstIterator end() const {
	return m_free.end();
    }

    /**
     * Return the number of free arguments.
     */
    size_t numFreeArgs() const {
	return m_free.size();
    }

    /**
     * Returns whether there are any free arguments.
     */
    bool hasFreeArgs() const {
	return !m_free.empty();
    }
};

} /* namespace psynth */

#endif /* PSYNTH_ARGPARSER_H */
