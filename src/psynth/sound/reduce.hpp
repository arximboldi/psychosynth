/**
 *  Time-stamp:  <2011-04-04 17:42:13 raskolnikov>
 *
 *  @file        reduce.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 11:38:11 2010
 *
 *  Constructs for static-to-dynamic integer convesion.
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

#ifndef PSYNTH_SOUND_REDUCE_HPP
#define PSYNTH_SOUND_REDUCE_HPP

#include <boost/mpl/insert_range.hpp>
#include <boost/mpl/range_c.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/back.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/set.hpp>
#include <boost/mpl/long.hpp>
#include <boost/mpl/logical.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/insert.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/order.hpp>

#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/sound/dynamic_at_c.hpp>


#ifdef PSYNTH_REDUCE_CODE_BLOAT


// Max number of cases in the cross-expension of binary operation for
// it to be reduced as unary
#define PSYNTH_BINARY_REDUCE_LIMIT 226

namespace boost
{
namespace mpl
{

namespace detail {

template <typename VecOfVecs, std::size_t N, std::size_t B_SIZE>
struct _select_subvector {
private:
    typedef typename front<VecOfVecs>::type cur_vec;
    static const std::size_t cur_base = size<cur_vec>::value;
    typedef typename pop_front<VecOfVecs>::type  next_vec_of_vecs;
    typedef typename _select_subvector<next_vec_of_vecs,  N/cur_base, B_SIZE-1>::type
    next_subvector;
public:
    typedef typename push_front<
    next_subvector,
    typename at_c<cur_vec, N%cur_base>::type >::type type;
};

template <typename VecOfVecs, std::size_t N>
struct _select_subvector<VecOfVecs, N, 0> {
    typedef vector<>::type type;
};

template <typename VecOfVecs, std::size_t N>
struct select_subvector_c {
    typedef typename _select_subvector<
        VecOfVecs, N, size<VecOfVecs>::value>::type type;
};

} /* namespace detail */

/**
 * \brief Represents the virtual cross-product of the types generated
 * from VecOfVecs.
 *
 * \ingroup CrossVector
 *
 * INPUT:
 *  VecOfVecs - a vector of vector types. For example [ [A1,A2,A3],
 *  [B1,B2], [C1,C2,C3,C4] ]
 *
 * Each element must be a non-empty mpl vector TypeGen - a
 * metafunction that generates a type from a vector of types, each of
 * which can be selected from the corresponding vector in
 * VecOfVecs. For example, [A1, B2, C4]
 *
 * Represents the virtual cross-product of the types generated from
 * VecOfVecs.
 *
 * For example, [ TypeGen[A1,B1,C1], TypeGen[A2,B1,C1], TypeGen[A3,B1,C1],
 *                TypeGen[A1,B2,C1], TypeGen[A2,B2,C1], TypeGen[A3,B2,C1],
 *                TypeGen[A1,B1,C2], TypeGen[A2,B1,C2],
 *                TypeGen[A3,B1,C2], ... ]
 *
 * Models an immutable MPL Random Access Sequence
 * Traversal, random-access, etc, is defined, but mutable operations,
 * such as push_back and pop_front are not supported
 *
 */

template <typename VecOfVecs, typename TypeGen>
struct cross_vector {};

/**
 * \brief Iterator of cross_vector
 * \ingroup CrossVectorIterator
 */
template <typename VecOfVecs, typename TypeGen, std::size_t K>
struct cross_iterator
{
    typedef boost::mpl::random_access_iterator_tag category;
};

/*
 *
 *      Implementation of the iterator functions of cross vector
 *
 */

/**
 * \brief Dereferences a cross-vector iterator
 * \ingroup CrossVectorIterator
 *
 * Creates a vector of the sizes of each type vector in VecOfVecs,
 * then uses it as a basis to represent the iterator's position K as a
 * vector of indices. Extracts the corresponding type of each input
 * vector and passes the element types to the type generation
 * function, which returns the dereferenced type
 */
template <typename VecOfVecs, typename TypeGen, std::size_t K>
struct deref<cross_iterator<VecOfVecs,TypeGen,K> >
{
private:
    typedef typename detail::select_subvector_c<VecOfVecs, K>::type DerefTypes;
public:
    typedef typename TypeGen::template apply<DerefTypes>::type type;
};

/**
 * \brief Increments a cross-vector iterator.
 * \ingroup CrossVectorIterator
 */
template <typename VecOfVecs, typename TypeGen, std::size_t K>
struct next<cross_iterator<VecOfVecs,TypeGen,K> > {
    typedef cross_iterator<VecOfVecs,TypeGen,K+1> type;
};

/**
 * \brief Decrements a cross-vector iterator.
 * \ingroup CrossVectorIterator
 */
template <typename VecOfVecs, typename TypeGen, std::size_t K>
struct prior<cross_iterator<VecOfVecs,TypeGen,K> > {
    typedef cross_iterator<VecOfVecs,TypeGen,K-1> type;
};

/**
 * \brief Advances a cross-vector iterator.
 * \ingroup CrossVectorIterator
 */
template <typename VecOfVecs, typename TypeGen, std::size_t K, typename Distance>
struct advance<cross_iterator<VecOfVecs,TypeGen,K>, Distance > {
    typedef cross_iterator<VecOfVecs,TypeGen,K+Distance::value> type;
};

/**
 * \brief Computes the distance between two cross-vector iterator-s.
 * \ingroup CrossVectorIterator
 * (shortened the names of the template arguments - otherwise doxygen
 * cannot parse this...)
 */
template <typename VecOfVecs, typename TypeGen,
	  std::size_t K1, std::size_t K2>
struct distance<cross_iterator<VecOfVecs,TypeGen,K1>,
		cross_iterator<VecOfVecs,TypeGen,K2> >
{
    typedef size_t<K2-K1> type;
};


/*
 *
 *      Implementation of cross vector
 *
 */

/**
 * \brief Computes the size of a cross vector as the product of the
 * sizes of all vectors in VecOfVecs
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen>
struct size<cross_vector<VecOfVecs,TypeGen> >
{
    typedef typename fold<VecOfVecs, size_t<1>,
			  times<_1, size<_2> > >::type type;
    static const std::size_t value = type::value;
};

/**
 * \brief Determines whether a cross vector is empty
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen>
struct empty<cross_vector<VecOfVecs,TypeGen> > {
    typedef typename empty<VecOfVecs>::type type;
};

/**
 * \brief Returns the K-th element of a cross vector
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen, typename K>
struct at<cross_vector<VecOfVecs,TypeGen>, K>
{
private:
    typedef cross_iterator<VecOfVecs,TypeGen,K::value> KthIterator;
public:
    typedef typename deref<KthIterator>::type type;
};

/**
 * \brief Returns an iterator to the first element of a cross vector
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen>
struct begin<cross_vector<VecOfVecs,TypeGen> > {
    typedef cross_iterator<VecOfVecs,TypeGen,0> type;
};

/**
 * \brief Returns an iterator to the last element of a cross vector
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen>
struct end<cross_vector<VecOfVecs,TypeGen> >
{
private:
    typedef cross_vector<VecOfVecs,TypeGen> this_t;
public:
    typedef cross_iterator<VecOfVecs,TypeGen,size<this_t>::value> type;
};

/**
 * \brief Returns the first element of a cross vector
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen>
struct front<cross_vector<VecOfVecs,TypeGen> >
{
private:
    typedef cross_vector<VecOfVecs,TypeGen> this_t;
public:
    typedef typename deref<typename begin<this_t>::type>::type type;
};

/**
 * \brief Returns the last element of a cross vector
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen>
struct back<cross_vector<VecOfVecs,TypeGen> > {
private:
    typedef cross_vector<VecOfVecs,TypeGen> this_t;
    typedef typename size<this_t>::type            size;
    typedef typename minus<size, size_t<1> >::type last_index;
public:
    typedef typename at<this_t, last_index>::type type;
};

/**
 * \brief Transforms the elements of a cross vector
 * \ingroup CrossVector
 */
template <typename VecOfVecs, typename TypeGen, typename OPP>
struct transform<cross_vector<VecOfVecs,TypeGen>, OPP > {
    typedef typename lambda<OPP>::type Op;
    struct adapter {
        template <typename Elements>
        struct apply {
            typedef typename TypeGen::template apply<Elements>::type orig_t;
            typedef typename Op::template apply<orig_t>::type type;
        };
    };
    typedef cross_vector<VecOfVecs, adapter > type;
};

} /* namespace mpl */
} /* namespace boost */

namespace boost
{
namespace mpl
{

/**
 * Mapping vector - represents the mapping of one type vector to
 * another It is not a full-blown MPL Random Access Type sequence;
 * just has at_c and size implemented
 *
 * SrcTypes, DstTypes: MPL Random Access Type Sequences
 *
 * Implements size and at_c to behave as if this is an MPL vector of integers
 */

template <typename SrcTypes, typename DstTypes>
struct mapping_vector {};

template <typename SrcTypes, typename DstTypes, long K>
struct at_c<mapping_vector<SrcTypes,DstTypes>, K>
{
    static const std::size_t value =
	size<DstTypes>::value -
	mpl::order<DstTypes, typename at_c<SrcTypes,K>::type>::type::value + 1;
    typedef size_t<value> type;
};

template <typename SrcTypes, typename DstTypes>
struct size<mapping_vector<SrcTypes,DstTypes> >
{
    typedef typename size<SrcTypes>::type type;
    static const std::size_t value = type::value;
};


/**
 * copy_to_vector - copies a sequence (boost::mpl::set) to vector.
 *
 * Temporary solution because I couldn't get boost::mpl::copy to do
 * this.  This is what I tried: boost::mpl::copy<SET,
 * boost::mpl::back_inserter<boost::mpl::vector<> > >::type; It works
 * when SET is boost::mpl::vector, but not when SET is
 * boost::mpl::set...
 */

namespace detail
{

template <typename SFirst, std::size_t NLeft>
struct copy_to_vector_impl
{
private:
    typedef typename deref<SFirst>::type T;
    typedef typename next<SFirst>::type next;
    typedef typename copy_to_vector_impl<next, NLeft-1>::type rest;
public:
    typedef typename push_front<rest, T>::type type;
};

template <typename SFirst>
struct copy_to_vector_impl<SFirst,1>
{
    typedef vector<typename deref<SFirst>::type> type;
};

} /* namespace detail */

template <typename Src>
struct copy_to_vector
{
    typedef typename detail::copy_to_vector_impl<
	typename begin<Src>::type, size<Src>::value>::type type;
};

template <>
struct copy_to_vector<mpl::set<> >
{
    typedef mpl::vector0<> type;
};

} /* namespace mpl */
} /* namespace boost */


namespace psynth
{
namespace sound
{

/**
 * unary_reduce, binary_reduce - given an MPL Random Access Sequence,
 * dynamically specified index to that container, the bits of an
 * instance of the corresponding type and a generic operation, invokes
 * the operation on the given type
 */

/**
 * \brief Unary reduce.
 *
 * Given a set of types and an operation, reduces each type in the set
 * (to reduced_t), then removes duplicates (to unique_t) To apply the
 * operation, first constructs a lookup table that maps each element
 * from Types to its place in unique_t and uses it to map the index to
 * anther index (in map_index). Then invokes apply_operation_base on
 * the unique types with the new index.
 */

template <typename Types, typename Op>
struct unary_reduce_impl
{
    typedef typename boost::mpl::transform<
	Types, detail::reduce<Op, boost::mpl::_1> >::type reduced_t;
    typedef typename boost::mpl::copy<
	reduced_t, boost::mpl::inserter<
		       boost::mpl::set<>,
		       boost::mpl::insert<boost::mpl::_1,
					  boost::mpl::_2> > >::type unique_t;
    static const bool is_single = boost::mpl::size<unique_t>::value == 1;
};

template <typename Types, typename Op,
	  bool IsSingle = unary_reduce_impl<Types,Op>::is_single>
struct unary_reduce : public unary_reduce_impl<Types,Op>
{
    typedef typename unary_reduce_impl<Types,Op>::reduced_t reduced_t;
    typedef typename unary_reduce_impl<Types,Op>::unique_t unique_t;

    static unsigned short inline map_index (std::size_t index)
    {
        typedef typename boost::mpl::mapping_vector<reduced_t, unique_t> indices_t;
        return gil::at_c<indices_t, unsigned short>(index);
    }

    template <typename Bits> PSYNTH_FORCEINLINE
    static typename Op::result_type applyc (
	const Bits& bits, std::size_t index, Op op)
    {
        return apply_operation_basec<unique_t>(bits, map_index (index), op);
    }

    template <typename Bits> PSYNTH_FORCEINLINE
    static typename Op::result_type apply (Bits& bits, std::size_t index, Op op)
    {
        return apply_operation_base<unique_t>(bits, map_index(index), op);
    }
};

template <typename Types, typename Op>
struct unary_reduce<Types,Op,true> : public unary_reduce_impl<Types,Op>
{
    typedef typename unary_reduce_impl<Types,Op>::unique_t unique_t;

    static unsigned short inline map_index (std::size_t index)
    {
	return 0;
    }

    template <typename Bits> PSYNTH_FORCEINLINE
    static typename Op::result_type applyc (
	const Bits& bits, std::size_t index, Op op)
    {
        return op (*psynth_reinterpret_cast_c<
		       const typename boost::mpl::front<unique_t>::type*>(&bits));
    }

    template <typename Bits> PSYNTH_FORCEINLINE
    static typename Op::result_type apply (Bits& bits, std::size_t index, Op op)
    {
        return op (*psynth_reinterpret_cast<
		       typename boost::mpl::front<unique_t>::type*>(&bits));
    }
};


/**
 * \brief Binary reduce.
 *
 * Given two sets of types, Types1 and Types2, first performs unary
 * reduction on each. Then checks if the product of their sizes is
 * above the PSYNTH_BINARY_REDUCE_LIMIT limit. If so, the operation is
 * too complex to be binary-reduced and uses a specialization of
 * binary_reduce_impl to simply call the binary apply_operation_base
 * (which performs two nested 1D apply operations) If the operation is
 * not too complex, uses the other specialization of
 * binary_reduce_impl to create a cross-product of the input types and
 * performs unary reduction on the result (bin_reduced_t). To apply
 * the binary operation, it simply invokes a unary
 * apply_operation_base on the reduced cross-product types.
 */

namespace detail
{

struct pair_generator
{
    template <typename Vec2>
    struct apply
    {
	typedef std::pair<
	    const typename boost::mpl::at_c<Vec2,0>::type*,
	    const typename boost::mpl::at_c<Vec2,1>::type*> type;
};
};

// When the types are not too large, applies reduce on their cross
// product
template <typename Unary1, typename Unary2, typename Op,
	  bool IsComplex>
struct binary_reduce_impl
{
    // TODO: private:
    typedef typename boost::mpl::copy_to_vector<
	typename Unary1::unique_t>::type vec1_types;
    typedef typename boost::mpl::copy_to_vector<
	typename Unary2::unique_t>::type vec2_types;

    typedef boost::mpl::cross_vector<
	boost::mpl::vector2<vec1_types, vec2_types>, pair_generator> BIN_TYPES;

    typedef unary_reduce<BIN_TYPES,Op> bin_reduced_t;

    static unsigned short inline map_index (std::size_t index1,
					    std::size_t index2)
    {
	unsigned short r1=Unary1::map_index(index1);
	unsigned short r2=Unary2::map_index(index2);
	return bin_reduced_t::map_index (
	    r2 * boost::mpl::size<vec1_types>::value + r1);
    }

public:
    typedef typename bin_reduced_t::unique_t unique_t;

    template <typename Bits1, typename Bits2>
    static typename Op::result_type inline apply (
	const Bits1& bits1, std::size_t index1,
	const Bits2& bits2, std::size_t index2, Op op)
    {
	std::pair<const void*,const void*> pr (&bits1, &bits2);
	return apply_operation_basec<unique_t> (
	    pr, map_index (index1, index2), op);
    }
};

// When the types are large performs a double-dispatch. Binary
// reduction is not done.
template <typename Unary1, typename Unary2, typename Op>
struct binary_reduce_impl<Unary1, Unary2,Op, true>
{
    template <typename Bits1, typename Bits2>
    static typename Op::result_type inline apply (
	const Bits1& bits1, std::size_t index1,
	const Bits2& bits2, std::size_t index2, Op op)
    {
	return apply_operation_base<Unary1::unique_t, Unary2::unique_t>(
	    bits1, index1, bits2, index2, op);
    }
};

} /* namespace detail */


template <typename Types1, typename Types2, typename Op>
struct binary_reduce
{
// TODO private:
    typedef unary_reduce<Types1,Op> unary1_t;
    typedef unary_reduce<Types2,Op> unary2_t;

    static const std::size_t CROSS_SIZE =
	boost::mpl::size<typename unary1_t::unique_t>::value *
	boost::mpl::size<typename unary2_t::unique_t>::value;

    typedef detail::binary_reduce_impl<
	unary1_t, unary2_t, Op, (CROSS_SIZE > PSYNTH_BINARY_REDUCE_LIMIT)> impl;

public:
    template <typename Bits1, typename Bits2>
    static typename Op::result_type inline apply (
	const Bits1& bits1, std::size_t index1,
	const Bits2& bits2, std::size_t index2, Op op)
    {
        return impl::apply (bits1, index1, bits2, index2, op);
    }
};

template <typename Types, typename UnaryOp> PSYNTH_FORCEINLINE
typename UnaryOp::result_type apply_operation (variant<Types>& arg, UnaryOp op)
{
    return unary_reduce<Types,UnaryOp>::template apply (
	arg._bits, arg._index, op);
}

template <typename Types, typename UnaryOp> PSYNTH_FORCEINLINE
typename UnaryOp::result_type apply_operation (const variant<Types>& arg,
					       UnaryOp op)
{
    return unary_reduce<Types,UnaryOp>::template applyc (
	arg._bits, arg._index, op);
}

template <typename Types1, typename Types2, typename BinaryOp> PSYNTH_FORCEINLINE
typename BinaryOp::result_type apply_operation (const variant<Types1>& arg1,
						const variant<Types2>& arg2,
						BinaryOp op)
{
    return binary_reduce<Types1, Types2, BinaryOp>::template apply (
	arg1._bits, arg1._index,
	arg2._bits, arg2._index, op);
}

#undef PSYNTH_BINARY_REDUCE_LIMIT

} /* namespace sound */
} /* namespace psynth */


namespace psynth
{
namespace sound
{

template <typename Types, typename T> struct type_to_index;
template <typename V> struct range_is_basic;

#if 0 // HACK?

struct stereo_space;
struct mono_space;
struct quad_space;
struct surround_space;
struct error_type;

#endif

namespace detail
{

/*
 *
 *       Generic reduce operation
 *
 */
template <typename Op, typename T>
struct reduce
{
    typedef T type;
};

/**
 *
 * Unary reduce_range operation. Splits into basic and non-basic ranges.
 * Algorithm-specific reduce should specialize for basic ranges
 *
 */
template <typename Op, typename Range, bool IsBasic>
struct reduce_range_basic
{
    typedef Range type;
};

template <typename Op, typename Loc>
struct reduce<Op, buffer_range<Loc> >
    : public reduce_range_basic<Op, buffer_range<Loc>,
			       range_is_basic<buffer_range<Loc> >::value> {};

/**
 *
 * Unary reduce_buffer operation. Splits into basic and non-basic buffers.
 * Algorithm-specific reduce should specialize for basic buffers
 *
 */

template <typename Op, typename Buf, bool IsBasic>
struct reduce_buffer_basic
{
    typedef Buf type;
};

template <typename Op, typename F, bool P, typename A>
struct reduce<Op, buffer<F, P, A>> :
    public reduce_buffer_basic<Op,
                               buffer<F, P, A>,
                               buffer_is_basic<buffer<F, P, A>>::value > {};

/**
 *
 * Binary reduce_range operation. Splits into basic and non-basic ranges.
 * Algorithm-specific reduce should specialize for basic ranges
 *
 */

template <typename Op, typename V1, typename V2, bool AreBasic>
struct reduce_ranges_basic
{
    typedef std::pair<const V1*, const V2*> type;
};

template <typename Op, typename L1, typename L2>
struct reduce<Op, std::pair<const buffer_range<L1>*, const buffer_range<L2>*> >
    : public reduce_ranges_basic<Op,
				buffer_range<L1>,
				buffer_range<L2>,
				boost::mpl::and_<
				    range_is_basic<buffer_range<L1> >,
				    range_is_basic<buffer_range<L2> > >::value >
    {};


/**
 *
 * Channel space unary reduce operation. Reduce a channel space to a base
 * with the same number of samples
 *
 */

template <typename Cs>
struct reduce_channel_space {
    typedef Cs type;
};

/*
  TODO:
  template <> struct reduce_channel_space<lab_t> { typedef rgb_t type; };
  template <> struct reduce_channel_space<hsb_t> { typedef rgb_t type; };
  template <> struct reduce_channel_space<cmyk_t> { typedef rgba_t type; };
*/

/**

   Channel space binary reduce operation. Given a source and
   destination channel spaces, returns a reduced source and
   destination channel spaces that have the same mapping of samples

   Precondition: The two channel spaces must be compatible (i.e. must
   have the same set of samples)

    template <typename Vec, int Basis, int VecSize>
    struct type_vec_to_integer_impl {
        typedef typename boost::mpl::back<Vec>::type     last;
        typedef typename boost::mpl::pop_back<Vec>::type rest;
        static const int value =
	    type_vec_to_integer_impl<rest, Basis, VecSize-1>::value *
	    Basis + last::value;
    };

    template <typename Vec, int Basis>
    struct type_vec_to_integer_impl<Vec,Basis,0> {
        static const int value=0;
    };

    template <typename Vec, int Basis=10>
    struct type_vec_to_integer {
        static const int value = type_vec_to_integer_impl<
	        Vec,Basis, boost::mpl::size<Vec>::value>::value;
    };

    // Given two channel spaces and the mapping of the samples between
    // them, returns the reduced pair of channel spaces The default
    // version performs no reduction

    template <typename SrcChannelSpace, typename DstChannelSpace, int Mapping>
    struct reduce_channel_spaces_impl {
        typedef SrcChannelSpace first_t;
        typedef DstChannelSpace second_t;
    };

    // 012: RGB-RGB, bgr-bgr, lab-lab, hsb-hsb
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,12> {
        typedef rgb_t first_t;
        typedef rgb_t second_t;
    };

    // 210: RGB-bgr, bgr-RGB
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,210> {
        typedef rgb_t first_t;
        typedef bgr_t second_t;
    };

    // 0123: RGBA-RGBA, bgra-bgra, argb-argb, abgr-abgr cmyk-cmyk
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,123> {
        typedef rgba_t first_t;
        typedef rgba_t second_t;
    };

    // 3210: RGBA-abgr, bgra-argb, argb-bgra, abgr-RGBA
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,3210> {
        typedef rgba_t first_t;
        typedef abgr_t second_t;
    };

    // 1230: RGBA-argb, bgra-abgr
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,1230> {
        typedef rgba_t first_t;
        typedef argb_t second_t;
    };

    // 2103: RGBA-bgra, bgra-RGBA (uses subclass to ensure that base
    // channel space is not reduced to derived)
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,2103> {
        typedef rgba_t first_t;
        typedef bgra_t second_t;
    };

    // 3012: argb-RGBA, abgr-bgra
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,3012> {
        typedef argb_t first_t;
        typedef rgba_t second_t;
    };

    // 0321: argb-abgr, abgr-argb
    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces_impl<SrcChannelSpace,DstChannelSpace,321> {
        typedef argb_t first_t;
        typedef abgr_t second_t;
    };

    template <typename SrcChannelSpace, typename DstChannelSpace>
    struct reduce_channel_spaces
    {
        typedef typename sample_order<SrcChannelSpace>::type src_order_t;
        typedef typename sample_order<DstChannelSpace>::type dst_order_t;
        typedef typename boost::mpl::transform<
	    src_order_t, type_to_index<
	         dst_order_t,boost::mpl::_1> >::type mapping;
        static const int mapping_val = type_vec_to_integer<mapping>::value;

        typedef typename reduce_channel_spaces_impl<
	     SrcChannelSpace,DstChannelSpace,mapping_val>::first_t  _first_t;
        typedef typename reduce_channel_spaces_impl<
	     SrcChannelSpace,DstChannelSpace,mapping_val>::second_t _second_t;

	typedef typename boost::mpl::and_<
	     channel_space_is_base<DstChannelSpace>,
	     boost::mpl::not_< channel_space_is_base<_second_t> > > swap_t;

    public:
        typedef typename boost::mpl::if_<
	           swap_t, _second_t, _first_t>::type first_t;
        typedef typename boost::mpl::if_<
	           swap_t, _first_t, _second_t>::type second_t;
    };

*/

// TODO: Use the old code for reduce_color_spaces above to do
// color layout reduction

template <typename SrcLayout, typename DstLayout>
struct reduce_channel_layouts
{
    typedef SrcLayout first_t;
    typedef DstLayout second_t;
};


/*
 *
 *       Reduce for copy_frames
 *
 */

struct copy_frames_fn;

/*

// 1D reduce for copy_frames reduces the sample to mutable and the
// channel space to its base with same dimensions

    template <typename Range>
    struct reduce_range_basic<copy_frames_fn,Range,true> {
    private:
        typedef typename reduce_channel_space<
	    typename Range::channel_space_t>::type Cs;
	    // reduce the channel space
        typedef layout<Cs, typename Range::sample_mapping_t> layout_t;
    public:
        typedef typename derived_range_type<
	        Range, use_default, layout_t, use_default,
		use_default, boost::mpl::true_>::type type;
    };
*/

// Incompatible ranges cannot be used in copy_frames - will throw
// std::bad_cast
template <typename V1, typename V2, bool Compatible>
struct reduce_copy_pixop_compat
{
    typedef error_type type;
};

// For compatible basic ranges, reduce their channel spaces based on
// their sample mapping.  Make the source immutable and the
// destination mutable (they should already be that way)
template <typename V1, typename V2>
struct reduce_copy_pixop_compat<V1, V2, true>
{
    typedef layout<typename V1::channel_space,
		   typename V1::sample_mapping> layout1;
    typedef layout<typename V2::channel_space,
		   typename V2::sample_mapping> layout2;

     typedef typename reduce_channel_layouts<layout1,layout2>::first_t L1;
     typedef typename reduce_channel_layouts<layout1,layout2>::second_t L2;

     typedef typename derived_range_type<
	 V1, boost::use_default, L1,
         boost::use_default,
	 boost::use_default, boost::mpl::false_>::type DV1;

     typedef typename derived_range_type<
	 V2, boost::use_default, L2,
         boost::use_default,
	 boost::use_default, boost::mpl::true_ >::type DV2;

     typedef std::pair<const DV1*, const DV2*> type;
};

// The general 2D version branches into compatible and incompatible
// ranges
template <typename V1, typename V2>
struct reduce_ranges_basic<copy_frames_fn, V1, V2, true>
    : public reduce_copy_pixop_compat<
    V1, V2, boost::mpl::and_<ranges_are_compatible<V1,V2>,
			     range_is_mutable<V2> >::value >
{
};


/**
 *
 *       Reduce for variant destructor (basic ranges have no destructor)
 *
 */

struct destructor_op;
template <typename Range>
struct reduce_range_basic<destructor_op,Range,true>
{
    typedef mono8_range type;
};


/**
 *
 *      Reduce for get_dimensions (basic ranges and buffers have the
 *      same structure and the dimensions are contained at the
 *      beginning)
 *
 */

struct dynamic_type_get_dimensions;
template <typename Range>
struct reduce_range_basic<dynamic_type_get_dimensions, Range, true>
{
    typedef mono8_range type;
};

template <typename Img>
struct reduce_buffer_basic<dynamic_type_get_dimensions, Img, true>
{
    typedef mono8_buffer type;
};

/*
 *
 *      Reduce for get_num_samples (only channel space matters)
 *
 */

struct dynamic_type_get_num_samples;
template <typename Range>
struct reduce_range_basic<dynamic_type_get_num_samples,Range,true>
{
    typedef typename Range::channel_space::base Cs;

    typedef typename range_type<
	bits8, typename reduce_channel_space<Cs>::type>::type type;
};

template <typename Img>
struct reduce_buffer_basic<dynamic_type_get_num_samples, Img, true>
{
    typedef typename Img::channel_space_t::base Cs;
    typedef typename buffer_type<
	bits8, typename reduce_channel_space<Cs>::type>::type type;
};

/*
 *
 *      Reduce for resample_frames (same as copy_frames)
 *
 */
template <typename Sampler, typename MapFn> struct resample_frames_fn;

template <typename S, typename M, typename V, bool IsBasic>
struct reduce_range_basic<resample_frames_fn<S,M>, V, IsBasic> :
    public reduce_range_basic<copy_frames_fn, V, IsBasic> {};

template <typename S, typename M, typename V1, typename V2, bool IsBasic>
struct reduce_ranges_basic<resample_frames_fn<S,M>, V1, V2, IsBasic>
    : public reduce_ranges_basic<copy_frames_fn, V1, V2, IsBasic> {};

/*
 *
 *      Reduce for copy_and_convert_frames
 *
 *      (the only reduction could be made when ranges are compatible
 *      and have the same mapping, planarity and stepness)
 *
 */

template <typename CC> class copy_and_convert_frames_fn;

// the only thing for 1D reduce is making them all mutable...
template <typename CC, typename Range, bool IsBasic>
struct reduce_range_basic<copy_and_convert_frames_fn<CC>, Range, IsBasic>
    : public derived_range_type<Range, boost::use_default, boost::use_default,
			       boost::use_default, boost::use_default, boost::mpl::true_>
{
};

// For 2D reduce, if they have the same samples and channel spaces
// (i.e. the same frames) then copy_and_convert is just copy.  In this
// case, reduce their common channel space. In general make the first
// immutable and the second mutable
template <typename CC, typename V1, typename V2, bool AreBasic>
struct reduce_ranges_basic<copy_and_convert_frames_fn<CC>, V1, V2, AreBasic>
{
    typedef boost::is_same<typename V1::frame_type, typename V2::frame_type> Same;

    typedef reduce_channel_space<typename V1::channel_space::base> CsR;
    typedef typename boost::mpl::if_<
	Same, typename CsR::type, typename V1::channel_space>::type Cs1;
    typedef typename boost::mpl::if_<
	Same, typename CsR::type, typename V2::channel_space>::type Cs2;

    typedef typename derived_range_type<
	V1, boost::use_default, layout<Cs1, typename V1::sample_mapping>,
	boost::use_default, boost::use_default, boost::mpl::false_>::type DV1;

    typedef typename derived_range_type<
	V2, boost::use_default, layout<Cs2, typename V2::sample_mapping_t>,
	boost::use_default, boost::use_default, boost::mpl::true_ >::type DV2;

    typedef std::pair<const DV1*, const DV2*> type;
};

/*
  TODO: What the hell is this?
*/

//integral_buffer_generator
//resize_clobber_buffer_fnobj
//buffer_default_construct_fnobj
//fill_converted_frames_fn
//bind(gil::detail::copy_frames_fn(), _1, dst)
//bind(gil::detail::copy_frames_fn(), src,_1)

//bind(detail::copy_and_convert_frames_fn(), _1, dst)
//bind(detail::copy_and_convert_frames_fn(), src, _1)
//gil::detail::fill_frames_fn<Value>(val)

//detail::copy_construct_in_place_fn<base_t>
//detail::equal_to_fn<typename variant<Types>::base_t>

//detail::dynamic_buffer_get_range<typename dynamic_buffer<Types>::range_t>
//detail::dynamic_buffer_get_const_range<typename dynamic_buffer<Types>::range_t>
//detail::flipped_up_down_range_fn<dynamic_buffer_range<RangeTypes> >
//detail::flipped_left_right_range_fn<typename dynamic_buffer_range<RangeTypes>::dynamic_step_t>
//detail::tranposed_range_fn<typename dynamic_buffer_range<RangeTypes>::dynamic_step_t>
//detail::rotated90cw_range_fn<typename dynamic_buffer_range<RangeTypes>::dynamic_step_t>
//detail::rotated90ccw_range_fn<typename dynamic_buffer_range<RangeTypes>::dynamic_step_t>
//detail::rotated180_range_fn<typename dynamic_buffer_range<RangeTypes>::dynamic_step_t>
//detail::subbuffer_range_fn<dynamic_buffer_range<RangeTypes> >
//detail::sub_sampled_range_fn<typename dynamic_buffer_range<RangeTypes>::dynamic_step_t>
//detail::nth_sample_range_fn<typename nth_sample_range_type<dynamic_buffer_range<RangeTypes> >
//detail::channel_converted_range_fn<DstP,typename channel_convert_range_type<dynamic_buffer_range<RangeTypes>, DstP>::type >

} /* namespace detail */

} /* namespace sound */
} /* namespace psynth */

#endif // PSYNTH_REDUCE_CODE_BLOAT

#endif /* PSYNTH_SOUND_REDUCE */
