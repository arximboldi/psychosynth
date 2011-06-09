/**
 *  Time-stamp:  <2011-06-09 19:53:23 raskolnikov>
 *
 *  @file        exception.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Jun  7 17:41:41 2011
 *
 *  @brief Testing the exception interface.
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

#include <boost/test/unit_test.hpp>
#include <boost/mpl/vector.hpp>

#include <psynth/base/exception.hpp>
#include <psynth/base/throw.hpp>

#define PSYNTH_MODULE_NAME "psynth.test.base.exception"

BOOST_AUTO_TEST_SUITE(base_exception_test_suite)

#define TEST_ERROR_LEVEL   psynth::base::log::warning
#define TEST_ERROR_MESSAGE "test_message"

template <class Exception>
void test_throw ()
{
    PSYNTH_THROW (Exception)
        << TEST_ERROR_LEVEL
        << TEST_ERROR_MESSAGE;
}

typedef boost::mpl::vector<std::logic_error,
                           psynth::base::error>
test_errors;

BOOST_AUTO_TEST_CASE_TEMPLATE(base_test_throw_psynth,
                              Error,
                              test_errors)
{
    typedef psynth::base::detail::exception_wrapper<
        Error,
        std::is_base_of<psynth::base::exception, Error>::value>
        error_actual_type;
    
    BOOST_CHECK_THROW (test_throw<Error> (), error_actual_type);
    BOOST_CHECK_THROW (test_throw<Error> (), Error);
    BOOST_CHECK_THROW (test_throw<Error> (), std::exception);
    BOOST_CHECK_THROW (test_throw<Error> (), psynth::base::exception);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(base_test_catch,
                              Error,
                              test_errors)
{
    bool entered = false;
    
    try { test_throw<Error> (); }
    catch (psynth::base::exception& err)
    {
        entered = true;
        BOOST_CHECK (strcmp (err.what (), TEST_ERROR_MESSAGE) == 0);
        BOOST_CHECK (strcmp (err.where (), PSYNTH_MODULE_NAME) == 0);
        BOOST_CHECK (err.level () == TEST_ERROR_LEVEL);
    }
    
    BOOST_CHECK (entered);
}

BOOST_AUTO_TEST_SUITE_END ()
