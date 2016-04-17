/**
 *  @file        exception.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2009
 *
 *  Basic exception facility that permits the ease definition of
 *  new exceptions that are hierarchically distributed and
 *  cooperate with the logging system.
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
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


#ifndef PSYNTH_EXCEPTION_H_
#define PSYNTH_EXCEPTION_H_

#include <exception>
#include <string>

namespace psynth
{
namespace base
{

/**
 * Base class for all the exceptions inside the framework.
 */
class exception
{
public:
    exception () = default;
    exception (const exception&) = default;
    exception (exception&&) = default;
    exception& operator= (const exception&) = default;
    exception& operator= (exception&&) = default;
    
    virtual ~exception () throw () {}

    /**
     * Logs the exception somewhere. Probably this somewhere is:
     * @code
     *   logger::self ().path (where ())
     * @endcode
     */
    virtual void log () const;

    /**
     * Returns a message in natural language describing the
     * exception and/or its context.
     */
    virtual const char* what () const throw () = 0;

    /**
     * Returns the module where the exception was produced.
     */
    virtual const char* where () const throw () = 0;

    /**
     * Returns the severity of the exception.
     */
    virtual int level () const throw () = 0;
};

/**
 * A base clase for the errors defined with PSYNTH_DECLARE_ERROR and
 * alike preprocessor commands.
 */
class error_base : public virtual exception,
                   public virtual std::exception
{
public:
    /** Constructor */
    error_base () = default;
    error_base (const error_base&) = default;
    error_base& operator= (const error_base&) = default;

    /** Constructor specifying the error message. */
    error_base (const std::string& what)
	: _what (what)
    {}
    
    virtual ~error_base () throw () {}

    virtual const char* what () const throw ()
    { return _what.empty () ? default_what () : _what.c_str (); }

    virtual const char* where () const throw ()
    { return "psynth"; }

    virtual int level () const throw ();

protected:
    /**
     * Returns a default message for the exception.
     */
    virtual const char* default_what () const throw ();

private:
    std::string _what;
};

int default_error_level () throw ();

#define PSYNTH_DECLARE_ERROR(d_parent, d_error)				\
    class d_error : public d_parent					\
    {									\
    public:								\
	typedef d_parent base_type;					\
	d_error () = default;                                           \
        d_error (const d_error&) = default;                             \
        d_error& operator= (const d_error&) = default;                  \
	d_error (const std::string& s) : base_type (s) {}		\
	virtual const char* where () const throw ();			\
    protected:								\
	virtual const char* default_what () const throw ();		\
    };

#define PSYNTH_DEFINE_ERROR_WHERE_WHAT(d_error, d_where, d_what)	\
    const char* d_error::where () const	throw ()			\
    { return d_where; }							\
    const char* d_error::default_what () const throw ()			\
    { return d_what; }

#define PSYNTH_DEFINE_ERROR_WHERE(d_error, d_where)			\
    const char* d_error::default_what () const throw ()			\
    { return base_type::default_what (); }				\
    const char* d_error::where () const	throw ()			\
    { return d_where; }							\

#define PSYNTH_DEFINE_ERROR_WHAT(d_error, d_what)			\
    const char* d_error::default_what () const throw ()			\
    { return d_what; }							\
    const char* d_error::where () const	throw ()			\
    { return PSYNTH_MODULE_NAME; }

#define PSYNTH_DEFINE_ERROR(d_error)					\
    const char* d_error::default_what () const	throw ()		\
    { return base_type::default_what (); }				\
    const char* d_error::where () const	throw ()			\
    { return PSYNTH_MODULE_NAME; }

PSYNTH_DECLARE_ERROR (error_base, error);

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_EXCEPTION_H_ */
