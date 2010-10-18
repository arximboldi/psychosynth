/**
 *  Time-stamp:  <2010-10-17 19:50:45 raskolnikov>
 *
 *  @file        factory_manager.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri May 22 13:03:04 2009
 *
 *  Implementación de la clase factory_manager.
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

#ifndef PSYNTH_FACTORY_MANAGER_T_
#define PSYNTH_FACTORY_MANAGER_T_

#include <boost/function.hpp>
#include <psynth/base/factory.hpp>

namespace psynth
{
namespace base
{

template <class K, class B, class P>
template <class Concrete>
void factory_manager<K, B, P>::add (const K& k)
{
    m_map [k] = factory_functor<B, Concrete> ();
}

template <class K, class B, class P>
void factory_manager<K, B, P>::add (const K& k, factory_method fn)
{
    m_map [k] = fn;
}

template <class K, class B, class P>
void factory_manager<K, B, P>::del (const K& k)
{
    m_map.erase (k);
}

template <class K, class B, class P>
P factory_manager<K, B, P>::create (const K& k)
{
    typename factory_map::iterator it = m_map.find (k);

    if (it == m_map.end ())
	throw factory_error;

    return P ((it->second) ());
}

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_FACTORY_MANAGER_T_ */
