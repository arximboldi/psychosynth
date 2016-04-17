/**
 *  Time-stamp:  <2012-02-05 08:42:55 raskolnikov>
 *
 *  @file        util.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar 21 18:04:40 2011
 *
 *  @brief Synthesis utilities of various kinds.
 *
 *  @todo Use more generic arithmetic.
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

#ifndef PSYNTH_SYNTH_UTIL_H_
#define PSYNTH_SYNTH_UTIL_H_

#include <psynth/sound/forwards.hpp>
#include <psynth/sound/algorithm.hpp>

namespace psynth
{
namespace synth
{

/**
 * @todo Fix and probably remove.
 */
template <class Range>
std::size_t find_hill (const Range& data, std::size_t start)
{
    typedef typename sound::sample_type<Range>::type sample_type;
    const auto zero = sound::sample_traits<sample_type>::zero_value ();
    ++start;

    while (start < (size_t) data.size () &&
           (data [start-1] != zero ||
            data [start] == zero))
        ++start;

    return start;
}

template <class R1, class R2, class R3>
void mix (const R1& src1, const R2& src2, const R3& dst)
{
    typedef typename R1::value_type src1_frame;
    typedef typename R2::value_type src2_frame;
    typedef typename R3::value_type dst_frame;

    // FIXME: Only for homogeneous frames!

    typedef std::plus<typename sound::sample_type<dst_frame>::type> mix_op;

    sound::transform_frames (
        src1, src2, dst,
        [] (const src1_frame& a, const src2_frame& b)
        {
            dst_frame res;
            static_transform (a, b, res, mix_op ());
            return res;
        });
}

template <class R1, class R2, class R3, typename Sample>
void mix (const R1& src1, const R2& src2, Sample ampl, const R3& dst)
{
    typedef typename R1::value_type src1_frame;
    typedef typename R2::value_type src2_frame;
    typedef typename R3::value_type dst_frame;
    typedef typename sound::sample_type<dst_frame>::type sample_type;

    sound::transform_frames (
        src1, src2, dst,
        [&] (const src1_frame& a, const src2_frame& b)
        {
            dst_frame res;
            static_transform (
                a, b, res,
                [&] (sample_type a, sample_type b) {
                    return a + b * ampl;
                });
            return res;
        });
}

template <class R1, class R2, class R3>
void modulate (const R1& src1, const R2& src2, const R3& dst)
{
    typedef typename R1::value_type src1_frame;
    typedef typename R2::value_type src2_frame;
    typedef typename R3::value_type dst_frame;

    // FIXME: Only for homogeneous frames!

    typedef std::multiplies<typename sound::sample_type<dst_frame>::type> mix_op;

    sound::transform_frames (
        src1, src2, dst,
        [] (const src1_frame& a, const src2_frame& b)
        {
            dst_frame res;
            static_transform (a, b, res, mix_op ());
            return res;
        });
}

template <class R1, class R2, class R3, typename Sample>
void modulate (const R1& src1, const R2& src2, Sample ampl, const R3& dst)
{
    typedef typename R1::value_type src1_frame;
    typedef typename R2::value_type src2_frame;
    typedef typename R3::value_type dst_frame;
    typedef typename sound::sample_type<dst_frame>::type sample_type;

    sound::transform_frames (
        src1, src2, dst,
        [&] (const src1_frame& a, const src2_frame& b)
        {
            dst_frame res;
            static_transform (
                a, b, res,
                [&] (sample_type aa, sample_type bb) {
                    return aa * bb * ampl;
                });
            return res;
        });
}


template <class R1, class R2, class R3, typename Sample>
void blend (const R1& src1, const R2& src2, Sample stable, const R3& dst)
{
    typedef typename R1::value_type src1_frame;
    typedef typename R2::value_type src2_frame;
    typedef typename R3::value_type dst_frame;

    typedef typename sound::sample_type<src1_frame>::type sample_type;
    typedef sound::sample_multiplier<sample_type> blend_op;
    const auto max_value = sound::sample_traits<sample_type>::max_value ();

    sound::transform_frames (
        src1, src2, dst,
        [&] (const src1_frame& a, const src2_frame& b)
        {
            dst_frame res;
            static_transform (
                a, b, res, [&] (const sample_type a, sample_type b) {
                    return // FIXME: sample_multiply ?
                        a * b + stable * (max_value - b);
                });
            return res;
        });
}

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_UTIL_H_ */
