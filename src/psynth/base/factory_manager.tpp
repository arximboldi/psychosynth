/**
 *  Time-stamp:  <2011-06-09 20:45:58 raskolnikov>
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
#include <psynth/base/type_traits.hpp>

namespace psynth
{
namespace base
{

template <class K, class B, class... A>
template <class Concrete>
void restricted_factory_manager<K, B, A...>::add (const K& k)
{
    _map [k] = factory<B, Concrete, A...> ();
}

template <class K, class B, class... A>
void restricted_factory_manager<K, B, A...>::add (const K& k, factory_method fn)
{
    _map [k] = fn;
}

template <class K, class B, class... A>
void restricted_factory_manager<K, B, A...>::del (const K& k)
{
    _map.erase (k);
}

template <class K, class B, class... A>
B restricted_factory_manager<K, B, A...>::create (const K& k, A... args)
{
    typename factory_map::iterator it = _map.find (k);

    if (it == _map.end ())
	throw factory_error ();

    return B ((it->second) (std::forward<A> (args) ...));
}

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_FACTORY_MANAGER_T_ */
