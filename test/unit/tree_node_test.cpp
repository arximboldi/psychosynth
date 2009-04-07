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

#include <boost/test/unit_test.hpp>
#include <psynth/base/tree.hpp>

template <class T>
struct value_node : public psynth::tree_node<value_node<T> >,
		    public PSYNTH_DEFAULT_THREADING <value_node<T> >
{
    T value;
};

template <>
struct value_node<int> : public psynth::tree_node<value_node<int> >
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
	root.get_path ("1.2.3.4").value = -1;
	root.get_path ("...").value = -1;
    }

    ~tree_node_fixture () {}
};

BOOST_FIXTURE_TEST_SUITE(tree_node_test_suite_0, tree_node_fixture)

BOOST_AUTO_TEST_CASE(tree_node_test_0)
{
    BOOST_CHECK_EQUAL (root.get_child ("1").value, 0);
    BOOST_CHECK_EQUAL (root.get_child ("").value, 0);
    BOOST_CHECK_EQUAL (root.get_child ("").get_child ("")
			.get_child ("").get_child ("").value, -1);
    BOOST_CHECK_EQUAL (root.get_child ("1").get_child ("2")
			.get_child ("3").get_child ("4").value, -1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_1)
{
    root.get_child ("1").value = 1;
    root.get_child ("2").value = 2;
    root.get_child ("").get_child ("")
	.get_child ("").get_child ("").value = 1;

    BOOST_CHECK_EQUAL (root.get_child ("1").value, 1);
    BOOST_CHECK_EQUAL (root.get_child ("2").value, 2);
    BOOST_CHECK_EQUAL (root.get_child ("").get_child ("")
			.get_child ("").get_child ("").value, 1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_2)
{
    BOOST_CHECK_EQUAL (root.get_path ("1").value, 0);
    BOOST_CHECK_EQUAL (root.get_path ("").value, 0);
    BOOST_CHECK_EQUAL (root.get_path ("...").value, -1);
    BOOST_CHECK_EQUAL (root.get_path ("1.2.3.4").value, -1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_3)
{
    root.get_path ("1").value = 1;
    root.get_path ("2").value = 2;
    root.get_path ("...").value = 1;

    BOOST_CHECK_EQUAL (root.get_path ("1").value, 1);
    BOOST_CHECK_EQUAL (root.get_path ("2").value, 2);
    BOOST_CHECK_EQUAL (root.get_path ("...").value, 1);
}

BOOST_AUTO_TEST_CASE(tree_node_test_4)
{
    BOOST_CHECK_EQUAL (root.get_path ("...").get_path_name (), "...");
    BOOST_CHECK_EQUAL (root.get_child ("").get_child("").get_path_name (), ".");
}

BOOST_AUTO_TEST_CASE(tree_node_test_5)
{
    BOOST_CHECK_EQUAL (root.get_path ("...").get_path_name (), "...");
    BOOST_CHECK_EQUAL (root.get_child ("").get_child("").get_path_name (), ".");
}

BOOST_AUTO_TEST_CASE(tree_node_test_7)
{
    BOOST_CHECK_THROW (root.get_existing_child ("node"), psynth::tree_node_error);
    BOOST_CHECK_NO_THROW (root.get_existing_child ("1"));

    BOOST_CHECK_THROW (root.get_existing_path ("...."), psynth::tree_node_error);
    BOOST_CHECK_THROW (root.get_existing_path ("1.2.3.4.5"), psynth::tree_node_error);
    BOOST_CHECK_NO_THROW (root.get_existing_path ("1.2.3.4"));
    BOOST_CHECK_NO_THROW (root.get_existing_path ("..."));
}

BOOST_AUTO_TEST_CASE(tree_node_test_6)
{   
    value_node<int>& ref1 = root.detach (std::string ("1"));
    BOOST_CHECK_EQUAL (ref1.get_path ("2.3.4").value, -1);
    BOOST_CHECK_EQUAL (ref1.get_name (), "");
    BOOST_CHECK_THROW (root.get_existing_path ("1.2.3.4"),
		       psynth::tree_node_error);
    
    BOOST_CHECK_EQUAL (root.attach ("1", ref1), true);
    BOOST_CHECK_NO_THROW (root.get_existing_path ("1.2.3.4"));
    BOOST_CHECK_EQUAL (root.get_path ("1.2.3.4").value, -1);

    value_node<int> node;
    node.value = 1;
    BOOST_CHECK_EQUAL (root.get_path ("1.2.3").attach ("5", node), true);
    BOOST_CHECK_EQUAL (root.get_path ("1.2.3.5").value, 1);
    BOOST_CHECK_THROW (root.get_path ("1.2.3").detach (std::string ("")),
		       psynth::tree_node_error);
    BOOST_CHECK_NO_THROW (root.get_path ("1.2.3").detach (std::string ("5")));
	
    value_node<int>& ref2 = root.get_path ("1.2.3").detach (std::string ("4"));
    BOOST_CHECK_EQUAL (ref2.value, -1);
    delete &ref2;
}

BOOST_AUTO_TEST_SUITE_END()
