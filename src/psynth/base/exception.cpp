/**
 *  Time-stamp:  <2009-04-27 16:37:18 raskolnikov>
 *
 *  @file        exception.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2009
 *
 *  Exception handling system implementation.
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
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

namespace psynth
{

PSYNTH_DEFINE_ERROR_WHERE (base_error,   "base");
PSYNTH_DEFINE_ERROR_WHERE (psynth_error, "psynth");

error::error (const std::string& where, const std::string& what) throw ()
    : m_what (what)
    , m_where (where)
{
}

std::string error::default_error ()
{
    return "unknown message";
}

void error::log () const
{
    logger::self  () (m_where, log::error, m_what);
}

} /* namespace psynth */

