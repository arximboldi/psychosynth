/**
 *  Time-stamp:  <2010-11-07 16:07:07 raskolnikov>
 *
 *  @file        planar_frame_iterator.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 14:30:33 2010
 *
 *  Planar frame iterator.
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

#ifndef PSYNTH_SOUND_PLANAR_PTR_H
#define PSYNTH_SOUND_PLANAR_PTR_H

#include <cassert>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/frame.hpp>
#include <psynth/sound/step_iterator.hpp>

namespace psynth
{
namespace sound
{

/*
  forward declaration (as this file is included in
  planar_frame_reference.hpp)
*/
template <typename SampleReference, typename ChannelSpace> 
struct planar_frame_reference;

/**
   \defgroup ChannelBaseModelPlanarPtr planar_frame_iterator 
   \ingroup ChannelBaseModel
   \brief A homogeneous channel base whose element is a sample
   iterator. Models HomogeneousChannelBaseValueConcept
   
   This class is used as an iterator to a planar frame.
*/

/**
   \defgroup FrameIteratorModelPlanarPtr planar_frame_iterator
   \ingroup FrameIteratorModel
   \brief An iterator over planar frames. Models FrameIteratorConcept,
   HomogeneousFrameBasedConcept, MemoryBasedIteratorConcept,
   HasDynamicXStepTypeConcept
*/

/**
   \brief An iterator over planar frames. Models
   HomogeneousChannelBaseConcept, FrameIteratorConcept,
   HomogeneousFrameBasedConcept, MemoryBasedIteratorConcept,
   HasDynamicXStepTypeConcept
   
   Planar frames have sample data that is not consecutive in memory.
   To abstract this we use classes to represent references and
   pointers to planar frames.
   
   \ingroup FrameIteratorModelPlanarPtr ChannelBaseModelPlanarPtr
   FrameBasedModel
*/
template <typename SamplePtr, typename ChannelSpace>
struct planar_frame_iterator
    : public boost::iterator_facade<
    planar_frame_iterator<SamplePtr,ChannelSpace>,
    frame<typename std::iterator_traits<
	      SamplePtr>::value_type,
	  layout<ChannelSpace> >,
    std::random_access_iterator_tag,
    const planar_frame_reference<
	typename std::iterator_traits<
	    SamplePtr>::reference,
	ChannelSpace> >

    , public detail::homogeneous_channel_base<
    SamplePtr,
    layout<ChannelSpace>,
    boost::mpl::size<ChannelSpace>::value >
{
private:
    typedef
    boost::iterator_facade<
    planar_frame_iterator<SamplePtr,ChannelSpace>,
    frame<typename std::iterator_traits<SamplePtr>::value_type,
	  layout<ChannelSpace> >,
	      std::random_access_iterator_tag,
	      const planar_frame_reference<
		  typename std::iterator_traits<SamplePtr>::reference,
		  ChannelSpace> >
    parent_type;

    typedef detail::homogeneous_channel_base<
	SamplePtr, layout<ChannelSpace>,
	boost::mpl::size<ChannelSpace>::value>
    channel_base_parent_type;

    typedef typename std::iterator_traits<SamplePtr>::value_type sample_type;

public:
    typedef typename parent_type::value_type         value_type;
    typedef typename parent_type::reference          reference;
    typedef typename parent_type::difference_type    difference_type;

    planar_frame_iterator () : channel_base_parent_type (0) {} 
    planar_frame_iterator (bool) {}
    // constructor that does not fill with zero (for performance)

    planar_frame_iterator (const SamplePtr& v0,
			   const SamplePtr& v1)
	: channel_base_parent_type (v0, v1) {}
    
    planar_frame_iterator (const SamplePtr& v0,
			   const SamplePtr& v1,
			   const SamplePtr& v2)
	: channel_base_parent_type (v0, v1, v2) {}

    planar_frame_iterator (const SamplePtr& v0,
			   const SamplePtr& v1,
			   const SamplePtr& v2,
			   const SamplePtr& v3)
	: channel_base_parent_type (v0, v1, v2, v3) {}
    
    planar_frame_iterator (const SamplePtr& v0,
			   const SamplePtr& v1,
			   const SamplePtr& v2,
			   const SamplePtr& v3,
			   const SamplePtr& v4)
	: channel_base_parent_type (v0, v1, v2, v3, v4) {}

    template <typename IC1, typename C1> 
    planar_frame_iterator (const planar_frame_iterator<IC1,C1>& ptr)
	: channel_base_parent_type (ptr) {}


    /**
       Copy constructor and operator= from pointers to compatible
       planar frames or planar frame references.  That allow
       constructs like pointer = &value or pointer = &reference Since
       we should not override operator& that's the best we can do.
    */
    template <typename P> 
    planar_frame_iterator (P* pix)
	: channel_base_parent_type (pix, true)
    {
	boost::function_requires<FramesCompatibleConcept<P,value_type> >();
    }

    struct address_of
    {
	template <typename T>
	T* operator () (T& t) { return &t; }
    };
    
    template <typename P> 
    planar_frame_iterator& operator= (P* pix)
    {
	/*
	  HACK HACK HACK! TODO!:
	  Without this it seems that the next statement is optimized
	  away by the compiler. For some reason it feels that it has
	  no side effects or something. Why? I do not know...
	  See in unit tests hack_planar_frame_iterator_copy
	 */
	boost::function_requires<FramesCompatibleConcept<P, value_type> >();	
	static_transform (*pix, *this, address_of ());
		
	/*
	  PERFORMANCE_CHECK: Compare to this:
	  this->template semantic_at_c<0>()=&pix->template
	  semantic_at_c<0>();
	  this->template semantic_at_c<1>()=&pix->template
	  semantic_at_c<1>();
	  this->template semantic_at_c<2>()=&pix->template
	  semantic_at_c<2>();
	*/
        return *this;
    }

    /**
       For some reason operator[] provided by iterator_facade returns
       a custom class that is convertible to reference.
       
       We require our own reference because it is registered in
       iterator_traits.
    */
    reference operator[] (difference_type d) const
    {
	return memunit_advanced_ref (*this, d * sizeof (sample_type));
    }

    reference operator-> () const
    {
	return **this;
    }

    // PERFORMANCE_CHECK: Remove?
    bool operator< (const planar_frame_iterator& ptr) const
    {
	return sound::at_c<0>(*this) < sound::at_c<0>(ptr);
    }
    
    bool operator!= (const planar_frame_iterator& ptr) const
    {
	return sound::at_c<0>(*this) != sound::at_c<0>(ptr);
    }
    
private:
    friend class boost::iterator_core_access;

    void increment()
    {
	static_transform (*this, *this, detail::inc<SamplePtr>());
    }
    
    void decrement ()
    {
	static_transform (*this, *this, detail::dec<SamplePtr>());
    }
    
    void advance (ptrdiff_t d)
    {
	static_transform (
	    *this, *this,
	    std::bind2nd (detail::plus_asymmetric<SamplePtr, ptrdiff_t>(), d));
    }
    
    reference dereference () const
    {
	return this->template deref<reference> ();
    }

    ptrdiff_t distance_to (const planar_frame_iterator& it) const
    {
	return sound::at_c<0>(it) - sound::at_c<0>(*this);
    }
    
    bool equal (const planar_frame_iterator& it) const
    {
	return sound::at_c<0>(*this) == sound::at_c<0>(it);
    }
};

namespace detail
{

template <typename IC>
struct sample_iterator_is_mutable : public boost::mpl::true_ {};
template <typename T>
struct sample_iterator_is_mutable<const T*> : public boost::mpl::false_ {};

} /* namespace detail */

template <typename IC, typename C> 
struct const_iterator_type<planar_frame_iterator<IC,C> >
{ 
private:
    typedef typename std::iterator_traits<IC>::value_type sample_type;

public:
    typedef
    planar_frame_iterator<typename sample_traits<sample_type>::const_pointer,
			  C> type; 
};

/**
   The default implementation when the iterator is a C pointer is to
   use the standard constness semantics
*/
template <typename IC, typename C> 
struct iterator_is_mutable<planar_frame_iterator<IC,C> > :
	public detail::sample_iterator_is_mutable<IC> {};

/*
 *
 *    ChannelBasedConcept
 *
 */

template <typename IC, typename C, int K>  
struct kth_element_type<planar_frame_iterator<IC, C>, K>
{
    typedef IC type;
};

template <typename IC, typename C, int K>  
struct kth_element_reference_type<planar_frame_iterator<IC,C>, K> :
    public boost::add_reference<IC> {};

template <typename IC, typename C, int K>  
struct kth_element_const_reference_type<planar_frame_iterator<IC, C>, K> :
    public boost::add_reference<typename boost::add_const<IC>::type> {};


/*
 *
 *    HomogeneousFrameBasedConcept
 *
 */

template <typename IC, typename C>
struct channel_space_type<planar_frame_iterator<IC,C> >
{
    typedef C type;
};

template <typename IC, typename C>
struct sample_mapping_type<planar_frame_iterator<IC,C> > :
    public sample_mapping_type<
    typename planar_frame_iterator<IC,C>::value_type> {};

template <typename IC, typename C>
struct is_planar<planar_frame_iterator<IC,C> > : public boost::mpl::true_ {};

template <typename IC, typename C>
struct sample_type<planar_frame_iterator<IC,C> >
{
    typedef typename std::iterator_traits<IC>::value_type type;
};


/*
 *
 *    MemoryBasedIteratorConcept
 *
 */

template <typename IC, typename C>
inline std::ptrdiff_t memunit_step (const planar_frame_iterator<IC,C>&)
{
    return sizeof (typename std::iterator_traits<IC>::value_type);
}

template <typename IC, typename C>
inline std::ptrdiff_t memunit_distance (const planar_frame_iterator<IC,C>& p1,
					const planar_frame_iterator<IC,C>& p2)
{ 
    return memunit_distance (sound::at_c<0>(p1), sound::at_c<0>(p2)); 
}

template <typename IC>
struct memunit_advance_fn
{
    memunit_advance_fn (std::ptrdiff_t diff)
	: _diff (diff) {}

    IC operator () (const IC& p) const { return memunit_advanced (p, _diff); }

    std::ptrdiff_t _diff;
};

template <typename IC, typename C>
inline void memunit_advance (planar_frame_iterator<IC, C>& p,
			     std::ptrdiff_t diff)
{ 
    static_transform (p, p, memunit_advance_fn<IC> (diff));
}

template <typename IC, typename C>
inline planar_frame_iterator<IC,C> memunit_advanced (
    const planar_frame_iterator<IC,C>& p, std::ptrdiff_t diff)
{
    planar_frame_iterator<IC,C> ret = p;
    memunit_advance (ret, diff);
    return ret;
}

template <typename SamplePtr, typename ChannelSpace> inline
planar_frame_reference<typename std::iterator_traits<SamplePtr>::reference,
		       ChannelSpace> 
memunit_advanced_ref (const planar_frame_iterator<SamplePtr, ChannelSpace>& ptr,
		      std::ptrdiff_t diff)
{
    return planar_frame_reference<
	typename std::iterator_traits<SamplePtr>::reference,
	ChannelSpace> (ptr, diff);
}


/*
 *
 *    HasDynamicStepTypeConcept
 *
 */

template <typename IC, typename C>
struct dynamic_step_type<planar_frame_iterator<IC,C> >
{
    typedef memory_based_step_iterator<planar_frame_iterator<IC,C> > type;
};

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_PLANAR_FRAME_ITERATOR */
