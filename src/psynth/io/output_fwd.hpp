/**
 *  Time-stamp:  <2011-03-18 19:24:16 raskolnikov>
 *
 *  @file        output_fwd.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar 18 12:37:16 2011
 *
 *  @brief Output forward declarations.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynht.
 *   
 *  Psychosynht is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynht is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_IO_OUTPUT_FWD_H_
#define PSYNTH_IO_OUTPUT_FWD_H_

#include <psynth/version.hpp>
#include <psynth/base/declare.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_SHARED_TEMPLATE(output, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(async_output, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(dummy_output, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(dummy_async_output, class);

PSYNTH_DECLARE_SHARED_TEMPLATE(buffered_output, class, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(buffered_async_output, class, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(buffered_output_adapter, class, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(buffered_async_output_adapter, class, class);

#ifdef PSYNTH_HAVE_ALSA
PSYNTH_DECLARE_SHARED_TEMPLATE(alsa_output, class);
#endif /* PSYNTH_HAVE_ALSA */

#ifdef PSYNTH_HAVE_OSS
PSYNTH_DECLARE_SHARED_TEMPLATE(oss_output, class);
#endif /* PSYNTH_HAVE_OSS */

#ifdef PSYNTH_HAVE_JACK
PSYNTH_DECLARE_SHARED_TEMPLATE(jack_output, class);
#endif /* PSYNTH_HAVE_JACK */

#ifdef PSYNTH_HAVE_PCM
PSYNTH_DECLARE_SHARED_TEMPLATE(file_output, class);
#endif /* PSYNTH_HAVE_PCM */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_OUTPUT_FWD_H_ */
