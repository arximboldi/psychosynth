/**
 *  Time-stamp:  <2010-10-19 01:59:44 raskolnikov>
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
#include <psynth/base/singleton.hpp>

namespace psynth
{
namespace base
{

PSYNTH_DECLARE_ERROR (psynth_error, factory_error);

namespace detail
{

/**
 * Factory manager with private mutation can be mutated through this
 * class.
 */
class <typename FactoryManager>
class factory_access
{
    template <class Concrete>
    void add (FactoryManager& self,
	      const FactoryManager::key_type& k)
    {
	self.add <Concrete> (k)
    }

    void add (FactoryManager& self,
	      const FactoryManager::key_type& k,
	      FactoryManager::factory_method fn)
    {
	self.add (k, fn);
    }

    void del (FactoryManager& self, const Key& k)
    {
	self.del (k);
    }
};

}

/**
 * A factory manager that is mutable through special
 * detail::factory_access class.
 */
template <class Key, class Base, typename Pointer = Base*>
class resticted_factory_manager
{
public:
    typedef Key     key_type;
    typedef Pointer pointer_type;
    typedef Base    base_type;
    
    typedef std::function<ptr_type ()>            factory_method;
    typedef map_key_iterator<Key, factory_method> iterator;
    
    pointer_type create (const Key&);

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

protected:
    template <class Concrete>
    void add (const Key& k);

    void add (const Key& k, factory_method fn);

    void del (const Key& k);

private:
    friend class factory_access<restricted_factory_manager>;
    typedef std::map<Key, factory_method> factory_map;
    factory_map m_map;
};

template <class Key, class Base, typename Pointer = Base*>
class factory_manager : public restricted_factory_manager <Key, Base, Pointer>
{
    typedef restricted_factory_manager <Key, Base, Pointer> base;
    
public:
    template <class Concrete>
    void add (const Key& k)
    {
	base::add <Concrete> (k);
    }
    
    void add (const Key& k, factory_method fn)
    {
	base::add (k, fn);
    }
    
    void del (const Key& k)
    {
	base::del (k);
    }
};

template <class GlobalFactory>
struct factory_registrant : public boost::noncopyable
{
    registrant (GlobalFactory::type::key_type k)
    {
	detail::factory_access::add<Concrete> (GlobalFactory::self (), k);
    }
};

template <typename Key, typename Base, Pointer = Base*>
struct global_factory_manager :
	public singleton_holder<factory_manager<Key, Base, Pointer>>
{
    typedef factory_registrant<global_factory_manager> registrant;
};

template <typename Key, typename Base, Pointer = Base*>
struct restricted_global_factory_manager :
	public singleton_holder<restricted_factory_manager<Key, Base, Pointer>>
{
    typedef factory_registrant<restricted_global_factory_manager> registrant;
};


} /* namespace base */
} /* namespace psynth */

#include <psynth/base/factory_manager.tpp>

#endif /* PSYNTH_FACTORY_MANAGER_H_ */
