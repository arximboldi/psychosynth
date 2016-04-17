/**
 *  Time-stamp:  <2010-11-04 12:48:28 raskolnikov>
 *
 *  @file        dynamic_at_c.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 12:37:28 2010
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

#ifndef PSYNTH_SOUND_DYNAMIC_AT_C_HPP
#define PSYNTH_SOUND_DYNAMIC_AT_C_HPP

#include <psynth/base/compat.hpp>

#include <cassert>
#include <stdexcept>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>

namespace psynth
{
namespace base
{

#define PSYNTH_AT_C_VALUE(z, N, text)			\
    boost::mpl::at_c<IntTypes, S+N>::type::value,

#define PSYNTH_DYNAMIC_AT_C_LIMIT        226
// size of the maximum vector to handle

#define PSYNTH_AT_C_LOOKUP(z, NUM, text)				\
    template<std::size_t S>                                             \
    struct at_c_fn<S,NUM>						\
    {									\
	template <typename IntTypes, typename ValueType> inline		\
	    static ValueType apply(std::size_t index)			\
	{								\
            static ValueType table[] = {				\
                BOOST_PP_REPEAT(NUM, PSYNTH_AT_C_VALUE, BOOST_PP_EMPTY)	\
            };                                                          \
            return table[index];                                        \
        }                                                               \
    };

namespace detail
{
namespace at_c
{

template <std::size_t START, std::size_t NUM> struct at_c_fn;
BOOST_PP_REPEAT(PSYNTH_DYNAMIC_AT_C_LIMIT, PSYNTH_AT_C_LOOKUP, BOOST_PP_EMPTY)

template <std::size_t QUOT> struct at_c_impl;

template <>
struct at_c_impl<0>
{
    template <typename IntTypes, typename ValueType> inline
    static ValueType apply(std::size_t index)
    {
	return at_c_fn<0, boost::mpl::size<IntTypes>::value>::template apply<
	    IntTypes, ValueType>(index);
    }
};

template <>
struct at_c_impl<1>
{
    template <typename IntTypes, typename ValueType> inline
    static ValueType apply (std::size_t index)
    {
	const std::size_t SIZE = boost::mpl::size<IntTypes>::value;
	const std::size_t REM  = SIZE % PSYNTH_DYNAMIC_AT_C_LIMIT;

	switch (index / PSYNTH_DYNAMIC_AT_C_LIMIT)
	{
	case 0: return at_c_fn<0, PSYNTH_DYNAMIC_AT_C_LIMIT-1>::template apply<
	    IntTypes, ValueType> (index);
	case 1: return at_c_fn<PSYNTH_DYNAMIC_AT_C_LIMIT, REM>::template apply<
	    IntTypes,ValueType> (index - PSYNTH_DYNAMIC_AT_C_LIMIT);
	};
	throw;
    }
};

template <>
struct at_c_impl<2>
{
    template <typename IntTypes, typename ValueType> inline
    static ValueType apply (std::size_t index)
    {
	const std::size_t SIZE = boost::mpl::size<IntTypes>::value;
	const std::size_t REM  = SIZE % PSYNTH_DYNAMIC_AT_C_LIMIT;

	switch (index / PSYNTH_DYNAMIC_AT_C_LIMIT)
	{
	case 0: return at_c_fn<0, PSYNTH_DYNAMIC_AT_C_LIMIT-1>::template apply<
	    IntTypes, ValueType> (index);
	case 1: return at_c_fn<
	    PSYNTH_DYNAMIC_AT_C_LIMIT,
	    PSYNTH_DYNAMIC_AT_C_LIMIT-1>::template apply<IntTypes,ValueType>(
		index - PSYNTH_DYNAMIC_AT_C_LIMIT);
	case 2: return at_c_fn<PSYNTH_DYNAMIC_AT_C_LIMIT*2, REM>::template apply<
	    IntTypes, ValueType>(index - PSYNTH_DYNAMIC_AT_C_LIMIT*2);
	};
	throw;
    }
};

template <>
struct at_c_impl<3>
{
    template <typename IntTypes, typename ValueType> inline
    static ValueType apply(std::size_t index)
    {
	const std::size_t SIZE = boost::mpl::size<IntTypes>::value;
	const std::size_t REM  = SIZE % PSYNTH_DYNAMIC_AT_C_LIMIT;
	switch (index / PSYNTH_DYNAMIC_AT_C_LIMIT)
	{
	case 0: return at_c_fn<0, PSYNTH_DYNAMIC_AT_C_LIMIT-1>::template apply<
	    IntTypes, ValueType> (index);
	case 1: return at_c_fn<
	    PSYNTH_DYNAMIC_AT_C_LIMIT,
	    PSYNTH_DYNAMIC_AT_C_LIMIT-1>::template apply<
		IntTypes, ValueType> (index - PSYNTH_DYNAMIC_AT_C_LIMIT);
	case 2: return at_c_fn <
	    PSYNTH_DYNAMIC_AT_C_LIMIT*2,
	    PSYNTH_DYNAMIC_AT_C_LIMIT-1>::template apply<
		IntTypes, ValueType> (index - PSYNTH_DYNAMIC_AT_C_LIMIT*2);
	case 3: return at_c_fn<PSYNTH_DYNAMIC_AT_C_LIMIT*3, REM>::template apply<
	    IntTypes, ValueType> (index - PSYNTH_DYNAMIC_AT_C_LIMIT*3);
	};
	throw;
    }
};

} /* namespace detail */
} /* namespace at_c */

/**
 *
 * \brief Given an MPL Random Access Sequence and a dynamic index n,
 *  returns the value of the n-th element It constructs a lookup table
 *  at compile time
 *
 */
template <typename IntTypes, typename ValueType> inline
ValueType at_c (std::size_t index)
{
    const std::size_t Size = boost::mpl::size<IntTypes>::value;
    return detail::at_c::at_c_impl<
	Size / PSYNTH_DYNAMIC_AT_C_LIMIT>::template apply<
	    IntTypes, ValueType> (index);
}

#undef PSYNTH_AT_C_VALUE
#undef PSYNTH_DYNAMIC_AT_C_LIMIT
#undef PSYNTH_AT_C_LOOKUP

} /* namespace sound */
} /* namespace psynth */

#endif /* namespace PSYNTH_SOUND_DYNAMIC_AT_C */
