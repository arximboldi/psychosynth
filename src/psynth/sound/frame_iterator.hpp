/**
 *  Time-stamp:  <2010-11-03 13:35:35 raskolnikov>
 *
 *  @file        frame_iterator.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 14:01:20 2010
 *
 *  Frame iterator.
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

/*
 *  Copyright 2005-2007 Adobe Systems Incorporated
 * 
 *  Use, modification and distribution are subject to the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 */

#ifndef PSYNTH_SOUND_FRAME_ITERATOR_H
#define PSYNTH_SOUND_FRAME_ITERATOR_H

#include <cassert>
#include <iterator>

#include <psynth/base/compat.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/util.hpp>
#include <psynth/sound/frame.hpp>

namespace psynth
{
namespace sound
{

/* forwarded declaration (as this file is included in
 * step_iterator.hpp) */
template <typename Iterator>
class memory_based_step_iterator;

template <typename Iterator> struct dynamic_step_type;

/**
   \brief metafunction predicate determining whether the given
   iterator is a plain one or an adaptor over another iterator.
   
   Examples of adaptors are the step iterator and the dereference
   iterator adaptor.
*/
template <typename It>
struct is_iterator_adaptor : public boost::mpl::false_{};

/**
   \brief returns the base iterator for a given iterator
   adaptor. Provide an specialization when introducing new iterator
   adaptors
*/
template <typename It>
struct iterator_adaptor_get_base;

/**
   \brief Changes the base iterator of an iterator adaptor. Provide an
   specialization when introducing new iterator adaptors
*/
template <typename It, typename NewBaseIt>
struct iterator_adaptor_rebind;

/**
   \brief Returns the type of an iterator just like the input
   iterator, except operating over immutable values
*/
template <typename It>
struct const_iterator_type;

/**
   The default implementation when the iterator is a C pointer is to
   use the standard constness semantics
*/
template <typename T>
struct const_iterator_type<T*>
{
    typedef const T* type;
};

template <typename T>
struct const_iterator_type<const T*>
{
    typedef const T* type;
};

/**
   \brief Metafunction predicate returning whether the given iterator
   allows for changing its values
   
   \ingroup GILIsMutable
*/
template <typename It>
struct iterator_is_mutable{};

/**
   The default implementation when the iterator is a C pointer is to
   use the standard constness semantics
*/
template <typename T> struct iterator_is_mutable<      T*> : public boost::mpl::true_ {};
template <typename T> struct iterator_is_mutable<const T*> : public boost::mpl::false_ {};

/**
   \defgroup FrameIteratorModelInterleavedPtr C pointer to a frame
   \ingroup FrameIteratorModel
   \brief Iterators over interleaved frames.
   A C pointer to a model of FrameValueConcept is used as an iterator
   over interleaved frames. Models FrameIteratorConcept,
   HomogeneousFrameBasedConcept, HasDynamicXStepTypeConcept,
   MemoryBasedIteratorConcept
*/


/*
 *
 *    HasDynamicStepTypeConcept
 *
 */

/** \ingroup FrameIteratorModelInterleavedPtr */
template <typename Frame>
struct dynamic_step_type<Frame*>
{
    typedef memory_based_step_iterator<Frame*> type;
};

/** \ingroup FrameIteratorModelInterleavedPtr */
template <typename Frame>
struct dynamic_step_type<const Frame*>
{
    typedef memory_based_step_iterator<const Frame*> type;
};


/*
 *
 *    FrameBasedConcept
 *
 */

template <typename Frame>
struct channel_space_type<Frame*> : public channel_space_type<Frame> {};
template <typename Frame>
struct channel_space_type<const Frame*> : public channel_space_type<Frame> {};

template <typename Frame>
struct sample_mapping_type<Frame*> : public sample_mapping_type<Frame> {};
template <typename Frame>
struct sample_mapping_type<const Frame*> : public sample_mapping_type<Frame> {};

template <typename Frame>
struct is_planar<Frame*> : public is_planar<Frame> {};
template <typename Frame>
struct is_planar<const Frame*> : public is_planar<Frame> {};


/*
 *
 *    HomogeneousFrameBasedConcept
 *
 */

template <typename Frame>
struct sample_type<Frame*> : public sample_type<Frame> {};
template <typename Frame>
struct sample_type<const Frame*> : public sample_type<Frame> {};


/**
   Support for frame iterator movement measured in memory units (bytes
   or bits) as opposed to frame type.
   Necessary to handle image row alignment and sample plane alignment.
*/

/*
 *
 *    MemoryBasedIteratorConcept
 *
 */

template <typename T>
struct byte_to_memunit : public boost::mpl::int_<1> {};

template <typename P>
inline std::ptrdiff_t memunit_step (const P*)
{
    return sizeof(P);
}

template <typename P>
inline std::ptrdiff_t memunit_distance (const P* p1, const P* p2)
{ 
    return (psynth_reinterpret_cast_c<const unsigned char*> (p2) -
	    psynth_reinterpret_cast_c<const unsigned char*> (p1)); 
}

template <typename P>
inline void memunit_advance (P* &p, std::ptrdiff_t diff)
{ 
    p = (P*) ((unsigned char*)(p) + diff);
}

template <typename P>
inline P* memunit_advanced (const P* p, std::ptrdiff_t diff)
{
    return (P*) ((char*)(p) + diff);
}

/**
   memunit_advanced_ref
   (shortcut to advancing a pointer by a given number of memunits and
   taking the reference in case the compiler is not smart enough)
*/
template <typename P>
inline P& memunit_advanced_ref (P* p, std::ptrdiff_t diff)
{
    return *memunit_advanced(p,diff);
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_FRAME_ITERATOR */
