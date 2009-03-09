/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2009                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <psynth/base/arg_parser.hpp>

static const char* argv_0 []  = {"cmd", "-a", "hola", "-b", "asd", "--bold", "-123"};
static const int   argc_0 = 7;

static const char* argv_1 []  = {"cmd", "-a", "hola", "-b", "1", "--bold", "-123"};
static const int   argc_1 = 7;

BOOST_AUTO_TEST_SUITE (arg_parser_test_suite_0)

BOOST_AUTO_TEST_CASE (arg_paser_test_0)
{
    bool arg_bool = false;
    int arg_int = 0;

    psynth::arg_parser p;
    p.add ('a', 0, &arg_bool);
    p.add ('b', "bold", &arg_int);

    BOOST_CHECK_THROW (p.parse (argc_0, argv_0), psynth::arg_parser_error);

    BOOST_CHECK_NO_THROW (p.parse (argc_1, argv_1));
    BOOST_CHECK_EQUAL (p.has_free_args (), true);
    BOOST_CHECK_EQUAL (p.free_args (), 1);
    BOOST_CHECK_EQUAL (*p.begin (), argv_1 [2]);
    BOOST_CHECK_EQUAL (arg_bool, true);
    BOOST_CHECK_EQUAL (arg_int, -123);
}

BOOST_AUTO_TEST_CASE (arg_paser_test_2)
{
    bool arg_bool = false;
    float arg_float = 0;
    
    psynth::arg_parser p;
    p.add ('a', 0, &arg_bool);
    p.add ('b', "bold", &arg_float);

    BOOST_CHECK_THROW (p.parse (argc_0, argv_0), psynth::arg_parser_error);

    BOOST_CHECK_NO_THROW (p.parse (argc_1, argv_1));
    BOOST_CHECK_EQUAL (p.has_free_args (), true);
    BOOST_CHECK_EQUAL (p.free_args (), 1);
    BOOST_CHECK_EQUAL (*p.begin (), argv_1 [2]);
    BOOST_CHECK_EQUAL (arg_bool, true);
    BOOST_CHECK_EQUAL (arg_float, -123);
}

BOOST_AUTO_TEST_SUITE_END ()