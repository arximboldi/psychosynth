/**
 *  Time-stamp:  <2011-06-09 20:45:41 raskolnikov>
 *
 *  @file        factory.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri May 22 12:50:06 2009
 *
 *  Utilitarios para la construcción de factorías.
 *  @todo Aquí tal vez haya que generalizar algunas cosas...
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

#ifndef PSYNTH_BASE_FACTORY_HPP_
#define PSYNTH_BASE_FACTORY_HPP_

#include <psynth/base/type_traits.hpp>

namespace psynth
{
namespace base
{

template <class BasePtr, typename ...Args>
class factory_base
{
public:
    typedef typename pointee<BasePtr>::type base_type;
    typedef BasePtr pointer_type;

    virtual ~factory_base () {};
    virtual BasePtr create (Args... args) = 0;

    BasePtr operator () (Args... args)
    {
        return this->create (std::forward<Args> (args)...);
    }
};

template <class BasePtr, class Concrete, typename ...Args>
class factory : public factory_base <BasePtr, Args...>
{
public:
    typedef Concrete concrete_type;

    BasePtr create (Args... args)
    {
	return BasePtr (new concrete_type (std::forward<Args> (args) ...));
    }
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_FACTORY_HPP_ */
