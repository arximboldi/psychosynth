/**
 *  Time-stamp:  <2010-11-02 22:36:19 raskolnikov>
 *
 *  @file        logger_def.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Nov  2 20:31:31 2010
 *
 *  Forward declarations for the logger.
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

#ifndef PSYNTH_BASE_LOGGER_DEF_H_
#define PSYNTH_BASE_LOGGER_DEF_H_

#include <psynth/base/declare.hpp>

namespace psynth
{
namespace base
{

PSYNTH_DECLARE_SHARED_TYPE (log_sink);
PSYNTH_DECLARE_SHARED_TYPE (log_stream_sink);
PSYNTH_DECLARE_SHARED_TYPE (log_file_sink);
PSYNTH_DECLARE_SHARED_TYPE (log_std_sink);
PSYNTH_DECLARE_TYPE (log);
PSYNTH_DECLARE_TYPE (log_stream_adapter);
PSYNTH_DECLARE_TYPE (logger);

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_LOGGER_DEF_H_ */


