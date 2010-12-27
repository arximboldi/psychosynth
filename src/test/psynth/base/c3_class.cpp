/**
 *  Time-stamp:  <2010-12-27 21:33:23 raskolnikov>
 *
 *  @file        linearize_class.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Sat Dec 25 20:38:15 2010
 *
 *  Tests for psynth/base/linearize_class.hpp.
 *
 *  Most test heterarchies are taken from the original C3 paper:
 *  http://192.220.96.201/dylan/linearization-oopsla96.html
 */

/*
 *  Copyright (C) 2010 Juan Pedro Bolivar Puente
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
#include <psynth/base/c3_class.hpp>
#include <sstream>

namespace test
{

PSYNTH_C3_CLASS_BEGIN(object)
{
public:
    virtual void print (std::ostream& os) {}
};
PSYNTH_C3_CLASS_END(object)

#define PSYNTH_DEFINE_TEST_C3_CLASS(cls_name, ...)                      \
    PSYNTH_C3_CLASS_BEGIN(cls_name, __VA_ARGS__)                        \
    {                                                                   \
    public:                                                             \
        virtual void print (std::ostream& os)                           \
        {                                                               \
            os << ">" << #cls_name;                                     \
            this->Super::print (os);                                    \
        }                                                               \
    };                                                                  \
    PSYNTH_C3_CLASS_END(cls_name)

// Classes from the Dylan paper, figure 5
PSYNTH_DEFINE_TEST_C3_CLASS(pane, object);
PSYNTH_DEFINE_TEST_C3_CLASS(editing_mixin, object);
PSYNTH_DEFINE_TEST_C3_CLASS(editable_pane, pane, editing_mixin);
PSYNTH_DEFINE_TEST_C3_CLASS(scrolling_mixin, object);
PSYNTH_DEFINE_TEST_C3_CLASS(scrollable_pane, pane, scrolling_mixin);
PSYNTH_DEFINE_TEST_C3_CLASS(editable_scrollable_pane,
                            scrollable_pane, editable_pane);

// Classes from the Dylan paper, figure 4
PSYNTH_DEFINE_TEST_C3_CLASS(choice_widget, object);
PSYNTH_DEFINE_TEST_C3_CLASS(popup_mixin, object);
PSYNTH_DEFINE_TEST_C3_CLASS(menu, choice_widget);
PSYNTH_DEFINE_TEST_C3_CLASS(new_popup_menu, menu, popup_mixin, choice_widget);

// Classes from the Dylan paper, figure 2
PSYNTH_DEFINE_TEST_C3_CLASS(boat, object);
PSYNTH_DEFINE_TEST_C3_CLASS(day_boat, boat);
PSYNTH_DEFINE_TEST_C3_CLASS(wheel_boat, boat);
PSYNTH_DEFINE_TEST_C3_CLASS(engine_less, day_boat);
PSYNTH_DEFINE_TEST_C3_CLASS(small_multihull, day_boat);
PSYNTH_DEFINE_TEST_C3_CLASS(pedal_wheel_boat, engine_less, wheel_boat);
PSYNTH_DEFINE_TEST_C3_CLASS(small_catamaran, small_multihull);
PSYNTH_DEFINE_TEST_C3_CLASS(pedalo, pedal_wheel_boat, small_catamaran);

} /* namespace test */

BOOST_AUTO_TEST_SUITE(c3_class_test_suite)

BOOST_AUTO_TEST_CASE(c3_class_test_pane)
{
    std::ostringstream class_names;
    test::pane obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (), ">pane");
}

BOOST_AUTO_TEST_CASE(c3_class_test_editing_mixin)
{
    std::ostringstream class_names;
    test::editing_mixin obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (), ">editing_mixin");
}

BOOST_AUTO_TEST_CASE(c3_class_test_editable_pane)
{
    std::ostringstream class_names;
    test::editable_pane obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">editable_pane>pane>editing_mixin");
}

BOOST_AUTO_TEST_CASE(c3_class_test_scrolling_mixin)
{
    std::ostringstream class_names;
    test::scrolling_mixin obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (), ">scrolling_mixin");
}

BOOST_AUTO_TEST_CASE(c3_class_test_scrollable_pane)
{
    std::ostringstream class_names;
    test::scrollable_pane obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">scrollable_pane>pane>scrolling_mixin");
}

BOOST_AUTO_TEST_CASE(c3_class_test_editable_scrollable_pane)
{
    std::ostringstream class_names;
    test::editable_scrollable_pane obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">editable_scrollable_pane"
                      ">scrollable_pane"
                      ">editable_pane"
                      ">pane"
                      ">scrolling_mixin"
                      ">editing_mixin");
}

BOOST_AUTO_TEST_CASE(c3_class_test_new_popup_menu)
{
    std::ostringstream class_names;
    test::new_popup_menu obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">new_popup_menu"
                      ">menu"
                      ">popup_mixin"
                      ">choice_widget");
}

BOOST_AUTO_TEST_CASE(c3_class_test_pedal_wheel_boat)
{
    std::ostringstream class_names;
    test::pedal_wheel_boat obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">pedal_wheel_boat"
                      ">engine_less"
                      ">day_boat"
                      ">wheel_boat"
                      ">boat");
}

BOOST_AUTO_TEST_CASE(c3_class_test_small_catamaran)
{
    std::ostringstream class_names;
    test::small_catamaran obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">small_catamaran"
                      ">small_multihull"
                      ">day_boat"
                      ">boat");
}

BOOST_AUTO_TEST_CASE(c3_class_test_pedalo)
{
    std::ostringstream class_names;
    test::pedalo obj;
    obj.print (class_names);
    BOOST_CHECK_EQUAL(class_names.str (),
                      ">pedalo"
                      ">pedal_wheel_boat"
                      ">engine_less"
                      ">small_catamaran"
                      ">small_multihull"
                      ">day_boat"
                      ">wheel_boat"
                      ">boat");
}

BOOST_AUTO_TEST_SUITE_END()
