/**
 *  Time-stamp:  <2010-10-17 19:56:06 raskolnikov>
 *
 *  @file        logger.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  A hierarchical, multi-backend and thread-safe logging facility.
 *  Interface file.
 */

/*
 *  Copyright (C) 2007, 2008, 2009 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_LOGGER_H
#define PSYNTH_LOGGER_H

#include <map>
#include <list>
#include <iostream>
#include <fstream>

#include <psynth/base/singleton.hpp>
#include <psynth/base/tree.hpp>

namespace psynth
{
namespace base
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
 *
 * @note This class is thread safe.
 * @todo Make thread-safety optional.
 *       Note that there are some thread-safety in some corner cases in all the
 *       new thread safe classes. Please, take a deep look at it and consider
 *       using recursive_mutex as a default.
 */
class log : public tree_node <log>,
	    public PSYNTH_DEFAULT_THREADING <log>
{
public:
    /**
     * The message relevance.
     */
    enum level
    {
	info,    /**< Usefull info to the user. */
	warning, /**< A problem that may cause inconvenience. */
	error,   /**< A problem that definetly causes inconvenience. */
	fatal    /**< A problem that from which we cannot recover. */
    };

    /**
     * Returns a string version of each level name.
     * @param level The level of which we want to now its string name.
     */
    static const char* level_name (int level)
    {
	switch(level) {
	case info:
	    return "INFO";
	case warning:
	    return "WARNING";
	case error:
	    return "ERROR";
	case fatal:
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
    void attach_sink (log_sink* d)
    {
	lock lock (this);
	m_dumpers.push_back(d);
    }

    /**
     * Dettachs a sink from this node.
     * @param d The sink we don't want to dump massages of this log anymore.
     */
    void dattach_sink (log_sink* d)
    {
	lock lock (this);
	m_dumpers.remove(d);
    }

    /**
     * Logs a message in a child of this node and all its parents.
     * @param child The child in which we want to log the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (const std::string& child, int level, const std::string& msg)
    {
	lock lock (this);
	get_path (child) (level, msg);
    }

    /**
     * Log a message into this node.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (int level, const std::string& msg)
    {
	lock lock (this);
	operator () (*this, level, msg);
    };

private:
    /**
     * Log a message into this node which has been propagated from
     * another log.
     * @param log The node that originated the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (log& log, int level, const std::string& msg);

    std::list<log_sink*> m_dumpers;
};

/**
 * Root singleton @c Log to log global messages.
 */
typedef singleton_holder<log> logger;

/**
 * Simple log sink that logs messages to @c cout and @c cerr.
 *
 * The messages will look like this:
 * [log_node_name] MSG_LEVEL : Message.
 *
 * Messages with relevales less or equal to @c INFO will be dumped to
 * @c cout and others to @c cerr.
 */
class log_std_sink : public log_sink
{
public:
    /**
     * Dums a message to the standard output.
     * @param log The log that originated the message.
     * @param level The relevance of the message.
     * @param msg The message to dump.
     */
    void dump (log& l, int level, const std::string& msg)
    {
	(level > log::info ? std::cerr : std::cout)
	    << '['
	    << l.get_path_name ()
	    << "] "
	    << log::level_name (level)
	    << ": "
	    << msg
	    << std::endl;
    }
};

/**
 * Logs messages to a standard stream.
 *
 * The messages will look like this:
 * [log_node_name] MSG_LEVEL : Message.
 *
 */

class log_stream_sink : public log_sink
{
public:
    /** Constructor */
    log_stream_sink ()
	: _output (0)
    {}

    /** Copy constructor */
    log_stream_sink (const log_stream_sink& s)
	: _output (s._output)
    {}

    /** Constructor with a stream */
    log_stream_sink (std::ostream& os)
	: _output (&os)
    {}
    
    /**
     * Dums a message to a stream.
     * @param log The log that originated the message.
     * @param level The relevance of the message.
     * @param msg The message to dump.
     */
    void dump (log& l, int level, const std::string& msg)
    {
	if (_output)
	    *(_output)
		<< '['
		<< l.get_path_name ()
		<< "] "
		<< log::level_name (level)
		<< ": "
		<< msg
		<< std::endl;
    }

protected:
    void set_stream (std::ostream& os)
    {
	_output = &os;
    }
    
private:
    std::ostream* _output;
};

/**
 * Outputs to stream file.
 */
class log_file_sink : public log_stream_sink
{
public:
    log_file_sink (const char* file,
		   std::ios_base::openmode mode =
		   std::ios_base::out | std::ios_base::app)
	: _file (file, mode)
    {
	set_stream (_file);
    }

private:
    std::ofstream _file;
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_LOGGER_H */
