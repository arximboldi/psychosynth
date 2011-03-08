/**
 *  Time-stamp:  <2011-03-07 19:15:49 raskolnikov>
 *
 *  @file        alsa_output.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 16:03:35 2011
 *
 *  Implementation of the ALSA output system.
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

#ifndef PSYNTH_IO_ALSA_OUTPUT_TPP_
#define PSYNTH_IO_ALSA_OUTPUT_TPP_

#include <psynth/new_io/alsa_output.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <snd_pcm_format_t Value>
struct alsa_format_c : public mpl::integral_c<snd_pcm_format_t, Value> {};
template <snd_pcm_access_t Value>
struct alsa_access_c : public mpl::integral_c<snd_pcm_access_t, Value> {};

template <typename Sample>
struct alsa_format : public alsa_format_c<SND_PCM_FORMAT_UNKNOWN> {};

#define PSYNTH_DECLARE_ALSA_FORMAT(pfmt, afmt) \
    template <> struct alsa_format<pfmt> : public alsa_format_c<afmt> {};

PSYNTH_DECLARE_ALSA_FORMAT (sound::bits8,    SND_PCM_FORMAT_U8);
PSYNTH_DECLARE_ALSA_FORMAT (sound::bits8s,   SND_PCM_FORMAT_S8);
PSYNTH_DECLARE_ALSA_FORMAT (sound::bits16,   SND_PCM_FORMAT_U16);
PSYNTH_DECLARE_ALSA_FORMAT (sound::bits16s,  SND_PCM_FORMAT_S16);
PSYNTH_DECLARE_ALSA_FORMAT (sound::bits32,   SND_PCM_FORMAT_U32);
PSYNTH_DECLARE_ALSA_FORMAT (sound::bits32s,  SND_PCM_FORMAT_S32);
PSYNTH_DECLARE_ALSA_FORMAT (sound::bits32sf, SND_PCM_FORMAT_FLOAT);

template <typename Range>
struct alsa_access : public mpl::if_<
    sound::is_planar<Range>::type,
    alsa_access_c<SND_PCM_ACCESS_RW_NONINTERLEAVED>,
    alsa_access_c<SND_PCM_ACCESS_RW_INTERLEAVED> >;

} /* namespace detail */

template <typename Range>
struct alsa_support
{
    typedef detail::alsa_format<sound::sample_type<Range>::type>::type format;
    typedef detail::alsa_access<Range>::type access;
    typedef sound::num_samples<Range>::type channels;

    typedef mpl::not_<
        mpl::eq_<detail::alsa_format_c<SND_PCM_FORMAT_UNKNOWN>,
                 format> >::type
    is_supported;
};

template <typename Range>
alsa_output<Range>::alsa_output (const std::string& device,
                                 std::size_t        buffer_size,
                                 std::size_t        rate)
    : alsa_raw_output (device.c_str (),
                       alsa_support<Range>::format::value,
                       buffer_size,
                       alsa_support<Range>::access::value,
                       rate,
                       alsa_support<Range>::channels::value)
{
}

template <typename Range>
std::size_t alsa_output<Range>::put (const range& data)
{
    return put_on_raw (*this, data);
}

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_ALSA_OUTPUT_TPP_ */

