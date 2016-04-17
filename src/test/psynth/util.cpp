/**
 *  Time-stamp:  <2011-03-09 17:18:40 raskolnikov>
 *
 *  @file        util.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 16:11:32 2011
 *
 *  Implementation of testing utilities.
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

#include "util.hpp"

namespace psynth
{
namespace test
{

namespace
{

/*
 * HACK: When a test aborts because of a signal, the logger remains
 * in the global variable as the destructor of the fixture never gets
 * called. This kind of fixes it.
 */
int std_logger_count = 0;

} /* anonymous namespace */

std_logger_fixture::std_logger_fixture ()
{
    if (std_logger_count == 0)
    {
        _sink = base::new_log_std_sink ();
        base::logger::self ().add_sink (_sink);
        ++std_logger_count;
    }
}

std_logger_fixture::~std_logger_fixture ()
{
    base::logger::self ().del_sink (_sink);
    --std_logger_count;
}

} /* namespace test */
} /* namespace psynth */
