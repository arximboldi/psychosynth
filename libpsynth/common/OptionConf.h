/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007, 2008                    *
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

#ifndef PSYNTH_OPTIONCONF_H
#define PSYNTH_OPTIONCONF_H

#include <sstream>

#include <libpsynth/common/ArgParser.h>
#include <libpsynth/common/Config.h>

namespace psynth
{

template <class T>
class OptionConf : public Option
{
    ConfNode& m_node;
    
public:
    OptionConf(ConfNode& node) :
	m_node(node) {}

    bool parse(const char* arg) {
	T val;
	std::istringstream str(arg);
	str >> val;
	m_node.set(val);
	
	return true;
    }
};

template <>
class OptionConf<std::string> : public Option
{
    ConfNode& m_node;
    
public:
    OptionConf(ConfNode& node) :
	m_node(node) {}

    bool parse(const char* arg) {
	m_node.set(std::string(arg));
	return true;
    }
};

template<class T>
class OptionConfValue : public Option
{
    ConfNode& m_node;
    T m_val;
public:
    OptionConfValue(const T& val, ConfNode& node) :
	m_val(val), m_node(node) {}

    bool parse() {
	m_node.set(m_val);
	return true;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OPTIONCONF_H */
