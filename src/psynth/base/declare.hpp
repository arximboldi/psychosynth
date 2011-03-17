/**
 *  Time-stamp:  <2011-03-16 19:08:29 raskolnikov>
 *
 *  @file        declare.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Oct 18 15:11:23 2010
 *
 *  @brief Aiding in forward declaring types.
 *  @todo new_xxx function should use std::forward when available.
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

#include <utility>
#include <memory>

namespace psynth
{
namespace base
{

#define PSYNTH_DECLARE_SHARED_POINTER(type_name)                        \
    typedef std::shared_ptr<type_name> type_name ## _ptr;               \
    template <typename... Args>                                         \
    type_name ## _ptr new_ ## type_name (Args && ... args)              \
    {                                                                   \
        return std::make_shared <type_name> (args ...);                 \
    }

#define PSYNTH_DECLARE_TYPE(type_name)		\
    struct type_name;

#define PSYNTH_DECLARE_SHARED_TYPE(type_name)	\
    PSYNTH_DECLARE_TYPE (type_name)		\
    PSYNTH_DECLARE_SHARED_POINTER (type_name)

} /* namespace base */
} /* namespace psynth */
