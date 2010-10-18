/**
 *  Time-stamp:  <2010-10-17 19:49:57 raskolnikov>
 *
 *  @file        factory_manager.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri May 22 12:51:57 2009
 *
 *  Interfaz de la clase factory_manager.hpp
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

#ifndef PSYNTH_FACTORY_MANAGER_H_
#define PSYNTH_FACTORY_MANAGER_H_

#include <map>
#include <functional>
#include <psynth/base/exception.hpp>
#include <psynth/base/iterator.hpp>

namespace psynth
{
namespace base
{

PSYNTH_DECLARE_ERROR (psynth_error, factory_error);

template <class Key, class Base, typename Pointer = Base*>
class factory_manager
{
public:
    typedef Pointer ptr_type;
    typedef std::function<Base*()> factory_method;
    typedef map_key_iterator<Key, factory_method> iterator;
    
    template <class Concrete>
    void add (const Key& k);
    void add (const Key& k, factory_method fn);
    void del (const Key& k);
    Pointer create (const Key&);

    size_t size ()
    {
	return m_map.size ();
    }

    iterator begin ()
    {
	return m_map.begin ();
    }

    iterator end ()
    {
	return m_map.end ();
    }
    
private:
    typedef std::map<Key, factory_method> factory_map;
    factory_map m_map;
};

} /* namespace base */
} /* namespace psynth */

#include <psynth/base/factory_manager.tpp>

#endif /* PSYNTH_FACTORY_MANAGER_H_ */
