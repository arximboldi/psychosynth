/**
 *  Time-stamp:  <2010-10-20 19:48:45 raskolnikov>
 *
 *  @file        compat.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Oct 20 19:46:18 2010
 *
 *  Compiler compatibility functions.
 *  Some taken from boost::gil.
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
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

#ifndef PSYNTH_COMPAT_H_
#define PSYNTH_COMPAT_H_

#ifdef _DEBUG
#    define PSYNTH_FORCEINLINE inline
#else
#ifdef NDEBUG
#if   defined(_MSC_VER)
#    define PSYNTH_FORCEINLINE __forceinline
#elif defined(__GNUC__) && __GNUC__ > 3
#    define PSYNTH_FORCEINLINE inline __attribute__ ((always_inline))
#else
#    define PSYNTH_FORCEINLINE inline
#endif
#else
#    define PSYNTH_FORCEINLINE inline
#endif
#endif

/**
   Enable PSYNTH_NONWORD_POINTER_ALIGNMENT_SUPPORTED if your platform
   supports dereferencing on non-word memory boundary.
   
   Enabling the flag results in performance improvement
*/
#if !defined(__hpux) && !defined(sun) && \
    !defined(__sun) && !defined(__osf__)
#    define PSYNTH_NONWORD_POINTER_ALIGNMENT_SUPPORTED
#endif

#endif /* PSYNTH_COMPAT_H_ */
