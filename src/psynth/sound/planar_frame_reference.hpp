/**
 *  Time-stamp:  <2010-11-03 13:41:20 raskolnikov>
 *
 *  @file        planar_frame_reference.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 15:13:15 2010
 *
 *  Planar frame reference.
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

#ifndef PSYNTH_SOUND_PLANAR_REF_H
#define PSYNTH_SOUND_PLANAR_REF_H

#include <boost/mpl/range_c.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/channel_base.hpp>
#include <psynth/sound/sample.hpp>
#include <psynth/sound/frame.hpp>
#include <psynth/sound/planar_frame_iterator.hpp>

namespace psynth
{
namespace sound
{

/**
   \defgroup ChannelBaseModelPlanarRef planar_frame_reference 
   \ingroup ChannelBaseModel
   \brief A homogeneous channel base whose element is a sample
   reference. Models HomogeneousChannelBaseConcept,
   HomogeneousFrameConcept.
   
   This class is used as a reference proxy to a planar frame.
*/

/**
   \defgroup FrameModelPlanarRef planar_frame_reference 
   \ingroup FrameModel
   \brief A reference proxy to a planar frame. Models
   HomogeneousChannelBaseConcept, HomogeneousFrameConcept.
*/

/**
   \ingroup FrameModelPlanarRef ChannelBaseModelPlanarRef
   FrameBasedModel
   \brief A reference proxy to a planar frame. Models:
   HomogeneousChannelBaseConcept, HomogeneousFrameConcept
   
   A reference to a planar frame is a proxy class containing
   references to each of the corresponding samples.
*/
template <typename SampleReference, typename ChannelSpace>
// SampleReference is a sample reference (const or mutable)
struct planar_frame_reference  : public detail::homogeneous_channel_base<
    SampleReference, layout<ChannelSpace>, boost::mpl::size<ChannelSpace>::value>
{
    typedef detail::homogeneous_channel_base<
	SampleReference, layout<ChannelSpace>, boost::mpl::size<ChannelSpace>::value>
    parent_type;
    
private:
    // These three are only defined for homogeneous frames
    typedef typename sample_traits<SampleReference>::value_type
    sample_type;
    typedef typename sample_traits<SampleReference>::const_reference
    sample_const_reference;

public:
    BOOST_STATIC_CONSTANT(
	bool, is_mutable = sample_traits<SampleReference>::is_mutable);

    typedef frame <sample_type, layout<ChannelSpace> >  value_type;
    typedef planar_frame_reference                      reference;
    typedef planar_frame_reference<sample_const_reference, ChannelSpace>
    const_reference;

    planar_frame_reference (SampleReference v0, SampleReference v1)
	: parent_type(v0,v1) {}

    planar_frame_reference (SampleReference v0, SampleReference v1,
			    SampleReference v2)
	: parent_type(v0,v1,v2) {}

    planar_frame_reference (SampleReference v0, SampleReference v1,
			    SampleReference v2, SampleReference v3)
	: parent_type(v0,v1,v2,v3) {}

    planar_frame_reference (SampleReference v0, SampleReference v1,
			    SampleReference v2, SampleReference v3,
			    SampleReference v4)
	: parent_type (v0,v1,v2,v3,v4) {}

    planar_frame_reference (SampleReference v0, SampleReference v1,
			    SampleReference v2, SampleReference v3,
			    SampleReference v4, SampleReference v5)
	: parent_type (v0,v1,v2,v3,v4,v5) {}

    template <typename P>
    planar_frame_reference(const P& p)
	: parent_type(p)
    { check_compatible<P>(); }

    // PERFORMANCE_CHECK: Is this constructor necessary?
    template <typename SampleV, typename Mapping>           
    planar_frame_reference (frame<SampleV,layout<ChannelSpace,Mapping> >& p)
	: parent_type(p)
    { check_compatible<frame<SampleV,layout<ChannelSpace,Mapping> > >();}

    // Construct at offset from a given location
    template <typename SamplePtr>
    planar_frame_reference (
	const planar_frame_iterator<SamplePtr,ChannelSpace>& p,
	std::ptrdiff_t diff)
	: parent_type (p,diff) {}

    const planar_frame_reference&
    operator= (const planar_frame_reference& p) const
    {
	static_copy (p, *this);
	return *this;
    }
    
    template <typename P>
    const planar_frame_reference& operator= (const P& p) const
    {
	check_compatible<P>();
	static_copy (p,*this);
	return *this;
    }
    
    /*
      This overload is necessary for a compiler implementing Core
      Issue 574 to prevent generation of an implicit copy assignment
      operator (the reason for generating implicit copy assignment
      operator is that according to Core Issue 574, a cv-qualified
      assignment operator is not considered "copy assignment
      operator").  EDG implemented Core Issue 574 starting with EDG
      Version 3.8. I'm not sure why they did it for a template member
      function as well.
    */
    
#if BOOST_WORKAROUND(__HP_aCC, >= 61700) || \
    BOOST_WORKAROUND(__INTEL_COMPILER, >= 1000)

    const planar_frame_reference& operator= (const planar_frame_reference& p)
    {
	static_copy (p, *this);
	return *this;
    }
    
    template <typename P>
    const planar_frame_reference& operator= (const P& p)
    {
	check_compatible<P>();
	static_copy (p, *this);
	return *this;
    }

#endif /* BOOST_WORKAROUND */

    template <typename P>
    bool operator==(const P& p) const
    {
	check_compatible<P>();
	return static_equal (*this, p);
    }
    
    template <typename P>
    bool operator!= (const P& p) const
    {
	return !(*this == p);
    }

    SampleReference operator[] (std::size_t i) const
    {
	return this->at_c_dynamic (i);
    }

    const planar_frame_reference* operator->() const
    {
	return this;
    }
    
private:
    template <typename Frame>
    static void check_compatible()
    {
	base::psynth_function_requires<FramesCompatibleConcept<
	    Frame, planar_frame_reference> >();
    }
};


/*
 *
 *    ChannelBasedConcept
 *
 */

template <typename SampleReference, typename ChannelSpace, int K>  
struct kth_element_type<planar_frame_reference<SampleReference,ChannelSpace>, K>
{
    typedef SampleReference type;
};

template <typename SampleReference, typename ChannelSpace, int K>  
struct kth_element_reference_type<
    planar_frame_reference<SampleReference,ChannelSpace>, K>
{
    typedef SampleReference type;
};

template <typename SampleReference, typename ChannelSpace, int K>  
struct kth_element_const_reference_type<planar_frame_reference<
					    SampleReference,ChannelSpace>, K> 
    : public boost::add_reference<typename std::add_const<SampleReference>::type> 
{
//    typedef typename sample_traits<SampleReference>::const_reference type;
};


/*
 *
 *    FrameConcept
 *
 */

/**
   \brief Metafunction predicate that flags planar_frame_reference as
   a model of FrameConcept. Required by FrameConcept 
   \ingroup FrameModelPlanarRef
*/
template <typename SampleReference, typename ChannelSpace>  
struct is_frame< planar_frame_reference<SampleReference,ChannelSpace> >
    : public boost::mpl::true_{};


/*
 *
 *    HomogeneousFrameBasedConcept
 *
 */

/**
   \brief Specifies the channel space type of a planar frame
   reference. Required by FrameBasedConcept
   \ingroup FrameModelPlanarRef
*/
template <typename SampleReference, typename ChannelSpace>  
struct channel_space_type<planar_frame_reference<SampleReference,ChannelSpace> >
{
    typedef ChannelSpace type;
}; 

/**
   \brief Specifies the channel space type of a planar frame
   reference. Required by FrameBasedConcept
   
   \ingroup FrameModelPlanarRef
*/
template <typename SampleReference, typename ChannelSpace>  
struct sample_mapping_type<planar_frame_reference<SampleReference, ChannelSpace> >
{
    typedef typename layout<ChannelSpace>::sample_mapping_t type;
}; 


/**
   \brief Specifies that planar_frame_reference represents a planar
   construct. Required by FrameBasedConcept
   
   \ingroup FrameModelPlanarRef
*/
template <typename SampleReference, typename ChannelSpace>  
struct is_planar<planar_frame_reference<SampleReference,ChannelSpace> > :
    boost::mpl::true_ {};

/**
   \brief Specifies the channel space type of a planar frame
   reference. Required by HomogeneousFrameBasedConcept
   
   \ingroup FrameModelPlanarRef
*/
template <typename SampleReference, typename ChannelSpace>  
struct sample_type<planar_frame_reference<SampleReference,ChannelSpace> >
{
    typedef typename sample_traits<SampleReference>::value_type type;
}; 

} /* namespace sound  */
} /* namespace psynth */

namespace std
{

/*
  We are forced to define swap inside std namespace because on some
  platforms (Visual Studio 8) STL calls swap qualified.
  
  swap with 'left bias': 
  - swap between proxy and anything
  - swap between value type and proxy
  - swap between proxy and proxy

  Having three overloads allows us to swap between different (but
  compatible) models of FrameConcept
  
*/

/**
   \brief  swap for planar_frame_reference
   \ingroup FrameModelPlanarRef
*/
template <typename CR, typename CS, typename R> inline
void swap (const psynth::sound::planar_frame_reference<CR,CS> x, R& y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::planar_frame_reference<CR,CS>::value_type> (x, y); 
}

/**
   \brief  swap for planar_frame_reference
   \ingroup FrameModelPlanarRef
*/
template <typename CR, typename CS> inline
void swap (typename psynth::sound::planar_frame_reference<CR,CS>::value_type& x,
	   const psynth::sound::planar_frame_reference<CR,CS> y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::planar_frame_reference<CR,CS>::value_type> (x, y); 
}


/**
   \brief  swap for planar_frame_reference
   \ingroup FrameModelPlanarRef
*/
template <typename CR, typename CS> inline
void swap (const psynth::sound::planar_frame_reference<CR,CS> x,
	   const psynth::sound::planar_frame_reference<CR,CS> y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::planar_frame_reference<CR,CS>::value_type>(x, y); 
}

} /* namespace std */

#endif /* PSYNTH_SOUND_PLANAR_FRAME_REF */
