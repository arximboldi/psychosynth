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

#include <libpsynth/common/Singleton.h>
#include <libpsynth/common/Misc.h>

namespace psynth
{

class Log;

/**
 * Interface of a log sink. A log sink is an object that implements how log
 * messages are actually delivered to the user.
 */
class LogSink
{
public:
    /**
     * Deliver a message to the user.
     * @param log The log node that originated the message.
     * @param level The relevance of the message.
     * @param msg The message.
     */
    virtual void dump(Log& log, int level, const std::string& msg) = 0;
};

/**
 * A logger node. The log structure is actually a tree so log messages can be
 * categorized hierarchically. When a message its logged, the message is
 * propagated by all the log parents so, for example, you can set up your sinks
 * the root node if you want to listen for all messages. 
 */
class Log
{
    std::map<std::string, Log> m_childs;
    std::list<LogSink*> m_dumpers;
    Log* m_parent;
    std::string m_name;
    bool m_isinit;
    
public:
    /**
     * The message relevance.
     */
    enum Level {
	INFO,    /**< Usefull info to the user. */
	WARNING, /**< A problem that may cause inconvenience. */
	ERROR,   /**< A problem that definetly causes inconvenience. */
	FATAL    /**< A problem that from which we cannot recover. */
    };

    /**
     * Returns a string version of each level name.
     * @param level The level of which we want to now its string name.
     */
    static const char* levelName(int level) {
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
    Log() :
	m_parent(NULL), m_isinit(false) {}

    /** Destructor. */
    ~Log();

    /**
     * Initializes the log node by setting the log name and parent.
     * @param name The log node name.
     * @param parent Its parent.
     */
    void init(const std::string& name, Log* parent) {
	m_name = name;
	m_parent = parent;
	m_isinit = true;
    }

    /**
     * Returns whether the node has already been initialized. 
     */
    bool isInit() {
	return m_isinit;
    }

    /**
     * Returns the name of this log node.
     */
    const std::string& getName() const {
	return m_name;
    }

    /**
     * Attachs a sink to this node.
     * @param d The sink that we want to dump this log's messages.
     */
    void attachSink(LogSink* d) {
	m_dumpers.push_back(d);
    }

    /**
     * Dettachs a sink from this node.
     * @param d The sink we don't want to dump massages of this log anymore.
     */
    void dattachSink(LogSink* d) {
	m_dumpers.remove(d);
    }

    /**
     * Returns a pointer to the parent log of this node.
     */
    const Log* getParent() const {
	return m_parent;
    }

    /**
     * Returns a reference to the child of this node matching that name, or creates
     * it if does not exist.
     */
    Log& getChild(const std::string& name) {
	if (!m_childs[name].isInit())
	    m_childs[name].init(name, this);
	return m_childs[name];
    };

    /**
     * Returns a pointer to the parent log of this node, const version.
     */
    Log* getParent() {
	return m_parent;
    }

    /**
     * Logs a message in a child of this node and all its parents.
     * @param child The child in which we want to log the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void log(const std::string& child, int level, const std::string& msg) {
	getChild(child).log(level, msg);
    }

    /**
     * Returns the name of the full path from this log node to the root node.
     */
    std::string getPathName() {
	if (m_parent)
	    return m_parent->getPathName() + "/" + m_name; 
	else
	    return m_name;
    }

    /**
     * Log a message into this node.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void log(int level, const std::string& msg) {
	log(*this, level, msg);
    };

    /**
     * Log a message into this node which has been propagated from
     * another log.
     * @param log The node that originated the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void log(Log& log, int level, const std::string& msg);

    /**
     * Returns a reference to the child matching a given path. Note
     * that any non-existing nodes in the path are created.
     */
    Log& getPath(std::string path);
};

/**
 * Root singleton @c Log to log global messages.
 */
class Logger : public Singleton<Logger>,
	       public Log
{
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
class LogDefaultSink : public LogSink
{
public:
    /**
     * Dums a message to the standard output.
     * @param log The log that originated the message.
     * @param level The relevance of the message.
     * @param msg The message to dump.
     */
    void dump(Log& log, int level, const std::string& msg) {
	if (level > Log::INFO)
	    std::cerr << '[' << log.getPathName() << "] "
		      << Log::levelName(level)
		      << ": " << msg << std::endl;
	else
	    std::cout << '[' << log.getPathName() << "] "
		      << Log::levelName(level)
		      << ": " << msg << std::endl;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_LOGGER_H */


