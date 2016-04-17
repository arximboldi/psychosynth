/**
 *  @file        exception.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2009
 *
 *  Exception handling system implementation.
 */

/*
 *  Copyright (C) 2009, 2016 Juan Pedro Bolívar Puente
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

#include "base/exception.hpp"
#include "base/logger.hpp"

#define PSYNTH_MODULE_NAME "psynth.base.exception"

namespace psynth
{
namespace base
{

PSYNTH_DEFINE_ERROR (error);

int default_error_level () noexcept
{
    return log::error;
}

int error_base::level () const noexcept
{
    return default_error_level ();
}

const char* error_base::default_what () const noexcept
{
    return "unknown error";
}

void exception::log () const
{
    logger::self () (where (), level (), what ());
}

} /* namespace base */
} /* namespace psynth */
