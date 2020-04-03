/**
 *  Time-stamp:  <2020-04-03 12:16:12 raskolnikov>
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

PSYNTH_DECLARE_ERROR (error, factory_error);

namespace detail
{

/**
 *  Factory manager with private mutation can be mutated through this
 *  class.
 */
template <typename FactoryManager>
struct factory_access
{
    typedef typename FactoryManager::key_type key_type;
    typedef typename FactoryManager::factory_method factory_method;

    template <class Concrete>
    static void add (FactoryManager& self, const key_type& k)
    { self.template add <Concrete> (k); }

    static void add (FactoryManager& self, const key_type& k, factory_method fn)
    { self.template add (k, fn); }

    static void del (FactoryManager& self, const key_type& k)
    { self.del (k); }
};

} /* namespace detail */


/**
 *  A factory manager that is mutable through special
 *  detail::factory_access class.
 */
template <class Key, class BasePtr, typename ...Args>
class restricted_factory_manager
{
public:
    typedef Key                                key_type;
    typedef BasePtr                            pointer_type;
    typedef typename pointee<BasePtr>::type    base_type;

    typedef std::function<pointer_type (Args ...)> factory_method;
    typedef map_key_iterator<Key, factory_method>  iterator;

    pointer_type create (const Key&, Args...);

    size_t size ()
    { return _map.size (); }

    iterator begin ()
    { return _map.begin (); }

    iterator end ()
    { return _map.end (); }

protected:
    template <class Concrete>
    void add (const Key& k);
    void add (const Key& k, factory_method fn);
    void del (const Key& k);

private:
    friend class detail::factory_access<restricted_factory_manager>;
    typedef std::map<Key, factory_method> factory_map;
    factory_map _map;
};

template <class Key, class BasePtr, typename ...Args>
class factory_manager :
    public restricted_factory_manager <Key, BasePtr, Args...>
{
    typedef restricted_factory_manager <Key, BasePtr, Args...> base;
    typedef typename base::factory_method factory_method;

public:
    template <class Concrete>
    void add (const Key& k)
    { this->base::template add <Concrete> (k); }

    void add (const Key& k, factory_method fn)
    { this->base::add (k, fn); }

    void del (const Key& k)
    { this->base::del (k); }
};

template <class GlobalFactory, class Concrete>
struct factory_registrant : public boost::noncopyable
{
    typedef typename GlobalFactory::type factory_type;
    typedef typename factory_type::key_type key_type;

    factory_registrant (key_type k)
    {
	detail::factory_access<factory_type>::template add<Concrete> (
            GlobalFactory::self (), k);
    }
};

template <typename Key, typename BasePtr, typename... Args>
struct global_factory_manager :
	public singleton_holder<factory_manager<
                                    Key, BasePtr, Args...> >
{
    template <class Concrete>
    struct registrant : factory_registrant<global_factory_manager,
                                           Concrete>
    {
        registrant (Key k)
            : factory_registrant<global_factory_manager,
                                 Concrete> (k)
        {}
    };
};

template <typename Key, typename BasePtr, typename... Args>
struct restricted_global_factory_manager :
	public singleton_holder<restricted_factory_manager<
                                    Key, BasePtr, Args...> >
{
    template <class Concrete>
    struct registrant : factory_registrant<restricted_global_factory_manager,
                                           Concrete>
    {
        registrant (Key k)
            : factory_registrant<restricted_global_factory_manager,
                                 Concrete> (k)
        {}
    };
};


} /* namespace base */
} /* namespace psynth */

#include <psynth/base/factory_manager.tpp>

#endif /* PSYNTH_FACTORY_MANAGER_H_ */
