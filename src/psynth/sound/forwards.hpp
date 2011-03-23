/**
 *  Time-stamp:  <2011-03-08 23:52:17 raskolnikov>
 *
 *  @file        forwards.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 23:47:47 2011
 *
 *  Forward declare associated metafunctions and functions to common
 *  concepts.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_SOUND_FORWARDS_H_
#define PSYNTH_SOUND_FORWARDS_H_

#include <boost/type_traits.hpp>
#include <boost/utility.hpp>

namespace psynth
{
namespace sound
{

template <typename T> struct dynamic_step_type;

template <typename T> struct sample_traits;
template <typename P> struct is_frame;

template <typename dstT, typename srcT>
typename sample_traits<dstT>::value_type sample_convert (const srcT& val);

template <typename ChannelBase, int K> struct
kth_element_type;
template <typename ChannelBase, int K> struct
kth_element_reference_type;
template <typename ChannelBase, int K> struct
kth_element_const_reference_type;
template <typename ChannelBase, int K>
struct kth_semantic_element_reference_type;
template <typename ChannelBase, int K>
struct kth_semantic_element_const_reference_type;

template <typename ChannelBase> struct size;
template <typename ChannelBase> struct element_type;
template <typename T> struct sample_type;
template <typename T> struct channel_space_type;
template <typename T> struct sample_mapping_type;
template <typename T> struct is_planar;
template <typename T> struct num_samples;

template <typename It> struct const_iterator_type;
template <typename It> struct iterator_is_mutable;
template <typename It> struct is_iterator_adaptor;
template <typename It, typename NewBaseIt> struct iterator_adaptor_rebind;
template <typename It> struct iterator_adaptor_get_base;

/* forward-declare at_c */
namespace detail
{

template <typename Element, typename Layout, int K>
struct homogeneous_channel_base;

} /* namespace detail */

template <int K, typename E, typename L, int N>
typename boost::add_reference<E>::type at_c (
    detail::homogeneous_channel_base<E,L,N>& p);

template <int K, typename E, typename L, int N>
typename boost::add_reference<typename boost::add_const<E>::type>::type at_c (
    const detail::homogeneous_channel_base<E,L,N>& p);

#if !defined(_MSC_VER)  || _MSC_VER > 1310
template <typename P, typename C, typename L> struct packed_frame;
template <int K, typename P, typename C, typename L>
typename kth_element_reference_type<packed_frame<P,C,L>, K>::type 
at_c (packed_frame<P,C,L>& p);

template <int K, typename P, typename C, typename L>
typename kth_element_const_reference_type<packed_frame<P,C,L>,K>::type 
at_c (const packed_frame<P,C,L>& p);

template <typename B, typename C, typename L, bool M>
struct bit_aligned_frame_reference;

template <int K, typename B, typename C, typename L, bool M> inline
typename kth_element_reference_type<
    bit_aligned_frame_reference<B,C,L,M>, K>::type
at_c(const bit_aligned_frame_reference<B,C,L,M>& p);
#endif

/* Forward-declare semantic_at_c */
template <int K, typename ChannelBase>
typename boost::disable_if<
    boost::is_const<ChannelBase>,
    typename kth_semantic_element_reference_type<ChannelBase,K>::type>::type
semantic_at_c (ChannelBase& p);

template <int K, typename ChannelBase>
typename kth_semantic_element_const_reference_type<ChannelBase,K>::type
semantic_at_c(const ChannelBase& p);

namespace detail
{

template <typename T>
void initialize_it (T& x) {}

} /* namespace detail */

template <typename T>
struct remove_const_and_reference :
	public boost::remove_const<typename boost::remove_reference<T>::type> {};

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_FORWARDS_H_ */
