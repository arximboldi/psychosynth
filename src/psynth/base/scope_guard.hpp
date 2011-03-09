/**
 *  Time-stamp:  <2011-03-09 01:25:51 raskolnikov>
 *
 *  @file        scope_guard.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 18:49:32 2011
 *
 *  Scope guard for calling a function on scope exit. Specially useful
 *  when writting C wrappers.
 *
 *  Based on the well known Loki::ScopeGuard library developed by
 *  Andrei Alexandrescu and throughly described here but adapted for
 *  easier integration with C++0x lambdas.
 *
 *  http://drdobbs.com/184403758
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_BASE_SCOPE_GUARD_H_
#define PSYNTH_BASE_SCOPE_GUARD_H_

#include <boost/utility.hpp>

namespace psynth
{
namespace base
{
 
class scope_guard_impl_base : public boost::noncopyable
{
public:
    void dismiss () const throw()
    { _dismissed = true; }

protected:
    scope_guard_impl_base ()
        : _dismissed (false) {}

    scope_guard_impl_base (scope_guard_impl_base&& other)
        : _dismissed (other._dismissed)
    { other.dismiss (); }
    
    ~scope_guard_impl_base () {} // nonvirtual to allow polymorphic
                                 // behaviour. read the alexandrescu
                                 // article for details.
    
    mutable bool _dismissed;
};

template <typename Fun>
class scope_guard_impl : public scope_guard_impl_base
{
    typedef scope_guard_impl_base base;
public:
    scope_guard_impl (const Fun& fun)
        : _fun (fun) {}

    scope_guard_impl (scope_guard_impl&& other)
        : base (std::move (other)) // TODO: Why is this needed?
        , _fun (other._fun)
    {}
    
    ~scope_guard_impl ()
    {
        if (!_dismissed)
            try { _fun (); } catch (...) { /* TODO: Handle error? */ }
    }
    
private:
    Fun _fun;
};

template <typename Fun>
scope_guard_impl<Fun> make_guard (const Fun& fun)
{
    return scope_guard_impl<Fun> (fun);
}

typedef const scope_guard_impl_base& scope_guard;

#define PSYNTH_CONCATENATE_DIRECT(s1, s2)  s1##s2
#define PSYNTH_CONCATENATE(s1, s2)         PSYNTH_CONCATENATE_DIRECT(s1, s2)
#define PSYNTH_ANONYMOUS_VARIABLE(str)     PSYNTH_CONCATENATE(str, __LINE__)

#define PSYNTH_ON_BLOCK_EXIT                                            \
    ::psynth::base::scope_guard PSYNTH_ANONYMOUS_VARIABLE(scope_guard) = \
        ::psynth::base::make_guard


} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_SCOPE_GUARD_H_ */
