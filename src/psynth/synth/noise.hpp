/*
 *  File:       noise.hpp
 *  Author:     Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  Date:       Sat Feb  4 22:58:02 2012
 *  Time-stamp: <2012-02-05 08:38:29 raskolnikov>
 *
 *  Noise generation.
 */

/*
 *  Copyright (C) 2012 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_SYNTH_NOISE_HPP_
#define PSYNTH_SYNTH_NOISE_HPP_

#include <random>
#include <type_traits>

#include <boost/mpl/if.hpp>

#include <psynth/sound/sample.hpp>

namespace psynth
{
namespace synth
{

typedef std::mt19937 default_noise_generator;

template <typename SampleType>
class white_noise_distribution
{
    typedef typename boost::mpl::if_<
        typename std::is_integral<SampleType>::type,
        std::uniform_int_distribution<SampleType>,
        std::uniform_real_distribution<SampleType>>::type
        uniform_distribution_type;
    
public:
    typedef SampleType result_type;
    
    white_noise_distribution()
        : _uniform(sound::sample_traits<SampleType>::min_value (),
                   sound::sample_traits<SampleType>::max_value ())
    {}
    white_noise_distribution(const white_noise_distribution&) = default;
    white_noise_distribution& operator=(const white_noise_distribution&) = default;

    template <typename Engine>
    SampleType operator() (Engine& engine)
    { return _uniform (engine); }
    
private:
    uniform_distribution_type _uniform;
};


template<typename SampleType>
class pink_noise_distribution : public white_noise_distribution<SampleType>
{
public:
    pink_noise_distribution();
    pink_noise_distribution(const pink_noise_distribution&) = default;
    pink_noise_distribution& operator=(const pink_noise_distribution&) = default;
    
    template <typename GenEngine>
    SampleType operator()(GenEngine& gen);
    
private:
    SampleType _b0, _b1, _b2, _b3, _b4, _b5, _b6;
};


template<typename Distribution,
         typename Generator = default_noise_generator>
class noise
{
public:
    noise(const Distribution& distribution = Distribution(),
          const Generator&    generator    = Generator())
        : _distribution(distribution)
        , _generator(generator)
    {}

    noise(const noise&) = default;
    noise& operator=(const noise&) = default;
    
    template <class Range>
    void update (const Range& out_buf);

    void distribution(const Distribution& d)
    { _distribution = d; };
    
    const Distribution& distribution() const
    { return _distribution; }
    
    Distribution& distribution()
    { return _distribution; }

    
    void generator(const Generator& d)
    { _generator = d; };
    
    const Generator& generator() const
    { return _generator; }
    
    Generator& generator()
    { return _generator; }
    
private:
    Distribution _distribution;
    Generator    _generator;
};


template<typename SampleType, typename Generator = default_noise_generator>
class white_noise : public noise<white_noise_distribution<SampleType>, Generator>
{};

template<typename SampleType, typename Generator = default_noise_generator>
class pink_noise : public noise<white_noise_distribution<SampleType>, Generator>
{};


} /* namespace synth */
} /* namespace psynth */

#include <psynth/synth/noise.tpp>

#endif /* PSYNTH_SYNTH_NOISE_HPP_ */
