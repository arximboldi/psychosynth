/**
 *  Time-stamp:  <2011-03-09 16:14:10 raskolnikov>
 *
 *  @file        util.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Mar  9 16:09:43 2011
 *
 *  Utilities for testing modules.
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

#ifndef PSYNTH_TEST_UTIL_H_
#define PSYNTH_TEST_UTIL_H_

#include <psynth/base/logger.hpp>

namespace psynth
{
namespace test
{

class std_logger_fixture
{
public:
    std_logger_fixture ();
    ~std_logger_fixture ();

private:
    base::log_sink_ptr _sink;
};

} /* namespace test */
} /* namespace psynth */

#endif /* PSYNTH_TEST_UTIL_H_ */
