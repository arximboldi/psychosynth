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

#ifndef ARGPARSER_H
#define ARGPARSER_H

#include <string>	
#include <limits>
#include <list>
#include <map>
#include <ios>

class Option {
public:
    virtual ~Option() {}

    virtual bool parse(const char* arg) {
	return false;
    };
    
    virtual bool parse() {
	return false;
    };
};

struct atoi_func
{
    int operator() (const char* str) {
	return atoi(str);
    }
};

struct atof_func
{
    float operator() (const char* str) {
	return atof(str);
    }
};

template <class T>
struct atoany_func
{
    T operator() (const char* str) {
	return T(str);
    }
};

template <class T, class ConvFunc = atoany_func<T> >
class OptionGeneric : public Option
{
    T* m_data;
public:
    OptionGeneric(T* data) :
	m_data(data) {}

    bool parse(const char* arg) {
	*m_data = ConvFunc()(arg);
	return true;
    }
};

class OptionFlag : public Option {
    bool* m_flag;
public:
    OptionFlag(bool* f) :
	m_flag(f) {}

    bool parse() {
	*m_flag = true;
	return true;
    }
};

template <class T>
class OptionStream : public Option
{
    T* m_data;
public:
    OptionStream(T* data) :
	m_data(data) {}

    bool parse(const char* arg) {
	std::stringstream stream(std::string(arg), std::ios_base::in);
	stream >> *m_data;
	return true;
    }
};

typedef OptionGeneric<int, atoi_func> OptionInt;
typedef OptionGeneric<float, atof_func> OptionFloat;
typedef OptionGeneric<std::string> OptionString;
typedef OptionGeneric<const char*> OptionCString;

struct ltstr
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

class ArgParser
{
    /* TODO: Generate help screen?
    struc Item {
	Option* opt;
	std::string desc;
	char short_arg;
	std::string long_arg;
    };
    */
    
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
    typedef std::list<const char*>::iterator Iterator;
    typedef std::list<const char*>::const_iterator ConstIterator;
    
    ArgParser() {}
    ~ArgParser();

    void addOption(unsigned char flag, const char* str, int* data) {
	addOption(flag, str, new OptionInt(data));
    }
    
    void addOption(unsigned char flag, const char* str, float* data) {
	addOption(flag, str, new OptionFloat(data));
    }

    void addOption(unsigned char flag, const char* str, bool* data) {
	addOption(flag, str, new OptionFlag(data));
    }

    void addOption(unsigned char flag, const char* str, std::string* data) {
	addOption(flag, str, new OptionString(data));
    }

    void addOption(unsigned char flag, const char* str, const char** data) {
	addOption(flag, str, new OptionCString(data));
    }
    
    void addOption(unsigned char flag, const char* str, Option* op);
    
    void parseArgs(int argc, const char* argv[]);

    Iterator begin() {
	return m_free.begin();
    }

    ConstIterator begin() const {
	return m_free.begin();
    }

    Iterator end() {
	return m_free.end();
    }
    
    ConstIterator end() const {
	return m_free.end();
    }

    size_t numFreeArgs() const {
	return m_free.size();
    }
};


#endif /* ARGPARSER_H */
