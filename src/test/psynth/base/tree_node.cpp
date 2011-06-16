/**
 *  Time-stamp:  <2011-06-13 15:52:09 raskolnikov>
 *
 *  @file        tree_node_test.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2009.
 *
 *  tree_node unit tests.
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

#include <iostream>
#include <boost/test/unit_test.hpp>
#include <psynth/base/tree.tpp>

template <class T>
struct value_node : public psynth::base::tree_node<value_node<T> >,
		    public PSYNTH_DEFAULT_THREADING <value_node<T> >
{
    T value;
};

template <>
struct value_node<int> : public psynth::base::tree_node<value_node<int> >
		       , public PSYNTH_DEFAULT_THREADING <value_node<int> >
{
    int value;

    value_node () : value (0) {}
};

struct tree_node_fixture
{
    value_node<int> root;
    
    tree_node_fixture ()
    {
	root.path ("1.2.3.4").value = -1;
	root.path ("...").value = -1;
    }

    ~tree_node_fixture () {}
};

BOOST_FIXTURE_TEST_SUITE(tree_node_test_suite, tree_node_fixture)

BOOST_AUTO_TEST_CASE(tree_node_test_0)
{
    BOOST_CHECK_EQUAL (root.child ("1").value, 0);
    BOOST_CHECK_EQUAL (root.child ("").value, 0);
    BOOST_CHECK_EQUAL (root.child ("").child ("")
			.child ("").child ("").value, -1);
    BOOST_CHECK_EQUAL (root.child ("1").child ("2")
			.child ("3").child ("4").value, -1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_1)
{
    root.child ("1").value = 1;
    root.child ("2").value = 2;
    root.child ("").child ("")
	.child ("").child ("").value = 1;

    BOOST_CHECK_EQUAL (root.child ("1").value, 1);
    BOOST_CHECK_EQUAL (root.child ("2").value, 2);
    BOOST_CHECK_EQUAL (root.child ("").child ("")
			.child ("").child ("").value, 1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_2)
{
    BOOST_CHECK_EQUAL (root.path ("1").value, 0);
    BOOST_CHECK_EQUAL (root.path ("").value, 0);
    BOOST_CHECK_EQUAL (root.path ("...").value, -1);
    BOOST_CHECK_EQUAL (root.path ("1.2.3.4").value, -1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_3)
{
    root.path ("1").value = 1;
    root.path ("2").value = 2;
    root.path ("...").value = 1;

    BOOST_CHECK_EQUAL (root.path ("1").value, 1);
    BOOST_CHECK_EQUAL (root.path ("2").value, 2);
    BOOST_CHECK_EQUAL (root.path ("...").value, 1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_4)
{
    BOOST_CHECK_EQUAL (root.path ("...").path_name (), "...");
    BOOST_CHECK_EQUAL (root.child ("").child("").path_name (), ".");
}

BOOST_AUTO_TEST_CASE(tree_node_test_5)
{
    BOOST_CHECK_EQUAL (root.path ("...").path_name (), "...");
    BOOST_CHECK_EQUAL (root.child ("").child("").path_name (), ".");
}

BOOST_AUTO_TEST_CASE(tree_node_test_7)
{
    BOOST_CHECK_THROW (root.existing_child ("node"),
		       psynth::base::tree_node_error);
    BOOST_CHECK_NO_THROW (root.existing_child ("1"));

    BOOST_CHECK_THROW (root.existing_path ("...."),
		       psynth::base::tree_node_error);
    BOOST_CHECK_THROW (root.existing_path ("1.2.3.4.5"),
		       psynth::base::tree_node_error);
    BOOST_CHECK_NO_THROW (root.existing_path ("1.2.3.4"));
    BOOST_CHECK_NO_THROW (root.existing_path ("..."));
}

BOOST_AUTO_TEST_CASE(tree_node_test_6)
{
    auto ptr1 = root.detach (std::string ("1"));
    BOOST_CHECK_EQUAL (ptr1->path ("2.3.4").value, -1);
        
    BOOST_CHECK_EQUAL (ptr1->name (), "");
    BOOST_CHECK_THROW (root.existing_path ("1.2.3.4"),
		       psynth::base::tree_node_error);
    
    BOOST_CHECK_EQUAL (root.attach ("1", std::move (ptr1)), true);
    BOOST_CHECK_NO_THROW (root.existing_path ("1.2.3.4"));
    BOOST_CHECK_EQUAL (root.path ("1.2.3.4").value, -1);

    value_node<int> node;
    node.value = 1;
    BOOST_CHECK_EQUAL (root.path ("1.2.3").attach (
                           "5", std::unique_ptr<value_node<int>> (&node)),
                       true);
    BOOST_CHECK_EQUAL (root.path ("1.2.3.5").value, 1);
    BOOST_CHECK_THROW (root.path ("1.2.3").detach (std::string ("")),
		       psynth::base::tree_node_error);
    BOOST_CHECK_NO_THROW (root.path ("1.2.3").detach (std::string ("5")).release ());
	
    auto ptr2 = root.path ("1.2.3").detach (std::string ("4"));
    BOOST_CHECK_EQUAL (ptr2->value, -1);
}

BOOST_AUTO_TEST_SUITE_END()
