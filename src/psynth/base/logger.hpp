/**
 *  Time-stamp:  <2012-04-02 23:38:49 raskolnikov>
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

#ifndef PSYNTH_BASE_LOGGER_H
#define PSYNTH_BASE_LOGGER_H

#include <map>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

#include <psynth/base/singleton.hpp>
#include <psynth/base/tree.hpp>
#include <psynth/base/compat.hpp>
#include <psynth/base/logger_fwd.hpp>

namespace psynth
{
namespace base
{

/**
 * A iostreams alike adapter for a log.
 */
class log_stream_adapter
{
public:
    /**
     * Constructor.
     * @param l The log where to output. Its lifetime should be
     * longer than the one of the log_stream_adapter
     */
    inline log_stream_adapter (log& l);
    inline log_stream_adapter (const log_stream_adapter&);
    inline log_stream_adapter (log_stream_adapter&&);

    /** Destructor. */
    inline ~log_stream_adapter ();

    /** Sets the level. */
    void set_level (int level)
    { _level = level; }

    /** Gests current level. */
    int level () const { return _level; }

    /** Gests current message. */
    const std::string message () const { return _str.str (); }

    /** Sends the message. */
    inline void flush ();

    /** Sends the message keeping it. */
    inline void dump ();

private:
    log&               _log;
    int                _level;
    std::ostringstream _str;

    template <typename T>
    friend log_stream_adapter& operator<< (log_stream_adapter&, const T&);
    friend log_stream_adapter& operator<< (
	log_stream_adapter&, log_stream_adapter&(*)(log_stream_adapter&));
};

/**
 * Manipulator for flushing the stream.
 */
inline log_stream_adapter& flush (log_stream_adapter& s)
{
    s.flush ();
    return s;
}

/**
 * This allow to wrap the log_stream_adapter into a wrapper that
 * flushes the log_stream when the last copy is destroyed.
 */
class log_stream_adapter_wrapper : public boost::noncopyable
{
public:
    /** Constructor */
    log_stream_adapter_wrapper (log_stream_adapter& s)
	: _stream (&s)
    {}

    /** Move constructor */
    log_stream_adapter_wrapper (log_stream_adapter_wrapper&& s)
	: _stream (s._stream)
    {
	s._stream = 0;
    }

    /** Destructor. Flushes message. */
    ~log_stream_adapter_wrapper ()
    {
	if (_stream)
	    *_stream << flush;
    }

    /** Returns the wrapped object. */
    log_stream_adapter& wrapped ()
    {
	return *_stream;
    }

private:
    log_stream_adapter* _stream;
};

template<typename T> inline
log_stream_adapter_wrapper operator<< (log_stream_adapter_wrapper s, T p)
{
    s.wrapped () << p;
    return std::move (s);
}

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
     * @todo Make it easier to add custom levels.
     */
    enum level
    {
        debug,   /**< Debug information interesting only for devels. */
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
	case debug:
            return "DEBUG";
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
    log ()
	: _stream (*this)
    {}

    /** Destructor. */
    ~log ();

    /**
     * Attachs a sink to this node.
     * @param d The sink that we want to dump this log's messages.
     */
    void add_sink (log_sink_ptr d);

    /**
     * Dettachs a sink from this node.
     * @param d The sink we don't want to dump massages of this log anymore.
     */
    void del_sink (log_sink_ptr d);

    /**
     * Logs a message in a child of this node and all its parents.
     * @param child The child in which we want to log the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (const std::string& child, int level, const std::string& msg)
    {
	lock lock (this);
	path (child) (level, msg);
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

    /**
     * Returns a stream object that can be used to send messages to
     * the log in a iostreams fashion.
     */
    log_stream_adapter_wrapper stream ()
    {
	return log_stream_adapter_wrapper (_stream);
    }

private:
    /**
     * Log a message into this node which has been propagated from
     * another log.
     * @param log The node that originated the message.
     * @param level The relevance of this message.
     * @param msg The message to log.
     */
    void operator () (log& log, int level, const std::string& msg);

    log_stream_adapter      _stream; // TODO: We want to avoid the
				     // ostringstream instance here :(
    std::list<log_sink_ptr> _dumpers;
};

/**
 * Root singleton @c Log to log global messages.
 */
struct logger : public singleton_holder<log> {};

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
	    << l.path_name ()
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
		<< l.path_name ()
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

void log_stream_adapter::flush ()
{
    _log (_level, _str.str ());
    _str.str ("");
}

void log_stream_adapter::dump ()
{
    _log (_level, _str.str ());
}

inline
log_stream_adapter& operator<< (log_stream_adapter& s, log::level x)
{
    s.set_level (x);
    return s;
}

template <class T> inline
log_stream_adapter& operator<< (log_stream_adapter& s, const T& x)
{
    s._str << x;
    return s;
}

inline log_stream_adapter& operator<< (
    log_stream_adapter& stm,
    log_stream_adapter&(*func)(log_stream_adapter&))
{
    return func (stm);
}

log_stream_adapter::log_stream_adapter (log& l)
    : _log (l)
    , _level (log::info)
{}

log_stream_adapter::log_stream_adapter (const log_stream_adapter& l)
    : _log (l._log)
    , _level (l._level)
{
    _str << l._str.str ();
}

log_stream_adapter::log_stream_adapter (log_stream_adapter&& l)
    : _log (l._log)
    , _level (l._level)
{
    _str << l._str.str (); // FIXME: SLOW!!!
}

log_stream_adapter::~log_stream_adapter ()
{
    // *this << log_msg;
}

#define PSYNTH_LOG \
    ::psynth::base::logger::self ().path (PSYNTH_MODULE_NAME).stream ()

#if PSYNTH_DEBUG
#define PSYNTH_LOG_DEBUG \
    ::psynth::base::logger::self ().path (PSYNTH_MODULE_NAME).stream () \
    << ::psynth::base::log::debug
#else
#define PSYNTH_LOG_DEBUG ::psynth::base::nop_ostream ()
#endif

extern template class tree_node <log>;
extern template class PSYNTH_DEFAULT_THREADING <log>;
extern template class singleton_holder <log>;

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_LOGGER_H */
