/**
 *  Time-stamp: <2010-10-17 20:11:52 raskolnikov>
 *
 *  @file       singleton.tpp
 *  @author     Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date       Tue Apr  7 12:27:37 2009
 *
 *  Implementation of the singleton class and policies.
 *  This file is part of the Psychosynth project.
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

#ifndef PSYNTH_SINGLETON_T_
#define PSYNTH_SINGLETON_T_

#include <cassert>

namespace psynth
{
namespace base
{

template <class T,
	  template <class> class CP,
	  template <class> class LP,
	  template <class> class TP>
typename singleton_holder<T, CP, LP, TP>::instance_type
singleton_holder<T, CP, LP, TP>::m_instance = 0;

template <class T,
	  template <class> class CP,
	  template <class> class LP,
	  template <class> class TP>
bool singleton_holder<T, CP, LP, TP>::m_destroyed = false;

template <class T,
	  template <class> class CP,
	  template <class> class LP,
	  template <class> class TP>
T& singleton_holder<T, CP, LP, TP>::self ()
{
    if (!m_instance) {
	typename TP<T>::lock guard;

	if (!m_instance) {
	    if (m_destroyed) {
		LP<T>::on_dead_reference ();
		m_destroyed = false;
	    }

	    m_instance = CP<T>::create ();
	    LP<T>::schedule (m_instance, &destroy_singleton);
	}
    }

    return *m_instance;
}

template <class T,
	  template <class> class CP,
	  template <class> class LP,
	  template <class> class TP>
void singleton_holder<T, CP, LP, TP>::destroy_singleton ()
{
    assert (!m_destroyed);
    CP<T>::destroy (m_instance);
    m_instance  = 0;
    m_destroyed = true;
}

#ifndef PSYNTH_ATEXIT_FIXED
template <class T>
bool lifetime_phoenix<T>::m_destroyed = false;
#endif

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_SINGLETON_T_ */
