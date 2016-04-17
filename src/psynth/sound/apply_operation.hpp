/**
 *  Time-stamp:  <2010-11-03 21:05:33 raskolnikov>
 *
 *  @file        apply_operation.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 21:02:18 2010
 *
 *  Implements apply_operation for variants. Optionally performs type
 *  reduction.
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

#ifndef PSYNTH_SOUND_APPLY_OPERATION_HPP
#define PSYNTH_SOUND_APPLY_OPERATION_HPP

#include <psynth/sound/apply_operation_base.hpp>
#include <psynth/sound/variant.hpp>

#ifndef PSYNTH_REDUCE_CODE_BLOAT

namespace psynth
{
namespace sound
{

/**
 * \ingroup Variant
 * \brief Invokes a generic mutable operation (represented as a unary
 * function object) on a variant
 */
template <typename Types, typename UnaryOp> PSYNTH_FORCEINLINE
typename UnaryOp::result_type apply_operation (variant<Types>& arg, UnaryOp op)
{
    return apply_operation_base<Types>(arg._bits, arg._index ,op);
}

/**
 * \ingroup Variant
 * \brief Invokes a generic constant operation (represented as a unary
 * function object) on a variant
 */
template <typename Types, typename UnaryOp> PSYNTH_FORCEINLINE
typename UnaryOp::result_type apply_operation (
    const variant<Types>& arg, UnaryOp op)
{
    return apply_operation_basec<Types>(arg._bits, arg._index ,op);
}

/**
 * \ingroup Variant
 * \brief Invokes a generic constant operation (represented as a
 * binary function object) on two variants.
 */
template <typename Types1, typename Types2, typename BinaryOp> PSYNTH_FORCEINLINE
typename BinaryOp::result_type apply_operation (
    const variant<Types1>& arg1, const variant<Types2>& arg2, BinaryOp op)
{
    return apply_operation_base<Types1,Types2>(
	arg1._bits, arg1._index, arg2._bits, arg2._index, op);
}

} /* namespace sound */
} /* namespace psynth */

#else

#include "reduce.hpp"

#endif

#endif /* PSYNTH_SOUND_APPLY_OPERATION */
