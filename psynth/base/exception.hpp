/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2009                          *
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

#ifndef PSYNTH_EXCEPTION_H_
#define PSYNTH_EXCEPTION_H_

#include <exception>

namespace psynth
{

class exception : public std::exception
{
public:
    virtual ~exception () throw () {}
    
    virtual void log () const
    {
	do_log (where (), what ());
    }
    
    virtual const char* what () const throw () = 0;
    
    virtual const char* where () const throw () = 0;

protected:
    void do_log (const char* where, const char* what) const;
    
private:
    const char* _where;
    const char* _what;
};

class error : public exception
{
public:    
    error (const char* where, const char* what) throw ()
	: _where (where)
	, _what (what)
    {}
    
    virtual ~error () throw () {}
    
    virtual const char* what () const throw ()
    {
	return _what;
    }

    virtual const char* where () const throw ()
    {
	return _where;
    }
    
private:
    const char* _where;
    const char* _what;
};

#define PSYNTH_ERROR(d_parent, d_error)					\
    class d_error : public d_parent					\
    {									\
    public:								\
	d_error () : d_parent () {}					\
    protected:								\
	d_error (const char* where, const char* what)			\
	    : d_parent (where, what) {}					\
    };

#define PSYNTH_ERROR_WHERE(d_parent, d_error, d_where)			\
    class d_error : public d_parent					\
    {									\
    public:								\
	d_error (const char* what) : d_parent (d_where, what) {}	\
    protected:								\
	d_error (const char* where, const char* what)			\
	    : d_parent (where, what) {}					\
    };

#define PSYNTH_ERROR_WHAT(d_parent, d_error, d_what)			\
    class d_error : public d_parent					\
    {									\
    public:								\
	d_error () : d_parent (d_what) {}				\
    protected:								\
	d_error (const char* where, const char* what)			\
	    : d_parent (where, what) {}					\
    };

#define PSYNTH_ERROR_WHERE_WHAT(d_parent, d_error, d_where, d_what)	\
    class d_error : public d_parent					\
    {									\
    public:								\
	d_error () : d_parent (d_where, d_what) {}			\
    protected:								\
	d_error (const char* where, const char* what)			\
	    : d_parent (where, what) {}					\
    };

PSYNTH_ERROR_WHERE (error, base_error, "psynth.base");

} /* namespace psynth */

#endif /* PSYNTH_EXCEPTION_H_ */
