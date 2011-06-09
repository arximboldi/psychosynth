/**
 *  Time-stamp:  <2011-06-09 18:12:29 raskolnikov>
 *
 *  @file        hetero_queue.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun  8 13:02:15 2011
 *
 *  @brief Tests for the hetero_queue class.
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

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <psynth/base/hetero_deque.hpp>

BOOST_AUTO_TEST_SUITE(base_hetero_deque_test_suite)

struct test_base
{
    int count;
    test_base () : count (0) {}
    virtual ~test_base () {};
    virtual void method () {};
};

struct test_count : test_base
{ 
    void method () { ++count; }
};

struct test_count_dec : test_base
{
    int dec_count;
    test_count_dec () : dec_count (0) {} 
    void method () { ++dec_count; --count; }
};

struct test_except : test_base
{
    test_except () { throw std::logic_error ("except"); };
};

typedef psynth::base::hetero_deque<test_base> test_deque;

BOOST_AUTO_TEST_CASE(hetero_deque_test_too_small)
{
    test_deque q;

    BOOST_CHECK_THROW (q.front (), psynth::base::hetero_deque_empty);
    BOOST_CHECK_THROW (q.back (), psynth::base::hetero_deque_empty);

    BOOST_CHECK_EQUAL (q.push_back<test_count> (), false);
    BOOST_CHECK_EQUAL (q.push_back<test_count_dec> (), false);

    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_THROW (q.front (), psynth::base::hetero_deque_empty);
    BOOST_CHECK_THROW (q.back (), psynth::base::hetero_deque_empty);
}

BOOST_AUTO_TEST_CASE(hetero_deque_test_some_back)
{
    test_deque q (1024);

    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_EQUAL (q.push_back<test_count> (), true);
    BOOST_CHECK_EQUAL (q.push_back<test_count_dec> (), true);
    BOOST_CHECK_EQUAL (q.empty (), false);
    
    q.front ().method ();
    q.back ().method ();

    BOOST_CHECK_EQUAL (q.front ().count, 1);
    BOOST_CHECK_EQUAL (q.back ().count, -1);
    BOOST_CHECK_EQUAL (
        dynamic_cast<test_count_dec&>(q.back ()).dec_count, 1);

    BOOST_CHECK_EQUAL (q.pop_back (), true);
    BOOST_CHECK_EQUAL (q.empty (), false);
    BOOST_CHECK_EQUAL (q.back ().count, 1);
    BOOST_CHECK_EQUAL (&q.back (), &q.front ());

    BOOST_CHECK_EQUAL (q.pop_back (), true);
    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_THROW (q.front (), psynth::base::hetero_deque_empty);
    BOOST_CHECK_THROW (q.back (), psynth::base::hetero_deque_empty);
}

BOOST_AUTO_TEST_CASE(hetero_deque_test_some_front)
{
    test_deque q (1024);

    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_EQUAL (q.push_front<test_count> (), true);
    BOOST_CHECK_EQUAL (q.push_front<test_count_dec> (), true);
    BOOST_CHECK_EQUAL (q.empty (), false);
    
    q.front ().method ();
    q.back ().method ();

    BOOST_CHECK_EQUAL (q.back ().count, 1);
    BOOST_CHECK_EQUAL (q.front ().count, -1);
    BOOST_CHECK_EQUAL (
        dynamic_cast<test_count_dec&>(q.front ()).dec_count, 1);

    BOOST_CHECK_EQUAL (q.pop_front (), true);
    BOOST_CHECK_EQUAL (q.empty (), false);
    BOOST_CHECK_EQUAL (q.front ().count, 1);
    BOOST_CHECK_EQUAL (&q.front (), &q.front ());

    BOOST_CHECK_EQUAL (q.pop_front (), true);
    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_THROW (q.front (), psynth::base::hetero_deque_empty);
    BOOST_CHECK_THROW (q.back (), psynth::base::hetero_deque_empty);
}

BOOST_AUTO_TEST_CASE(hetero_deque_test_some_mixed)
{
    test_deque q (1024);

    while (q.push_front<test_count> ());
    q.pop_back ();
    q.pop_back ();
    q.pop_back ();
    while (q.push_back<test_count_dec> ());
        
    q.front ().method ();
    q.back ().method ();

    BOOST_CHECK_EQUAL (q.front ().count, 1);
    BOOST_CHECK_EQUAL (q.back ().count, -1);
}

BOOST_AUTO_TEST_CASE(hetero_deque_test_except)
{
    test_deque q (1024);

    BOOST_CHECK_EQUAL (q.push_back<test_count> (), true);
    BOOST_CHECK_EQUAL (q.push_back<test_count_dec> (), true);
    BOOST_CHECK_THROW (q.push_back<test_except> (), std::logic_error);
    
    q.front ().method ();
    q.back ().method ();

    BOOST_CHECK_EQUAL (q.front ().count, 1);
    BOOST_CHECK_EQUAL (q.back ().count, -1);
}

BOOST_AUTO_TEST_CASE(hetero_deque_iter)
{
    test_deque q (1024);
    
    for (int i = 0; i < 8; ++i)
    {
        BOOST_CHECK_EQUAL (q.push_back<test_count> (), true);
        BOOST_CHECK_EQUAL (q.push_back<test_count> (), true);
        BOOST_CHECK_EQUAL (q.push_back<test_count_dec> (), true);
    }

    int sum = 0;
    for (auto& x : q) {
        x.method ();
        sum += x.count;
    }

    BOOST_CHECK_EQUAL (sum, 8);
    BOOST_CHECK_EQUAL (&*--q.end (), &q.back ());
    BOOST_CHECK_EQUAL (&*q.begin (), &q.front ());
}

BOOST_AUTO_TEST_CASE(hetero_deque_clear)
{
    test_deque q (1024);

    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_EQUAL (q.push_back<test_count> (), true);
    BOOST_CHECK_EQUAL (q.push_back<test_count_dec> (), true);
    BOOST_CHECK_EQUAL (q.empty (), false);
    q.clear ();
    BOOST_CHECK_EQUAL (q.empty (), true);
}

BOOST_AUTO_TEST_CASE(hetero_deque_no_tpl)
{
    test_deque q (1024);

    BOOST_CHECK_EQUAL (q.empty (), true);
    BOOST_CHECK_EQUAL (q.push_back (test_count ()), true);
    test_count_dec var1;
    BOOST_CHECK_EQUAL (q.push_back (var1), true);
    test_count_dec var2;
    BOOST_CHECK_EQUAL (q.push_front (var1), true);
    BOOST_CHECK_EQUAL (q.empty (), false);

    int sum = 0;
    for (auto& x : q) {
        x.method ();
        sum += x.count;
    }

    BOOST_CHECK_EQUAL (sum, -1);
}

BOOST_AUTO_TEST_SUITE_END()
