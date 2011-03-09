/**
 *  Time-stamp:  <2011-03-09 18:22:51 raskolnikov>
 *
 *  @file        oss_output.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Mar  7 17:01:23 2011
 *
 *  Oss output implementation.
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

#ifndef PSYNTH_IO_OSS_OUTPUT_TPP_
#define PSYNTH_IO_OSS_OUTPUT_TPP_

#include <sys/soundcard.h>

#include <boost/mpl/int.hpp>
#include <psynth/new_io/oss_output.hpp>

namespace psynth
{
namespace io
{

namespace mpl = boost::mpl;

namespace detail
{

/**
 * @todo It seems that we should use AFMT_UNDEF for the "error"
 * format, but sadly it is not working. We thus use this obsolete
 * format that we won't want to use in the real world anyway.
 */
#define PSYNTH_OSS_UNDEF AFMT_S8

template <typename Sample>
struct oss_format : public mpl::int_<PSYNTH_OSS_UNDEF> {};

#define PSYNTH_DECLARE_OSS_FORMAT(pfmt, afmt) \
    template <> struct oss_format<pfmt> : public mpl::int_<afmt> {};

PSYNTH_DECLARE_OSS_FORMAT (sound::bits8,    AFMT_U8);
//PSYNTH_DECLARE_OSS_FORMAT (sound::bits8s,   AFMT_S8);
//PSYNTH_DECLARE_OSS_FORMAT (sound::bits16,   AFMT_U16_NE);
PSYNTH_DECLARE_OSS_FORMAT (sound::bits16s,  AFMT_S16_NE);
//PSYNTH_DECLARE_OSS_FORMAT (sound::bits32,   AFMT_U32_NE);
//PSYNTH_DECLARE_OSS_FORMAT (sound::bits32s,  AFMT_S32_NE);
//PSYNTH_DECLARE_OSS_FORMAT (sound::bits32sf, AFMT_FLOAT);

} /* namespace detail */

template <typename Range>
struct oss_support
{
    typedef typename detail::oss_format<
        typename sound::sample_type<Range>::type >::type format;
    typedef typename sound::is_planar<Range>::type is_planar;
    typedef typename sound::num_samples<Range>::type channels;
    
    typedef typename mpl::and_<mpl::not_<
                                   mpl::equal_to<mpl::int_<PSYNTH_OSS_UNDEF>,
                                                 format> >,
                               mpl::not_<is_planar> >::type
    is_supported;
};

template <typename Range>
oss_output<Range>::oss_output (const std::string& device,
                               std::size_t        buffer_size,
                               std::size_t        rate,
                               callback_type      cb)
    : async_base (cb)
    , oss_raw_output (device.c_str (),
                      oss_support<Range>::format::value,
                      sizeof (typename sound::sample_type<Range>::type),
                      buffer_size,
                      !oss_support<Range>::is_planar::value,
                      rate,
                      oss_support<Range>::channels::value)
{
}

template <typename Range>
std::size_t oss_output<Range>::put (const range& data)
{
    return put_on_raw (*this, data);
}

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_OSS_OUTPUT_TPP_ */

