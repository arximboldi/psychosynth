/**
 *  Time-stamp:  <2009-04-07 15:59:58 raskolnikov>
 *
 *  @file        util.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Apr  7 11:53:31 2009
 *
 *  Small utilities, most of them with almost no compilation overhead.
 *  Some of them where before in misc.hpp
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_UTIL_H_
#define PSYNTH_UTIL_H_

namespace psynth
{

template <template <class, class> class F, typename Y>
struct tpl_bind_snd
{
    template <class X>
    struct type : public F<X, Y>
    {};
};

template <template <class, class> class F, typename X>
struct tpl_bind_fst
{
    template <class Y>
    struct type : public F<X, Y>
    {};
};

/**
 * Inherit from this class if you want to forbid the copying of objects of
 * your class.
 */
class non_copyable
{
protected:
    /**
     * Forbids the creation of instances of NoCopy but allows instances of
     * its derivated..
     */
    non_copyable () {}

private:
    /**
     * Private copy constructor.
     */
    non_copyable (const non_copyable&);

    /**
     * Private assignment operator.
     */
    non_copyable& operator= (const non_copyable&);
};

} /* namespace psynth */


#endif /* PSYNTH_UTIL_H_ */
