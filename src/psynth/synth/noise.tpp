/**
 *  Time-stamp:  <2012-02-05 08:38:44 raskolnikov>
 *
 *  @file        noise.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Feb  4 23:36:17 2012
 *
 *  @brief Noise generation implementation.
 */

/*
 *  Copyright (C) 2012 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_SYNTH_NOISE_TPP_
#define PSYNTH_SYNTH_NOISE_TPP_

#include <psynth/synth/noise.hpp>

#include <psynth/sound/algorithm.hpp>
#include <psynth/sound/channel_base_algorithm.hpp>

namespace psynth
{
namespace synth
{

template <class D, class G>
template <class Range>
void noise<D, G>::update (const Range& out_buf)
{
    typedef typename Range::value_type outval;

    sound::generate_frames(out_buf, [&]() -> outval {
            typedef typename sound::sample_type<outval>::type sample;
            outval result;
            sound::static_generate(result, [&]() -> sample {
                    return _distribution(this->_generator);
                });
            return result;
        });
}


template <class Sample>
pink_noise_distribution<Sample>::pink_noise_distribution()
    : _b0 (.0f), _b1 (.0f), _b2 (.0f), _b3 (.0f), _b4 (.0f), _b5 (.0f), _b6 (.0f)
{
}

template <class Sample>
template <class Engine>
Sample pink_noise_distribution<Sample>::operator()(Engine& engine)
{
    Sample white = 0.2 * white_noise_distribution<Sample>::operator()(engine);

    _b0 = 0.99886f * _b0 + white * 0.0555179f;
    _b1 = 0.99332f * _b1 + white * 0.0750759f;
    _b2 = 0.96900f * _b2 + white * 0.1538520f;
    _b3 = 0.86650f * _b3 + white * 0.3104856f;
    _b4 = 0.55000f * _b4 + white * 0.5329522f;
    _b5 = -0.7616f * _b5 - white * 0.0168980f;

    Sample pink = _b0 + _b1 + _b2 + _b3 + _b4 + _b5 + _b6 + white * 0.5362f;

    _b6 = white * 0.115926f;

    return pink;
}

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_SYNTH_NOISE_TPP_ */
