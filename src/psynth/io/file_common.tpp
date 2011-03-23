/**
 *  Time-stamp:  <2011-03-16 23:38:04 raskolnikov>
 *
 *  @file        file_common.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 16:51:16 2011
 *
 *  File common utilities, template implementation.
 *
 *  @todo Actually the _impl methods have grown so much that they
 *  could be abstracted as a class.
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

#ifndef PSYNTH_IO_FILE_COMMON_TPP_
#define PSYNTH_IO_FILE_COMMON_TPP_

#include <boost/mpl/int.hpp>

#include <psynth/version.hpp>
#include <psynth/sound/sample.hpp>
#include <psynth/io/file_common.hpp>

#ifdef PSYNTH_HAVE_PCM

#include <sndfile.h>

namespace psynth
{
namespace io
{

namespace mpl = boost::mpl;

namespace detail
{

template <class Sample>
struct file_format : public mpl::int_<SF_FORMAT_ENDMASK> {};

#define PSYNTH_DECLARE_FILE_FORMAT(pfmt, afmt)                          \
    template <> struct file_format<pfmt> : public mpl::int_<afmt> {};

// TODO: The sndfile library supports 8 bits but not in its API.
// Because want to do the format conversion ourselves this ain't
// the way to go.
// PSYNTH_DECLARE_ALSA_FORMAT (sound::bits8,    SF_FORMAT_PCM_U8);
// PSYNTH_DECLARE_ALSA_FORMAT (sound::bits8s,   SF_FORMAT_PCM_S8);

PSYNTH_DECLARE_FILE_FORMAT (sound::bits16s,  SF_FORMAT_PCM_16);
PSYNTH_DECLARE_FILE_FORMAT (sound::bits32s,  SF_FORMAT_PCM_32);
PSYNTH_DECLARE_FILE_FORMAT (sound::bits32sf, SF_FORMAT_FLOAT);

} /* namespace detail */

template <class Range>
struct file_support
{
    typedef typename detail::file_format<
        typename sound::sample_type<Range>::type>::type format;
    
    typedef typename mpl::and_<
        mpl::not_<mpl::equal_to<format, mpl::int_<SF_FORMAT_ENDMASK> > >,
        mpl::not_<typename sound::is_planar<Range>::type> >
    is_supported;
};

namespace detail
{

SNDFILE* file_open_impl (const char* fname, int mode, SF_INFO* info);

int file_format_impl (file_fmt format, int sample_format);

std::size_t file_seek_impl (SNDFILE* file,
                            std::ptrdiff_t offset, seek_dir dir);

void file_close_impl (SNDFILE* file);

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */

#else

namespace psynth
{
namespace io
{

template <class Range>
struct file_support
{
    typedef mpl::false_ is_supported;
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_HAVE_PCM */

#endif /* PSYNTH_IO_FILE_COMMON_TPP_ */
