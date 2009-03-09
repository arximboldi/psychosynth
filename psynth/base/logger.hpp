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

#ifndef PSYNTH_LOGGER_H
#define PSYNTH_LOGGER_H

#include <map>
#include <list>
#include <iostream>

#include <psynth/base/singleton.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/base/tree.hpp>

namespace psynth
{

class log;

/**
 * Interface of a log sink. A log sink is an object that implements how log
 * messages are actually delivered to the user.
 */
class log_sink
{
public:
    
    /**
     * Deliver a message to the user.
     * @param log The log node that originated the message.
     * @param level The relevance of the message.
     * @param msg The message.
     */
    virtual void dump (log& log, int level, const std::string& msg) = 0;
};

/**
 * A logger node. The log structure is actually a tree so log messages can be
 * categorized hierarchically. When a message its logged, the message is
 * propagated by all the log parents so, for example, you can set up your sinks
 * the root node if you want to listen for all messages. 
 */
class log : public tree_node <log>
{
    std::list<log_sink*> m_dumpers;
    
public:
    /**
     * The message relevance.
     */
    enum level {
	INFO,    /**< Usefull info to the user. */
	WARNING, /**< A problem that may cause inconvenience. */
	ERROR,   /**< A problem that definetly causes inconvenience. */
	FATAL    /**< A problem that from which we cannot recover. */
    };

    /**
     * Returns a string version of each level name.
     * @param level The level of which we want to now its string name.
     */
    static const char* level_name (int level) {
	switch(level) {
	case INFO:
	    return "INFO";
	case WARNING:
	    return "WARNING";
	case ERROR:
	    return "ERROR";
	case FATAL:
	    return "FATAL";
	default:
	    return "UNKNOWN";
	};
    };

    /** Constructor. */
    log () {}
    
    /** Destructor. */
    ~log ();

    /**
     * Attachs a sink to this node.
     * @param d The sink that we want to dump this log's messages.
     */
    void attach_sink (log_sink* d) {
	m_dumpers.push_back(d);
    }

    /**
     * Dettachs a sink from this node.
     * @param d The sink we don't want to dump massages of this log anymore.
     */
    void dattach_sink (log_sink* d) {
	m_dumpers.remove(d);
    }

    /**
     * Logs a message in a child of this node and all its parents.
     * @param child The child in which we want to log the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (const std::string& child, int level, const std::string& msg) {
	get_child (child) (level, msg);
    }

    /**
     * Log a message into this node.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (int level, const std::string& msg) {
	operator () (*this, level, msg);
    };

    /**
     * Log a message into this node which has been propagated from
     * another log.
     * @param log The node that originated the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (log& log, int level, const std::string& msg);
};

/**
 * Root singleton @c Log to log global messages.
 */
class logger : public singleton <logger>,
	       public log
{
    friend class singleton <logger>;

    /** Hidden constructor. */
    logger () {};

     /** Hidden destructor. */
    ~logger () {};
public:
};

/**
 * Simple log sink that logs messages to @c cout and @c cerr.
 *
 * The messages will look like this:
 * [log_node_name] MSG_LEVEL : Message.
 *
 * Messages with relevales less or equal to @c INFO will be dumped to
 * @c cout and others to @c cerr.
 */
class log_default_sink : public log_sink
{
public:
    /**
     * Dums a message to the standard output.
     * @param log The log that originated the message.
     * @param level The relevance of the message.
     * @param msg The message to dump.
     */
    void dump (log& l, int level, const std::string& msg) {
	(level > log::INFO ? std::cerr : std::cout)
	    << '['
	    << l.get_path_name ()
	    << "] "
	    << log::level_name (level)
	    << ": "
	    << msg
	    << std::endl;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_LOGGER_H */
