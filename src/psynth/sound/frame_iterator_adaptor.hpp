/**
 *  Time-stamp:  <2010-11-05 02:37:03 raskolnikov>
 *
 *  @file        frame_iterator_adaptor.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 18:18:16 2010
 *
 *  Frame iterator adaptor.
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

#ifndef PSYNTH_SOUND_FRAME_ITERATOR_ADAPTOR_H
#define PSYNTH_SOUND_FRAME_ITERATOR_ADAPTOR_H

#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/concept.hpp>
#include <psynth/sound/frame_iterator.hpp>

namespace psynth
{
namespace sound
{

/**
   \defgroup FrameIteratorModelDerefPtr dereference_iterator_adaptor
   \ingroup FrameIteratorModel
   \brief An iterator that invokes a provided function object upon
   dereference. Models: IteratorAdaptorConcept, FrameIteratorConcept
*/

/**
   \ingroup FrameIteratorModelDerefPtr FrameBasedModel
   \brief An adaptor over an existing iterator that provides for
   custom filter on dereferencing the object. Models:
   IteratorAdaptorConcept, FrameIteratorConcept
*/

template <typename Iterator,
	  // Models Iterator
          typename DFn
	  // Models Returns the result of dereferencing a given
	  // iterator of type Iterator
	  >
class dereference_iterator_adaptor :
	public boost::iterator_adaptor<dereference_iterator_adaptor<Iterator, DFn>,
				       Iterator,
				       typename DFn::value_type,
				       typename std::iterator_traits<
					   Iterator>::iterator_category,
				       typename DFn::reference,
				       boost::use_default>
{
    DFn _deref_fn;

public:
    typedef boost::iterator_adaptor<dereference_iterator_adaptor<Iterator,DFn>,
				    Iterator,
				    typename DFn::value_type,
				    typename std::iterator_traits<
					Iterator>::iterator_category,
				    typename DFn::reference,
				    boost::use_default> parent_type;
    typedef typename DFn::result_type  reference;
    typedef typename std::iterator_traits<Iterator>::difference_type
    difference_type;
    typedef DFn                        dereference_fn;

    dereference_iterator_adaptor () {}

    template <typename Iterator1> 
    dereference_iterator_adaptor (
	const dereference_iterator_adaptor<Iterator1,DFn>& dit)
	: parent_type (dit.base())
	, _deref_fn (dit._deref_fn)
    {}
    
    dereference_iterator_adaptor (Iterator it, DFn deref_fn = DFn())
	: parent_type (it)
	, _deref_fn (deref_fn)
    {}
    
    template <typename Iterator1, typename DFn1> 
    dereference_iterator_adaptor (
	const dereference_iterator_adaptor<Iterator1,DFn1>& it)
	: parent_type (it.base())
	, _deref_fn (it._deref_fn)
    {}
    
    /**
       For some reason operator[] provided by iterator_facade returns
       a custom class that is convertible to reference We require our
       own reference because it is registered in iterator_traits
    */
    reference operator[] (difference_type d) const { return *(*this + d); }

    /*
      although iterator_adaptor defines these, the default
      implementation computes distance and compares for zero.  it is
      often faster to just apply the relation operator to the base
    */
    bool operator> (const dereference_iterator_adaptor& p) const
    { return this->base_reference() > p.base_reference(); }
    
    bool operator< (const dereference_iterator_adaptor& p) const
    { return this->base_reference() < p.base_reference(); }
    
    bool operator>= (const dereference_iterator_adaptor& p) const
    { return this->base_reference() >= p.base_reference(); }
    
    bool operator<= (const dereference_iterator_adaptor& p) const
    { return this->base_reference() <= p.base_reference(); }

    bool operator== (const dereference_iterator_adaptor& p) const
    { return this->base_reference() == p.base_reference(); }

    bool operator!= (const dereference_iterator_adaptor& p) const
    { return this->base_reference() != p.base_reference(); }

    Iterator& base ()              { return this->base_reference (); }
    const Iterator& base () const  { return this->base_reference (); }

    const DFn& deref_fn () const { return _deref_fn; }

private:
    template <typename Iterator1, typename DFn1> 
    friend class dereference_iterator_adaptor;
    friend class boost::iterator_core_access;

    reference dereference () const
    { return _deref_fn(*(this->base_reference())); }
};

template <typename I, typename DFn> 
struct const_iterator_type<dereference_iterator_adaptor<I,DFn> >
{ 
    typedef dereference_iterator_adaptor<
	typename const_iterator_type<I>::type,typename DFn::const_type> type; 
};

template <typename I, typename DFn> 
struct iterator_is_mutable<dereference_iterator_adaptor<I, DFn> > :
	public boost::mpl::bool_<DFn::is_mutable> {};


template <typename I, typename DFn>
struct is_iterator_adaptor<dereference_iterator_adaptor<I,DFn> > :
    public boost::mpl::true_ {};

template <typename I, typename DFn>
struct iterator_adaptor_get_base<dereference_iterator_adaptor<I,DFn> >
{
    typedef I type;
};

template <typename I, typename DFn, typename NewBaseIterator>
struct iterator_adaptor_rebind <dereference_iterator_adaptor<I,DFn>,
				NewBaseIterator>
{
    typedef dereference_iterator_adaptor<NewBaseIterator,DFn> type;
};


/*
 *
 *    FrameBasedConcept
 *
 */

template <typename I, typename DFn>
struct channel_space_type<dereference_iterator_adaptor<I,DFn> > :
    public channel_space_type<typename DFn::value_type> {};

template <typename I, typename DFn>
struct sample_mapping_type<dereference_iterator_adaptor<I,DFn> > :
    public sample_mapping_type<typename DFn::value_type> {};

template <typename I, typename DFn>
struct is_planar<dereference_iterator_adaptor<I,DFn> > :
    public is_planar<typename DFn::value_type> {};

template <typename I, typename DFn>
struct sample_type<dereference_iterator_adaptor<I,DFn> > :
    public sample_type<typename DFn::value_type> {};


/*
 *
 *    MemoryBasedIteratorConcept
 *
 */

template <typename Iterator, typename DFn>
struct byte_to_memunit<dereference_iterator_adaptor<Iterator,DFn> > :
    public byte_to_memunit<Iterator> {};

template <typename Iterator, typename DFn>
inline typename std::iterator_traits<Iterator>::difference_type 
memunit_step(const dereference_iterator_adaptor<Iterator, DFn>& p)
{ 
    return memunit_step (p.base());
}

template <typename Iterator, typename DFn>
inline typename std::iterator_traits<Iterator>::difference_type 
memunit_distance (const dereference_iterator_adaptor<Iterator,DFn>& p1, 
		  const dereference_iterator_adaptor<Iterator,DFn>& p2)
{ 
    return memunit_distance (p1.base(), p2.base()); 
}

template <typename Iterator, typename DFn>
inline void memunit_advance(
    dereference_iterator_adaptor<Iterator,DFn>& p, 
    typename std::iterator_traits<Iterator>::difference_type diff)
{ 
    memunit_advance (p.base(), diff);
}

template <typename Iterator, typename DFn>
inline dereference_iterator_adaptor<Iterator,DFn> 
memunit_advanced (
    const dereference_iterator_adaptor<Iterator,DFn>& p, 
    typename std::iterator_traits<Iterator>::difference_type diff)
{ 
    return dereference_iterator_adaptor<Iterator,DFn> (
	memunit_advanced (p.base(), diff), p.deref_fn ()); 
}

template <typename Iterator, typename DFn> inline 
typename std::iterator_traits<
    dereference_iterator_adaptor<Iterator,DFn> >::reference 
memunit_advanced_ref (
    const dereference_iterator_adaptor<Iterator,DFn>& p, 
    typename std::iterator_traits<Iterator>::difference_type diff)
{ 
    return * memunit_advanced (p, diff);
}


/*
 *
 *    HasDynamicStepTypeConcept
 *
 */

template <typename Iterator, typename DFn>
struct dynamic_step_type<dereference_iterator_adaptor<Iterator,DFn> >
{
    typedef dereference_iterator_adaptor<
	typename dynamic_step_type<Iterator>::type, DFn> type;
};

/**
   \brief Returns the type (and creates an instance) of an iterator
   that invokes the given dereference adaptor upon dereferencing
   
   \ingroup FrameIteratorModelDerefPtr
*/
template <typename Iterator, typename Deref>
struct iterator_add_deref
{
    PSYNTH_CLASS_REQUIRE(Deref, psynth::sound, FrameDereferenceAdaptorConcept)

    typedef dereference_iterator_adaptor<Iterator, Deref> type;

    static type make (const Iterator& it, const Deref& d)
    { return type (it, d); }
};

/**
   \ingroup FrameIteratorModelDerefPtr

   \brief For dereference iterator adaptors, compose the new function
   object after the old one
*/
template <typename Iterator, typename PrevDeref, typename Deref>
struct iterator_add_deref<
    dereference_iterator_adaptor<Iterator, PrevDeref>, Deref>
{
    // PSYNTH_CLASS_REQUIRE(Deref, psynth::sound, FrameDereferenceAdaptorConcept)

    typedef dereference_iterator_adaptor<
	Iterator, deref_compose<Deref, PrevDeref> > type;

    static type make (
	const dereference_iterator_adaptor<Iterator, PrevDeref>& it,
	const Deref& d)
    { 
        return type (it.base(),
		     deref_compose<Deref, PrevDeref>(d, it.deref_fn ())); 
    }
};

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_ITERATOR_ADAPTOR */
