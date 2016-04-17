/**
 *  Time-stamp:  <2011-03-21 16:38:24 raskolnikov>
 *
 *  @file        input_fwd.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar 18 12:50:15 2011
 *
 *  @brief Input forwards.
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

#ifndef PSYNTH_IO_INPUT_FWD_H_
#define PSYNTH_IO_INPUT_FWD_H_

#include <psynth/version.hpp>
#include <psynth/base/declare.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_SHARED_TEMPLATE(input, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(async_input, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(dummy_input, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(dummy_async_input, class);

PSYNTH_DECLARE_SHARED_TEMPLATE(buffered_input, class, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(buffered_input_adapter, class, class);

#ifdef PSYNTH_HAVE_PCM
PSYNTH_DECLARE_SHARED_TEMPLATE(file_input_base, class);
PSYNTH_DECLARE_SHARED_TEMPLATE(file_input, class);
#endif /* PSYNTH_HAVE_PCM */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_INPUT_FWD_H_ */
