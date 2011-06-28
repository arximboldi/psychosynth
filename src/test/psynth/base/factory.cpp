/**
 *  Time-stamp:  <2011-06-09 21:16:07 raskolnikov>
 *
 *  @file        factory.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 19:24:30 2011
 *
 *  @brief Unit tests for new factories.
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

#include <psynth/base/factory.hpp>
#include <psynth/base/factory_manager.hpp>

BOOST_AUTO_TEST_SUITE (base_factory_test_suite);

struct some_base
{
    int _x;
    virtual ~some_base () {}
};

struct some_concrete : some_base
{
    some_concrete (int x = 0) { _x = x + 1; };
};

struct other_concrete : some_base
{
    other_concrete (int x = 0) { _x = x - 1; };
};

typedef std::unique_ptr<some_base> some_base_ptr;

BOOST_AUTO_TEST_CASE (factory_test)
{    
    psynth::base::factory<some_base_ptr, some_concrete> f1;
    psynth::base::factory<some_base_ptr, other_concrete> f2;

    psynth::base::factory_base<some_base_ptr>* f3 = &f1;
    auto res = f3->create ();
    BOOST_CHECK_EQUAL (res->_x, 1);

    f3 = &f2;
    res = f3->create ();
    BOOST_CHECK_EQUAL (res->_x, -1);
}

BOOST_AUTO_TEST_CASE (factory_test_param)
{
    psynth::base::factory<some_base_ptr, some_concrete, int> f1;
    psynth::base::factory<some_base_ptr, other_concrete, int> f2;

    psynth::base::factory_base<some_base_ptr, int>* f3 = &f1;
    auto res = f3->create (3);
    BOOST_CHECK_EQUAL (res->_x, 4);

    f3 = &f2;
    res = f3->create (3);
    BOOST_CHECK_EQUAL (res->_x, 2);
}

BOOST_AUTO_TEST_CASE (factory_test_manager)
{
    psynth::base::factory_manager<std::string, some_base_ptr> mgr;

    mgr.add<some_concrete> ("some");
    mgr.add<other_concrete> ("other");
    
    auto res = mgr.create ("some");
    BOOST_CHECK_EQUAL (res->_x, 1);
    res = mgr.create ("other");
    BOOST_CHECK_EQUAL (res->_x, -1);
}

typedef psynth::base::restricted_global_factory_manager<
    std::string, some_base_ptr>
some_base_factory;

some_base_factory::registrant<some_concrete>  some_registrant ("some");
some_base_factory::registrant<other_concrete> other_registrant ("other");

BOOST_AUTO_TEST_CASE (factory_test_global_manager)
{
    auto& mgr = some_base_factory::self ();

    auto res = mgr.create ("some");
    BOOST_CHECK_EQUAL (res->_x, 1);
    res = mgr.create ("other");
    BOOST_CHECK_EQUAL (res->_x, -1);
}

BOOST_AUTO_TEST_SUITE_END ();
