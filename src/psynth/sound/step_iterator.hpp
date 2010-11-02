/**
 *  Time-stamp:  <2010-10-29 13:52:31 raskolnikov>
 *
 *  @file        step_iterator.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Oct 28 19:54:48 2010
 *
 *  Step iterator.
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

#ifndef GIL_STEP_ITERATOR_H
#define GIL_STEP_ITERATOR_H

#include <cstddef>
#include <iterator>
#include <boost/iterator/iterator_facade.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/util.hpp>
#include <psynth/sound/frame_iterator.hpp>
#include <psynth/sound/frame_iterator_adaptor.hpp>

namespace psynth
{
namespace sound
{

/**
   \defgroup FrameIteratorModelStepPtr step iterators
   \ingroup FrameIteratorModel
   \brief Iterators that allow for specifying the step between two
   adjacent values
*/

namespace detail
{

/**
  \ingroup FrameIteratorModelStepPtr
  \brief An adaptor over an existing iterator that changes the step
  unit

  (i.e. distance(it,it+1)) by a given predicate. Instead of calling
  base's operators ++, --, +=, -=, etc. the adaptor is using the
  passed policy object SFn for advancing and for computing the
  distance between iterators.

*/
template <typename Derived,  // type of the derived class
          typename Iterator, // Models Iterator
          typename SFn       // A policy object that can compute the
                             // distance between two iterators of type
                             // Iterator and can advance an iterator
                             // of type Iterator a given number of
                             // Iterator's units
	  >
class step_iterator_adaptor : public iterator_adaptor<
    Derived, Iterator, use_default, use_default, use_default,
    typename SFn::difference_type>
{
public:
    typedef iterator_adaptor<Derived, Iterator,
			     use_default, use_default, use_default,
			     typename SFn::difference_type> parent_type;

    typedef typename std::iterator_traits<Iterator>::difference_type
    base_difference_type;
    typedef typename SFn::difference_type  difference_type;
    typedef typename std::iterator_traits<Iterator>::reference reference;

    step_iterator_adaptor () {}
    
    step_iterator_adaptor (const Iterator& it, SFn step_fn = SFn ())
	: parent_type (it)
	, _step_fn (step_fn) {}

    difference_type step () const { return _step_fn.step(); }

protected:
    SFn _step_fn;

private:
    friend class boost::iterator_core_access;

    void increment ()
    { _step_fn.advance(this->base_reference(),1); }

    void decrement ()
    { _step_fn.advance(this->base_reference(),-1); }

    void advance (base_difference_type d)
    { _step_fn.advance(this->base_reference(),d); }

    difference_type distance_to(const step_iterator_adaptor& it) const
    {
	return _step_fn.difference (this->base_reference(),
				    it.base_reference());
    }
};

/**
   although iterator_adaptor defines these, the default implementation
   computes distance and compares for zero.  it is often faster to
   just apply the relation operator to the base
*/
template <typename D,typename Iterator,typename SFn> inline
bool operator> (const step_iterator_adaptor<D,Iterator,SFn>& p1,
		 const step_iterator_adaptor<D,Iterator,SFn>& p2)
{ 
    return p1.step() > 0 ? p1.base() > p2.base() : p1.base() < p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator< (const step_iterator_adaptor<D,Iterator,SFn>& p1,
		 const step_iterator_adaptor<D,Iterator,SFn>& p2)
{ 
    return p1.step()>0 ? p1.base()< p2.base() : p1.base()> p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator>= (const step_iterator_adaptor<D,Iterator,SFn>& p1,
		  const step_iterator_adaptor<D,Iterator,SFn>& p2)
{ 
    return p1.step()>0 ? p1.base()>=p2.base() : p1.base()<=p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator<= (const step_iterator_adaptor<D,Iterator,SFn>& p1,
		 const step_iterator_adaptor<D,Iterator,SFn>& p2)
{ 
    return p1.step()>0 ? p1.base()<=p2.base() : p1.base()>=p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator== (const step_iterator_adaptor<D,Iterator,SFn>& p1,
		 const step_iterator_adaptor<D,Iterator,SFn>& p2)
{ 
    return p1.base()==p2.base(); 
}

template <typename D,typename Iterator,typename SFn> inline
bool operator!= (const step_iterator_adaptor<D,Iterator,SFn>& p1,
		 const step_iterator_adaptor<D,Iterator,SFn>& p2)
{ 
    return p1.base()!=p2.base(); 
}

} /* namespace detail */


/*
 *
 *                 MEMORY-BASED STEP ITERATOR
 *
 */

/**
   \class memory_based_step_iterator
   \ingroup FrameIteratorModelStepPtr FrameBasedModel

   \brief Iterator with dynamically specified step in memory units
   (bytes or bits). Models StepIteratorConcept,
   IteratorAdaptorConcept, MemoryBasedIteratorConcept,
   FrameIteratorConcept, HasDynamicXStepTypeConcept

   A refinement of step_iterator_adaptor that uses a dynamic parameter
   for the step which is specified in memory units, such as bytes or
   bits

   Frame step iterators are used to provide iteration over
   non-adjacent frames.  Common use is a vertical traversal, where the
   step is the row stride.

   Another application is as a sub-sample view. For example, a red
   intensity image over interleaved RGB data would use a step iterator
   adaptor with step sizeof(sample_t)*3 In the latter example the step
   size could be fixed at compile time for efficiency.  Compile-time
   fixed step can be implemented by providing a step function object
   that takes the step as a template
*/

/**
   \ingroup FrameIteratorModelStepPtr

   \brief function object that returns the memory unit distance
   between two iterators and advances a given iterator a given number
   of mem units (bytes or bits)
*/
template <typename Iterator>
struct memunit_step_fn
{
    typedef std::ptrdiff_t difference_type;

    memunit_step_fn (difference_type step = memunit_step (Iterator()))
	: _step (step) {}

    difference_type difference (const Iterator& it1, const Iterator& it2) const
    {
	return memunit_distance (it1,it2) / _step;
    }

    void advance (Iterator& it, difference_type d) const
    {
	memunit_advance (it, d * _step);
    }
    
    difference_type step () const { return _step; }

    void set_step (std::ptrdiff_t step) { _step = step; }

private:
    PSYNTH_CLASS_REQUIRE(Iterator, psynth::sound, MemoryBasedIteratorConcept)
    difference_type _step;
};

template <typename Iterator>
class memory_based_step_iterator :
    public detail::step_iterator_adaptor<memory_based_step_iterator<Iterator>, 
					 Iterator, 
					 memunit_step_fn<Iterator> >
{
    PSYNTH_CLASS_REQUIRE(Iterator, psynth::sound, MemoryBasedIteratorConcept)

    public:
    typedef detail::step_iterator_adaptor<memory_based_step_iterator<Iterator>, 
                                          Iterator, 
                                          memunit_step_fn<Iterator> > parent_type;
    typedef typename parent_type::reference        reference;
    typedef typename parent_type::difference_type  difference_type;
    typedef Iterator                               base_iterator;
    
    memory_based_step_iterator ()
	: parent_type (Iterator()) {}
    memory_based_step_iterator (Iterator it, std::ptrdiff_t memunit_step)
	: parent_type(it, memunit_step_fn<Iterator>(memunit_step)) {}

    template <typename I2> 
    memory_based_step_iterator (const memory_based_step_iterator<I2>& it)
        : parent_type(it.base(), memunit_step_fn<Iterator>(it.step())) {}

    /**
       For some reason operator[] provided by iterator_adaptor
       returns a custom class that is convertible to reference We
       require our own reference because it is registered in
       iterator_traits
    */
    reference operator[] (difference_type d) const { return *(*this + d); }

    void set_step (std::ptrdiff_t memunit_step)
    { this->_step_fn.set_step (memunit_step); }

    base_iterator& base ()
    { return parent_type::base_reference (); }

    base_iterator const& base () const
    { return parent_type::base_reference (); }
};

template <typename Iterator>
struct const_iterator_type<memory_based_step_iterator<Iterator> >
{
    typedef memory_based_step_iterator<
	typename const_iterator_type<Iterator>::type>  type;
};

template <typename Iterator>
struct iterator_is_mutable<memory_based_step_iterator<Iterator> >
    : public iterator_is_mutable<Iterator> {};


/*
 *
 *    IteratorAdaptorConcept
 *
 */

template <typename Iterator>
struct is_iterator_adaptor<memory_based_step_iterator<Iterator> > :
    public mpl::true_{};

template <typename Iterator>
struct iterator_adaptor_get_base<memory_based_step_iterator<Iterator> >
{
    typedef Iterator type;
};

template <typename Iterator, typename NewBaseIterator>
struct iterator_adaptor_rebind<memory_based_step_iterator<Iterator>,
			       NewBaseIterator>
{
    typedef memory_based_step_iterator<NewBaseIterator> type;
};


/*
 *
 *    FrameBasedConcept
 *
 */

template <typename Iterator>
struct channel_space_type<memory_based_step_iterator<Iterator> > :
    public channel_space_type<Iterator> {};

template <typename Iterator>
struct sample_mapping_type<memory_based_step_iterator<Iterator> > :
    public sample_mapping_type<Iterator> {};

template <typename Iterator>
struct is_planar<memory_based_step_iterator<Iterator> > :
    public is_planar<Iterator> {};

template <typename Iterator>
struct sample_type<memory_based_step_iterator<Iterator> > :
    public sample_type<Iterator> {};

/*
 *
 *    MemoryBasedIteratorConcept
 *
 */
template <typename Iterator>
struct byte_to_memunit<memory_based_step_iterator<Iterator> >
    : public byte_to_memunit<Iterator> {};

template <typename Iterator>
inline std::ptrdiff_t memunit_step (
    const memory_based_step_iterator<Iterator>& p) { return p.step(); }

template <typename Iterator>
inline std::ptrdiff_t memunit_distance (
    const memory_based_step_iterator<Iterator>& p1, 
    const memory_based_step_iterator<Iterator>& p2)
{ 
    return memunit_distance (p1.base(),p2.base()); 
}

template <typename Iterator>
inline void memunit_advance (
    memory_based_step_iterator<Iterator>& p, 
    std::ptrdiff_t diff)
{ 
    memunit_advance (p.base(), diff);
}

template <typename Iterator>
inline memory_based_step_iterator<Iterator> 
memunit_advanced (
    const memory_based_step_iterator<Iterator>& p, 
    std::ptrdiff_t diff)
{
    return memory_based_step_iterator<Iterator>(
	memunit_advanced(p.base(), diff),p.step());
}

template <typename Iterator>
inline typename std::iterator_traits<Iterator>::reference 
memunit_advanced_ref (
    const memory_based_step_iterator<Iterator>& p, 
    std::ptrdiff_t diff)
{ 
    return memunit_advanced_ref(p.base(), diff);
}


/*
 *
 *    HasDynamicStepTypeConcept
 *
 */

template <typename Iterator>
struct dynamic_step_type<memory_based_step_iterator<Iterator> >
{
    typedef memory_based_step_iterator<Iterator> type;
};

/* For step iterators, pass the function object to the base */
template <typename Iterator, typename Deref>
struct iterator_add_deref<memory_based_step_iterator<Iterator>, Deref>
{
    PSYNTH_CLASS_REQUIRE(Deref, psynth::sound, FrameDereferenceAdaptorConcept)

    typedef memory_based_step_iterator<
	typename iterator_add_deref<Iterator, Deref>::type> type;

    static type make (const memory_based_step_iterator<Iterator>& it,
		      const Deref& d)
    {
	return type (iterator_add_deref<Iterator, Deref>::make (
			 it.base(), d), it.step ());
    }
};


/*
 *
 *   make_step_iterator
 *
 */

template <typename I>
typename dynamic_step_type<I>::type
make_step_iterator(const I& it, std::ptrdiff_t step);

namespace detail
{

/* if the iterator is a plain base iterator (non-adaptor), wraps it in
   memory_based_step_iterator */
template <typename I> 
typename dynamic_step_type<I>::type make_step_iterator_impl (
    const I& it, std::ptrdiff_t step, mpl::false_)
{
    return memory_based_step_iterator<I> (it, step);
}

// If the iterator is compound, put the step in its base
template <typename I> 
typename dynamic_step_type<I>::type make_step_iterator_impl (
    const I& it, std::ptrdiff_t step, mpl::true_)
{
    return make_step_iterator (it.base (), step);
}

// If the iterator is memory_based_step_iterator, change the step
template <typename BaseIt> 
memory_based_step_iterator<BaseIt> make_step_iterator_impl(
    const memory_based_step_iterator<BaseIt>& it,
    std::ptrdiff_t step, mpl::true_)
{
    return memory_based_step_iterator<BaseIt> (it.base(), step);
}
}

/**
   \brief Constructs a step iterator from a base iterator and a step.

   To construct a step iterator from a given iterator Iterator and a
   given step, if Iterator does not already have a dynamic step, we
   wrap it in a memory_based_step_iterator. Otherwise we do a
   compile-time traversal of the chain of iterator adaptors to locate
   the step iterator and then set it step to the new one.

   The step iterator of Iterator is not always
   memory_based_step_iterator<Iterator>. For example, Iterator may
   already be a memory_based_step_iterator, in which case it will be
   inefficient to stack them; we can obtain the same result by
   multiplying their steps. Note that for Iterator to be a step
   iterator it does not necessarily have to have the form
   memory_based_step_iterator<J>.  The step iterator can be wrapped
   inside another iterator. Also, it may not have the type
   memory_based_step_iterator, but it could be a user-provided type.
*/
template <typename I>
// Models MemoryBasedIteratorConcept, HasDynamicXStepTypeConcept
typename dynamic_step_type<I>::type
make_step_iterator (const I& it, std::ptrdiff_t step)
{
    return detail::make_step_iterator_impl (
	it, step, typename is_iterator_adaptor<I>::type());
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_STEP_ITERATOR */
