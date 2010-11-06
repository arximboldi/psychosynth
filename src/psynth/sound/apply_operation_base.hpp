/**
 *  Time-stamp:  <2010-11-05 13:54:13 raskolnikov>
 *
 *  @file        apply_operation_base.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 20:45:49 2010
 *
 *  Given an object with run-time specified type (denoted as an array
 *  of Bits, dynamic index, and a static set of Types) and a generic
 *  operation, casts the object to its appropriate type and applies
 *  the operation.
 *
 *  @todo Lets see if we can get rid of all that preprocessor crap and
 *  use variadic templates.
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

#ifndef PSYNTH_SOUND_APPLY_OPERATION_BASE_HPP
#define PSYNTH_SOUND_APPLY_OPERATION_BASE_HPP

#include <psynth/base/compat.hpp>
#include <psynth/sound/util.hpp>

#include <boost/mpl/begin.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/size.hpp>
#include <boost/preprocessor/repeat.hpp> 

namespace psynth
{
namespace sound
{

/*
  GENERATE_APPLY_FWD_OPS
  generates for every N functions that look like this (for N==2):

  template <> struct apply_operation_fwd_fn<3> {
     template <typename Types, typename Bits, typename UnaryOp>
     typename UnaryOp::result_type apply(Bits& bits,
           std::size_t index, UnaryOp op) const
     {
            typedef typename boost::mpl::begin<Types>::type T0;
            typedef typename boost::mpl::next<T0>::type T1;
            typedef typename boost::mpl::next<T1>::type T2;

	    switch (index) {
                case 0: return op(
		  reinterpret_cast<typename boost::mpl::deref<T0>::type&>(bits));
                case 1: return op(
		  reinterpret_cast<typename boost::mpl::deref<T1>::type&>(bits));
                case 2: return op(
		  reinterpret_cast<typename boost::mpl::deref<T2>::type&>(bits));
            }
            throw;
     }

     template <typename Types, typename Bits, typename UnaryOp>
     typename UnaryOp::result_type applyc (
        const Bits& bits, std::size_t index, UnaryOp op) const
     {
         typedef typename boost::mpl::begin<Types>::type T0;
	 typedef typename boost::mpl::next<T0>::type T1;
	 typedef typename boost::mpl::next<T1>::type T2;

	 switch (index)
	 {
	 case 0: return op(
	 reinterpret_cast<const typename boost::mpl::deref<T0>::type&>(bits));
	 case 1: return op(
	 reinterpret_cast<const typename boost::mpl::deref<T1>::type&>(bits));
	 case 2: return op(
	 reinterpret_cast<const typename boost::mpl::deref<T2>::type&>(bits));
     }
     throw;
     }
};
*/

#define PSYNTH_FWD_TYPEDEFS(z, N, text)			\
    T##N;						\
    typedef typename boost::mpl::next<T##N>::type

#define PSYNTH_FWD_CASE(z, N, SUM)					\
    case N: return op(							\
	*psynth_reinterpret_cast<typename boost::mpl::deref<T##N>::type*>( \
	    &bits));

#define PSYNTH_FWD_CONST_CASE(z, N, SUM)				\
    case N: return op(							\
	*psynth_reinterpret_cast_c<const typename boost::mpl::deref<T##N>::type*>( \
	    &bits));

#define PSYNTH_FWD_CASE_WITH_INFO(z, N, SUM)				\
    case N: return op(							\
	*psynth_reinterpret_cast<typename boost::mpl::deref<T##N>::type*>( \
	    &bits), info);

#define PSYNTH_FWD_CONST_CASE_WITH_INFO(z, N, SUM)			\
    case N: return op(							\
	*psynth_reinterpret_cast_c<const typename boost::mpl::deref<T##N>::type*>( \
	    &bits), info);

#define PSYNTH_APPLY_FWD_OP(z, N, text)					\
    template <> struct apply_operation_fwd_fn<BOOST_PP_ADD(N,1)> {	\
        template <typename Types, typename Bits, typename UnaryOp>	\
        typename UnaryOp::result_type apply (				\
	    Bits& bits, std::size_t index, UnaryOp op) const		\
	{								\
            typedef typename boost::mpl::begin<Types>::type		\
		BOOST_PP_REPEAT(N, PSYNTH_FWD_TYPEDEFS, BOOST_PP_EMPTY)	\
		T##N;							\
            switch (index) {						\
                BOOST_PP_REPEAT(BOOST_PP_ADD(N,1), PSYNTH_FWD_CASE,	\
				BOOST_PP_EMPTY)				\
		    }							\
            throw;							\
        }								\
        template <typename Types, typename Bits, typename UnaryOp>	\
        typename UnaryOp::result_type applyc (				\
	    const Bits& bits, std::size_t index, UnaryOp op) const	\
	{								\
            typedef typename boost::mpl::begin<Types>::type		\
		BOOST_PP_REPEAT(N, PSYNTH_FWD_TYPEDEFS, BOOST_PP_EMPTY)	\
		T##N;							\
            switch (index) {						\
                BOOST_PP_REPEAT(BOOST_PP_ADD(N,1), PSYNTH_FWD_CONST_CASE, \
				BOOST_PP_EMPTY)				\
		    }							\
            throw;							\
        }								\
        template <typename Types, typename Info,			\
		  typename Bits, typename UnaryOp>			\
        typename UnaryOp::result_type apply (				\
	    Bits& bits, std::size_t index,				\
	    const Info& info, UnaryOp op) const				\
	{								\
            typedef typename boost::mpl::begin<Types>::type		\
		BOOST_PP_REPEAT(N, PSYNTH_FWD_TYPEDEFS, BOOST_PP_EMPTY)	\
		T##N;							\
            switch (index) {						\
                BOOST_PP_REPEAT(BOOST_PP_ADD(N,1), PSYNTH_FWD_CASE_WITH_INFO, \
				BOOST_PP_EMPTY)				\
		    }							\
            throw;							\
        }								\
        template <typename Types, typename Bits, typename Info, typename UnaryOp> \
        typename UnaryOp::result_type applyc(				\
	    const Bits& bits, std::size_t index,			\
	    const Info& info, UnaryOp op) const {			\
            typedef typename boost::mpl::begin<Types>::type		\
		BOOST_PP_REPEAT(N, PSYNTH_FWD_TYPEDEFS, BOOST_PP_EMPTY)	\
		T##N;							\
            switch (index) {						\
                BOOST_PP_REPEAT(BOOST_PP_ADD(N,1),			\
				PSYNTH_FWD_CONST_CASE_WITH_INFO,BOOST_PP_EMPTY) \
		    }							\
            throw;							\
        }								\
    };

#define PSYNTH_GENERATE_APPLY_FWD_OPS(N)			\
    BOOST_PP_REPEAT(N, PSYNTH_APPLY_FWD_OP, BOOST_PP_EMPTY)

namespace detail
{

template <std::size_t N>
struct apply_operation_fwd_fn {};

// Create specializations of apply_operation_fn for each N 0..100
PSYNTH_GENERATE_APPLY_FWD_OPS(10)

} /* namespace detail */

// unary application
template <typename Types, typename Bits, typename Op> 
typename Op::result_type PSYNTH_FORCEINLINE
apply_operation_basec (const Bits& bits, std::size_t index, Op op)
{
    return detail::apply_operation_fwd_fn<
	boost::mpl::size<Types>::value>().template applyc<Types> (
	    bits, index, op);
}

// unary application
template <typename Types, typename Bits, typename Op> 
typename Op::result_type PSYNTH_FORCEINLINE
apply_operation_base (Bits& bits, std::size_t index, Op op)
{
    return detail::apply_operation_fwd_fn<
	boost::mpl::size<Types>::value>().template apply<Types> (
	    bits,index,op);
}

namespace detail
{

template <typename T2, typename Op>
struct reduce_bind1
{
    const T2& _t2;
    Op&       _op;
    
    typedef typename Op::result_type result_type;
    
    reduce_bind1(const T2& t2, Op& op)
	: _t2(t2), _op(op) {}
    
    template <typename T1> PSYNTH_FORCEINLINE
    result_type operator () (const T1& t1)
    { return _op(t1, _t2); }
};

template <typename Types1, typename Bits1, typename Op>
struct reduce_bind2
{
    const Bits1& _bits1;
    std::size_t  _index1;
    Op&          _op;

    typedef typename Op::result_type result_type;

    reduce_bind2 (const Bits1& bits1, std::size_t index1, Op& op)
	: _bits1 (bits1)
	, _index1 (index1)
	, _op (op)
    {}

    template <typename T2> PSYNTH_FORCEINLINE
    result_type operator () (const T2& t2)
    { 
	return apply_operation_basec<Types1> (
	    _bits1, _index1, reduce_bind1<T2,Op> (t2, _op));
    }
};

} /* namespace detail */

// Binary application by applying on each dimension separately
template <typename Types1, typename Types2,
	  typename Bits1, typename Bits2, typename Op>
static PSYNTH_FORCEINLINE
typename Op::result_type apply_operation_base (
    const Bits1& bits1, std::size_t index1,
    const Bits2& bits2, std::size_t index2, Op op)
{
    return apply_operation_basec<Types2> (
	bits2, index2, detail::reduce_bind2<Types1,Bits1,Op>(
	    bits1, index1, op));
}

#undef PSYNTH_FWD_TYPEDEFS
#undef PSYNTH_FWD_CASE
#undef PSYNTH_FWD_CONST_CASE
#undef PSYNTH_APPLY_FWD_OP
#undef PSYNTH_GENERATE_APPLY_FWD_OPS

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_APPLY_OPERATION_BASE */
