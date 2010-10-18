/**
 *  Time-stamp:  <2010-10-17 19:48:24 raskolnikov>
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

#ifndef PSYNTH_FACTORY_H_
#define PSYNTH_FACTORY_H_

namespace psynth
{
namespace base
{

template <class Base>
class factory_functor_base
{
public:
    virtual ~factory_functor_base () {};
    virtual Base* operator () () = 0;    
};

template <class Base, class Concrete>
class factory_functor : public factory_functor_base <Base>
{
public:
    Concrete* operator () ()
    {
	return new Concrete;
    }
};

template <class Base>
class factory_base
{
public:
    virtual ~factory_base () {};
    virtual Base* create () = 0;
};

template <class Base, class Concrete>
class factory : public factory_base <Base>
{
public:
    Concrete* create ()
    {
	return new Concrete;
    }
};

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_FACTORY_H_ */
