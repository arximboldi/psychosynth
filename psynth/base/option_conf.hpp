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

#include <psynth/base/arg_parser.hpp>
#include <psynth/base/config.hpp>

namespace psynth
{

/**
 * An arg_parser option that receives an argument and stores it in a conf_node.
 * The conf_node will be of type @a T. The only requirement for @a T is that it
 * must be a valid type for a conf_node and that it must have defined its
 * flow input operator for @c istreams.
 */
template <class T>
class option_conf : public option
{
    conf_node& m_node;
    
public:
    /**
     * Constructor.
     * @param node The node to fill with the option argument.
     */
    option_conf(conf_node& node) :
	m_node(node) {}

    /**
     * Parse an argument.
     * @return @c true.
     */
    bool parse(const char* arg)
    {
	T val;
	std::istringstream str(arg);
	str >> val;
	m_node.set(val);
	
	return true;
    }
};

/**
 * Optimized version of OptionConf for strings.
 */
template <>
class option_conf<std::string> : public option
{
    conf_node& m_node;
    
public:
    /**
     * Constructor.
     * @param node The node to fill with the option argument.
     */
    option_conf(conf_node& node) :
	m_node(node) {}

    /**
     * Parse an argument.
     * @return @c true.
     */
    bool parse(const char* arg)
    {
	m_node.set (std::string(arg));
	return true;
    }
};

/**
 * An arg_parser option that sets a conf_node to a given value if the
 * option is parsed. It does not catch any argument so its more a flag
 * type option, it just sets the node to a value if the option is present
 * and leaves it unchanged otherwise.
 */
template<class T>
class option_conf_value : public option
{
public:
    /**
     * Constructor.
     * @param val The value to set the node to if the argument is present.
     * @param node The node to change if the the argument is present.
     */
    option_conf_value(const T& val, conf_node& node) :
	m_val(val), m_node(node) {}

    /**
     * Parse the argument.
     * @return @c true.
     */
    bool parse()
    {
	m_node.set(m_val);
	return true;
    }

private:
    T m_val;
    conf_node& m_node;
};

} /* namespace psynth */

#endif /* PSYNTH_OPTION_CONF_H */
