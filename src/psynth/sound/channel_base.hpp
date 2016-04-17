/**
 *  Time-stamp:  <2010-11-07 16:00:59 raskolnikov>
 *
 *  @file        channel_base.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Nov  1 18:39:33 2010
 *
 *  Frame class and related utilities.
 *
 *  @todo Reimplement with C++0x variadic templates?
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

#ifndef PSYNTH_SOUND_CHANNEL_BASE_HPP
#define PSYNTH_SOUND_CHANNEL_BASE_HPP

#include <cassert>
#include <type_traits>

#include <boost/mpl/range_c.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/utility/enable_if.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/util.hpp>
#include <psynth/sound/concept.hpp>

namespace psynth
{
namespace sound
{

// Forward-declare
template <typename P>
P* memunit_advanced(const P* p, std::ptrdiff_t diff);

// Forward-declare semantic_at_c
template <int K, typename ChannelBase>
typename boost::disable_if<
    boost::is_const <ChannelBase>,
    typename kth_semantic_element_reference_type<ChannelBase,K>::type>::type
semantic_at_c(ChannelBase& p);

template <int K, typename ChannelBase>
typename kth_semantic_element_const_reference_type<ChannelBase, K>::type
semantic_at_c (const ChannelBase& p);

// Forward declare element_reference_type
template <typename ChannelBase> struct element_reference_type;
template <typename ChannelBase> struct element_const_reference_type;

template <typename ChannelBase, int K>
struct kth_element_type;
template <typename ChannelBase, int K>
struct kth_element_type<const ChannelBase,K> :
	public kth_element_type<ChannelBase,K> {};

template <typename ChannelBase, int K>
struct kth_element_reference_type;
template <typename ChannelBase, int K>
struct kth_element_reference_type<const ChannelBase,K> :
	public kth_element_reference_type<ChannelBase,K> {};

template <typename ChannelBase, int K>
struct kth_element_const_reference_type;
template <typename ChannelBase, int K>
struct kth_element_const_reference_type<const ChannelBase,K> :
	public kth_element_const_reference_type<ChannelBase,K> {};

namespace detail
{

template <typename DstLayout, typename SrcLayout, int K>
struct mapping_transform
    : public boost::mpl::at<typename SrcLayout::sample_mapping,
                     typename detail::type_to_index<
			 typename DstLayout::sample_mapping,
			 boost::mpl::integral_c<int, K> >::type
                           >::type {};

/**
   \defgroup ChannelBaseModelHomogeneous detail::homogeneous_channel_base
   \ingroup ChannelBaseModel
   \brief A homogeneous channel base holding one channel element. Models
   HomogeneousChannelBaseConcept or HomogeneousChannelBaseValueConcept

   If the element type models Regular, this class models
   HomogeneousChannelBaseValueConcept.
*/

/**
   \brief A homogeneous channel base holding one channel element. Models
   HomogeneousChannelBaseConcept or HomogeneousChannelBaseValueConcept

   \ingroup ChannelBaseModelHomogeneous
*/
template <typename Element, typename Layout>
struct homogeneous_channel_base<Element, Layout, 1>
{
private:
    Element _v0;
public:
    typedef Layout layout;

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>) { return _v0; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>) const { return _v0; }

    homogeneous_channel_base () {}
    homogeneous_channel_base (Element v) : _v0(v) {}

    // grayscale frame values are convertible to sample type
    operator Element () const { return _v0; }

    template <typename E2, typename L2>
    homogeneous_channel_base (const homogeneous_channel_base<E2,L2,1>& c)
	: _v0(at_c<0>(c)) {}
};


/**
   \brief A homogeneous channel base holding two channel
   elements. Models HomogeneousChannelBaseConcept or
   HomogeneousChannelBaseValueConcept

   \ingroup ChannelBaseModelHomogeneous
*/
template <typename Element, typename Layout>
struct homogeneous_channel_base<Element,Layout,2> {
private:
    Element _v0, _v1;
public:
    typedef Layout layout;

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>)       { return _v0; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>) const { return _v0; }

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<1>)       { return _v1; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<1>) const { return _v1; }

    homogeneous_channel_base () {}
    explicit homogeneous_channel_base (Element v)
	: _v0 (v), _v1(v) {}

    homogeneous_channel_base (Element v0, Element v1)
	: _v0 (v0), _v1(v1) {}

    template <typename E2, typename L2>
    homogeneous_channel_base (const homogeneous_channel_base<E2,L2,2>& c)
	: _v0 (sound::at_c<mapping_transform<Layout, L2, 0>::value>(c))
	, _v1 (sound::at_c<mapping_transform<Layout, L2, 1>::value>(c)) {}

    // Support for l-value reference proxy copy construction
    template <typename E2, typename L2>
    homogeneous_channel_base (homogeneous_channel_base<E2,L2,2>& c)
	: _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
	, _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c)) {}

    // Support for planar_frame_iterator construction and dereferencing
    template <typename P> homogeneous_channel_base(P* p, bool)
	: _v0 (&semantic_at_c<0>(*p))
	, _v1 (&semantic_at_c<1>(*p))
    {
    }

    template <typename Ref> Ref deref () const
    {
        return Ref (*semantic_at_c<0>(*this),
		    *semantic_at_c<1>(*this));
    }

    // Support for planar_frame_reference offset constructor
    template <typename Ptr>
    homogeneous_channel_base(const Ptr& ptr, std::ptrdiff_t diff)
        : _v0 (*memunit_advanced (semantic_at_c<0>(ptr), diff)),
          _v1 (*memunit_advanced (semantic_at_c<1>(ptr), diff)) {}

    // Support for planar_frame_reference operator[]
    Element at_c_dynamic (std::size_t i) const
    {
        if (i == 0) return _v0;
        return _v1;
    }
};

/**
   \brief A homogeneous channel base holding three channel
   elements. Models HomogeneousChannelBaseConcept or
   HomogeneousChannelBaseValueConcept

   \ingroup ChannelBaseModelHomogeneous
*/
template <typename Element, typename Layout>
struct homogeneous_channel_base<Element,Layout,3>
{
private:
    Element _v0, _v1, _v2;

public:
    typedef Layout layout;

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>)       { return _v0; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>) const { return _v0; }

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<1>)       { return _v1; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<1>) const { return _v1; }

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<2>)       { return _v2; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<2>) const { return _v2; }

    homogeneous_channel_base () {}

    explicit homogeneous_channel_base (Element v)
	: _v0(v), _v1(v), _v2(v) {}

    homogeneous_channel_base (Element v0, Element v1, Element v2)
	: _v0(v0), _v1(v1), _v2(v2) {}

    template <typename E2, typename L2>
    homogeneous_channel_base (const homogeneous_channel_base<E2,L2,3>& c)
	: _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
	, _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c))
	, _v2(sound::at_c<mapping_transform<Layout,L2,2>::value>(c)) {}

    // Support for l-value reference proxy copy construction
    template <typename E2, typename L2>
    homogeneous_channel_base(      homogeneous_channel_base<E2,L2,3>& c)
	: _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
	, _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c))
	, _v2(sound::at_c<mapping_transform<Layout,L2,2>::value>(c)) {}

    // Support for planar_frame_iterator construction and dereferencing
    template <typename P> homogeneous_channel_base(P* p,bool)
	: _v0(&semantic_at_c<0>(*p))
        , _v1(&semantic_at_c<1>(*p))
        , _v2(&semantic_at_c<2>(*p)) {}

    template <typename Ref>
    Ref deref() const
    {
        return Ref(*semantic_at_c<0>(*this),
                   *semantic_at_c<1>(*this),
                   *semantic_at_c<2>(*this));
    }

    // Support for planar_frame_reference offset constructor
    template <typename Ptr>
    homogeneous_channel_base (const Ptr& ptr, std::ptrdiff_t diff)
        : _v0(*memunit_advanced(semantic_at_c<0>(ptr), diff))
	, _v1(*memunit_advanced(semantic_at_c<1>(ptr), diff))
	, _v2(*memunit_advanced(semantic_at_c<2>(ptr), diff)) {}

    // Support for planar_frame_reference operator[]
    Element at_c_dynamic(std::size_t i) const
    {
        switch (i) {
            case 0: return _v0;
            case 1: return _v1;
        }
        return _v2;
    }
};

/**
   \brief A homogeneous channel base holding four channel
   elements. Models HomogeneousChannelBaseConcept or
   HomogeneousChannelBaseValueConcept

   \ingroup ChannelBaseModelHomogeneous
*/
template <typename Element, typename Layout>
struct homogeneous_channel_base<Element,Layout,4>
{
private:
    Element _v0, _v1, _v2, _v3;

public:
    typedef Layout layout;

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>)       { return _v0; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<0>) const { return _v0; }

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<1>)       { return _v1; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<1>) const { return _v1; }

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<2>)       { return _v2; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<2>) const { return _v2; }

    typename element_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<3>)       { return _v3; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at (boost::mpl::int_<3>) const { return _v3; }

    homogeneous_channel_base () {}

    explicit homogeneous_channel_base (Element v)
	: _v0(v), _v1(v), _v2(v), _v3(v) {}

    homogeneous_channel_base (Element v0, Element v1, Element v2, Element v3)
	: _v0(v0), _v1(v1), _v2(v2), _v3 (v3) {}

    template <typename E2, typename L2>
    homogeneous_channel_base (const homogeneous_channel_base<E2,L2,4>& c)
	: _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
        , _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c))
        , _v2(sound::at_c<mapping_transform<Layout,L2,2>::value>(c))
        , _v3(sound::at_c<mapping_transform<Layout,L2,3>::value>(c)) {}

    // Support for l-value reference proxy copy construction
    template <typename E2, typename L2>
    homogeneous_channel_base (homogeneous_channel_base<E2,L2,4>& c)
	: _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
        , _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c))
        , _v2(sound::at_c<mapping_transform<Layout,L2,2>::value>(c))
        , _v3(sound::at_c<mapping_transform<Layout,L2,3>::value>(c)) {}

    // Support for planar_frame_iterator construction and dereferencing
    template <typename P> homogeneous_channel_base(P* p,bool)
	: _v0(&semantic_at_c<0>(*p))
        , _v1(&semantic_at_c<1>(*p))
        , _v2(&semantic_at_c<2>(*p))
        , _v3(&semantic_at_c<3>(*p)) {}

    template <typename Ref>
    Ref deref () const
    {
        return Ref (*semantic_at_c<0>(*this),
		    *semantic_at_c<1>(*this),
		    *semantic_at_c<2>(*this),
		    *semantic_at_c<3>(*this));
    }

    // Support for planar_frame_reference offset constructor
    template <typename Ptr>
    homogeneous_channel_base (const Ptr& ptr, std::ptrdiff_t diff)
        : _v0(*memunit_advanced(semantic_at_c<0>(ptr),diff))
	, _v1(*memunit_advanced(semantic_at_c<1>(ptr),diff))
	, _v2(*memunit_advanced(semantic_at_c<2>(ptr),diff))
	, _v3(*memunit_advanced(semantic_at_c<3>(ptr),diff)) {}

    // Support for planar_frame_reference operator[]
    Element at_c_dynamic(std::size_t i) const
    {
        switch (i) {
            case 0: return _v0;
            case 1: return _v1;
            case 2: return _v2;
        }
        return _v3;
    }
};

/**
   \brief A homogeneous channel base holding five channel
   elements. Models HomogeneousChannelBaseConcept or
   HomogeneousChannelBaseValueConcept

   \ingroup ChannelBaseModelHomogeneous
*/
template <typename Element, typename Layout>
struct homogeneous_channel_base<Element,Layout,5>
{
private:
    Element _v0, _v1, _v2, _v3, _v4;

public:
    typedef Layout layout;

    typename element_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<0>)       { return _v0; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<0>) const { return _v0; }

    typename element_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<1>)       { return _v1; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<1>) const { return _v1; }

    typename element_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<2>)       { return _v2; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<2>) const { return _v2; }

    typename element_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<3>)       { return _v3; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<3>) const { return _v3; }

    typename element_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<4>)       { return _v4; }

    typename element_const_reference_type<homogeneous_channel_base>::type
    at(boost::mpl::int_<4>) const { return _v4; }

    homogeneous_channel_base() {}

    explicit homogeneous_channel_base(Element v)
	: _v0(v), _v1(v), _v2(v), _v3(v), _v4(v) {}

    homogeneous_channel_base (Element v0, Element v1, Element v2, Element v3,
			      Element v4)
	: _v0(v0), _v1(v1), _v2(v2), _v3(v3), _v4(v4) {}

    template <typename E2, typename L2>
    homogeneous_channel_base (const homogeneous_channel_base<E2,L2,5>& c)
        : _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
        , _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c))
        , _v2(sound::at_c<mapping_transform<Layout,L2,2>::value>(c))
        , _v3(sound::at_c<mapping_transform<Layout,L2,3>::value>(c))
        , _v4(sound::at_c<mapping_transform<Layout,L2,4>::value>(c)) {}

    // Support for l-value reference proxy copy construction
    template <typename E2, typename L2>
    homogeneous_channel_base (homogeneous_channel_base<E2,L2,5>& c)
        : _v0(sound::at_c<mapping_transform<Layout,L2,0>::value>(c))
        , _v1(sound::at_c<mapping_transform<Layout,L2,1>::value>(c))
        , _v2(sound::at_c<mapping_transform<Layout,L2,2>::value>(c))
        , _v3(sound::at_c<mapping_transform<Layout,L2,3>::value>(c))
        , _v4(sound::at_c<mapping_transform<Layout,L2,4>::value>(c)) {}

    // Support for planar_frame_iterator construction and dereferencing
    template <typename P> homogeneous_channel_base(P* p,bool)
	: _v0(&semantic_at_c<0>(*p))
        , _v1(&semantic_at_c<1>(*p))
        , _v2(&semantic_at_c<2>(*p))
        , _v3(&semantic_at_c<3>(*p))
        , _v4(&semantic_at_c<4>(*p)) {}

    template <typename Ref> Ref deref() const
    {
        return Ref (*semantic_at_c<0>(*this),
		    *semantic_at_c<1>(*this),
		    *semantic_at_c<2>(*this),
		    *semantic_at_c<3>(*this),
		    *semantic_at_c<4>(*this));
    }

    // Support for planar_frame_reference offset constructor
    template <typename Ptr>
    homogeneous_channel_base (const Ptr& ptr, std::ptrdiff_t diff)
        : _v0(*memunit_advanced(semantic_at_c<0>(ptr),diff))
	, _v1(*memunit_advanced(semantic_at_c<1>(ptr),diff))
	, _v2(*memunit_advanced(semantic_at_c<2>(ptr),diff))
	, _v3(*memunit_advanced(semantic_at_c<3>(ptr),diff))
	, _v4(*memunit_advanced(semantic_at_c<4>(ptr),diff)) {}

    // Support for planar_frame_reference operator[]
    Element at_c_dynamic(std::size_t i) const
    {
        switch (i) {
            case 0: return _v0;
            case 1: return _v1;
            case 2: return _v2;
            case 3: return _v3;
        }
        return _v4;
    }
};

/*
  The following way of casting adjacent samples (the contents of
  channel_base) into an array appears to be unsafe -- there is no
  guarantee that the compiler won't add any padding between adjacent
  samples.  Note, however, that GIL _must_ be compiled with compiler
  settings ensuring there is no padding in the channel base structs.
  This is because the channel base structs must model the interleaved
  organization in memory. In other words, the client may have existing
  RGB image in the form "RGBRGBRGB..." and we must be able to
  represent it with an array of RGB channel bases (i.e. RGB frames)
  with no padding. We have tested with char/int/float/double samples
  on gcc and VC and have so far discovered no problem.  We have even
  tried using strange samples consisting of short + char (3
  bytes). With the default 4-byte alignment on VC, the size of this
  sample is padded to 4 bytes, so an RGB frame of it will be 4x3=12
  bytes. The code below will still work properly.  However, the client
  must nevertheless ensure that proper compiler settings are used for
  their compiler and their sample types.
*/

template <typename Element, typename Layout, int K>
typename element_reference_type<
    homogeneous_channel_base<Element,Layout,K> >::type
dynamic_at_c (homogeneous_channel_base<Element,Layout,K>& cb, std::size_t i)
{
    assert (i < K);
    return (psynth_reinterpret_cast<Element*> (&cb)) [i];
}

template <typename Element, typename Layout, int K>
typename element_const_reference_type<
    homogeneous_channel_base<Element,Layout,K> >::type
dynamic_at_c (const homogeneous_channel_base<Element,Layout,K>& cb,
	      std::size_t i)
{
    assert (i < K);
    return (psynth_reinterpret_cast_c<const Element*>(&cb))[i];
}

template <typename Element, typename Layout, int K>
typename element_reference_type<
    homogeneous_channel_base<Element&,Layout,K> >::type
dynamic_at_c(const homogeneous_channel_base<Element&,Layout,K>& cb,
	     std::size_t i)
{
    assert (i < K);
    return cb.at_c_dynamic (i);
}

template <typename Element, typename Layout, int K>
typename element_const_reference_type<
    homogeneous_channel_base<const Element&,Layout,K> >::type
dynamic_at_c (const homogeneous_channel_base<const Element&,Layout,K>& cb,
	      std::size_t i)
{
    assert (i < K);
    return cb.at_c_dynamic (i);
}

} /* namespace detail */

template <typename Element, typename Layout, int K1, int K>
struct kth_element_type<detail::homogeneous_channel_base<Element,Layout,K1>, K>
{
    typedef Element type;
};

template <typename Element, typename Layout, int K1, int K>
struct kth_element_reference_type<
    detail::homogeneous_channel_base<Element,Layout,K1>, K> :
    public boost::add_reference<Element> {};

template <typename Element, typename Layout, int K1, int K>
struct kth_element_const_reference_type<
    detail::homogeneous_channel_base<Element,Layout,K1>, K> :
    public boost::add_reference<typename boost::add_const<Element>::type> {};

/**
   \brief Provides mutable access to the K-th element, in physical order
   \ingroup ChannelBaseModelHomogeneous
*/
template <int K, typename E, typename L, int N> inline
typename boost::add_reference<E>::type
at_c (detail::homogeneous_channel_base<E,L,N>& p)
{
    return p.at (boost::mpl::int_<K>());
}

/** TODO: Check why this results into ambiguous. */
/**
   \brief Provides constant access to the K-th element, in physical order
   \ingroup ChannelBaseModelHomogeneous
*/
template <int K, typename E, typename L, int N> inline
typename boost::add_reference<typename boost::add_const<E>::type>::type
at_c (const detail::homogeneous_channel_base<E,L,N>& p)
{
    return p.at (boost::mpl::int_<K>());
}

namespace detail
{
    struct swap_fn
    {
        template <typename T>
	void operator() (T& x, T& y) const
	{
            using std::swap;
            swap (x,y);
        }
    };
}
template <typename E, typename L, int N> inline
void swap (detail::homogeneous_channel_base<E,L,N>& x,
	   detail::homogeneous_channel_base<E,L,N>& y)
{
    static_for_each (x, y, detail::swap_fn ());
}

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_CHANNEL_BASE */
