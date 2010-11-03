/**
 *  Time-stamp:  <2010-11-03 15:05:02 raskolnikov>
 *
 *  @file        sample.cpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov  3 10:49:09 2010
 *
 *  Unit tests for the sound sample module.
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

/*
 *  Copyright 2005-2007 Adobe Systems Incorporated
 * 
 *  Use, modification and distribution are subject to the Boost
 *  Software License, Version 1.0. (See accompanying file
 *  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 */

#include <exception>
#include <boost/test/unit_test.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/sample_algorithm.hpp>
#include <psynth/sound/concept.hpp>

using namespace psynth::sound;
using namespace std;

bits8   c8_min   =  sample_traits<bits8  >::min_value();
bits8   c8_max   =  sample_traits<bits8  >::max_value();
bits8s  c8s_min  =  sample_traits<bits8s >::min_value();
bits8s  c8s_max  =  sample_traits<bits8s >::max_value();
bits16  c16_min  =  sample_traits<bits16 >::min_value();
bits16  c16_max  =  sample_traits<bits16 >::max_value();
bits16s c16s_min =  sample_traits<bits16s>::min_value();
bits16s c16s_max =  sample_traits<bits16s>::max_value();
bits32  c32_min  =  sample_traits<bits32 >::min_value();
bits32  c32_max  =  sample_traits<bits32 >::max_value();
bits32s c32s_min =  sample_traits<bits32s>::min_value();
bits32s c32s_max =  sample_traits<bits32s>::max_value();
bits32f c32f_min =  sample_traits<bits32f>::min_value();
bits32f c32f_max =  sample_traits<bits32f>::max_value();

BOOST_AUTO_TEST_SUITE (sound_sample_test_suite);

template <typename SampleTestCore>
struct do_test : public SampleTestCore
{
    typedef typename SampleTestCore::sample_t sample_t;
    typedef typename sample_traits<sample_t>::value_type sample_value_t;

    do_test ()
	: SampleTestCore()
    {
        BOOST_CHECK (this->_min_v == sample_traits<sample_t>::min_value());
        BOOST_CHECK (this->_max_v == sample_traits<sample_t>::max_value());
    }

    void test_all ()
    {
        test_sample_invert ();
        test_sample_convert ();
        test_sample_multiply ();
        test_sample_math ();
    }

    void test_mutable (boost::mpl::false_) {}

    void test_mutable (boost::mpl::true_)
    {
        sample_value_t mv = this->_min_v;
        ++this->_min_v; this->_min_v++;
        --this->_min_v; this->_min_v--;
        BOOST_CHECK_EQUAL (mv, this->_min_v);

        this->_min_v += 1;
        this->_min_v -= 1;
        BOOST_CHECK_EQUAL (mv, this->_min_v);

        this->_min_v *= 1;
        this->_min_v /= 1;
        BOOST_CHECK_EQUAL (mv, this->_min_v);

	this->_min_v = 1;    // assignable to scalar
	this->_min_v = mv;   // and to value type
	
	// test swap
	sample_value_t v1 = this->_min_v;
	sample_value_t v2 = this->_max_v;
	swap (this->_min_v, this->_max_v);

	sample_value_t v3 = this->_min_v;
	sample_value_t v4 = this->_max_v;
	BOOST_CHECK_EQUAL (v1, v4);
	BOOST_CHECK_EQUAL (v2, v3);
    }

    void test_sample_math ()
    {
        BOOST_CHECK_LT (this->_min_v, this->_max_v);
        BOOST_CHECK_GT (this->_max_v, this->_min_v);
        BOOST_CHECK_LE (this->_min_v, this->_max_v);
        BOOST_CHECK_GE (this->_max_v, this->_min_v);
        BOOST_CHECK_NE (this->_max_v, this->_min_v);
        BOOST_CHECK (this->_max_v != this->_min_v);

        BOOST_CHECK_EQUAL (this->_min_v * 1, this->_min_v);
        BOOST_CHECK_EQUAL (this->_min_v / 1, this->_min_v);

        BOOST_CHECK_EQUAL ((this->_min_v + 1) + 1, (this->_min_v + 2));
        BOOST_CHECK_EQUAL ((this->_max_v - 1) - 1, (this->_max_v - 2));

	#if 0
	// TODO: Something similar was in the original GIL unit tests
	// but I do no fucking understand this.
	BOOST_CHECK_NE (this->_min_v, 1);
	BOOST_CHECK_EQUAL (this->_min_v, 1);
	// comparable to integral
	#endif
	
        test_mutable (boost::mpl::bool_<sample_traits<sample_t>::is_mutable>());
    }

    void test_sample_invert ()
    {
        BOOST_CHECK_EQUAL (sample_invert (this->_min_v), this->_max_v);
        BOOST_CHECK_EQUAL (sample_invert (this->_max_v), this->_min_v);
    }

    void test_sample_multiply ()
    {
        BOOST_CHECK_EQUAL (sample_multiply (this->_min_v, this->_min_v),
			   this->_min_v);
        BOOST_CHECK_EQUAL (sample_multiply (this->_max_v, this->_max_v),
			   this->_max_v);
        BOOST_CHECK_EQUAL (sample_multiply (this->_max_v, this->_min_v),
			   this->_min_v);
    }

    void test_sample_convert ()
    {
        sample_value_t  v_min, v_max;

        v_min = sample_convert<sample_t>(c8_min);
        v_max = sample_convert<sample_t>(c8_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);

        v_min = sample_convert<sample_t>(c8s_min);
        v_max = sample_convert<sample_t>(c8s_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);

        v_min = sample_convert<sample_t>(c16_min);
        v_max = sample_convert<sample_t>(c16_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);

        v_min = sample_convert<sample_t>(c16s_min);
        v_max = sample_convert<sample_t>(c16s_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);

        v_min = sample_convert<sample_t>(c32_min);
        v_max = sample_convert<sample_t>(c32_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);

        v_min = sample_convert<sample_t>(c32s_min);
        v_max = sample_convert<sample_t>(c32s_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);

        v_min = sample_convert<sample_t>(c32f_min);
        v_max = sample_convert<sample_t>(c32f_max);
        BOOST_CHECK_EQUAL (v_min, this->_min_v);
	BOOST_CHECK_EQUAL (v_max, this->_max_v);
    }
};

/*
 * Different core classes depending on the different types of samples
 * - sample values, references and subbyte references The cores ensure
 * there are two members, _min_v and _max_v initialized with the
 * minimum and maximum sample value.  The different sample types have
 * different ways to initialize them, thus require different cores
 */

// For sample values simply initialize the value directly
template <typename SampleValue>
class value_core
{
protected:
    typedef SampleValue sample_t;
    sample_t _min_v, _max_v;

    value_core()
	: _min_v (sample_traits<SampleValue>::min_value())
	, _max_v (sample_traits<SampleValue>::max_value())
    {
        boost::function_requires<SampleValueConcept<SampleValue> >();
    }
};

// For sample references we need to have separate sample values
template <typename SampleRef>
class reference_core : public value_core<
    typename sample_traits<SampleRef>::value_type>
{
    typedef value_core<typename sample_traits<SampleRef>::value_type> parent_t;

protected:
    typedef SampleRef sample_t;
    sample_t _min_v, _max_v;

    reference_core()
	: parent_t ()
	, _min_v (parent_t::_min_v)
	, _max_v (parent_t::_max_v)
    {
        boost::function_requires<SampleConcept<SampleRef> >();
    }
};

// For subbyte sample references we need to store the bit buffers somewhere
template <typename SampleSubbyteRef,
	  typename SampleMutableRef = SampleSubbyteRef>
class packed_reference_core
{
protected:
    typedef SampleSubbyteRef sample_t;
    typedef typename sample_t::integer_t integer_t;
    sample_t _min_v, _max_v;

    integer_t _min_buf, _max_buf;

    packed_reference_core ()
	: _min_v(&_min_buf)
	, _max_v(&_max_buf)
    {
        SampleMutableRef b1(&_min_buf), b2(&_max_buf);
        b1 = sample_traits<sample_t>::min_value();
        b2 = sample_traits<sample_t>::max_value();

        boost::function_requires<SampleConcept<SampleSubbyteRef> >();
    }
};

template <typename SampleSubbyteRef,
	  typename SampleMutableRef = SampleSubbyteRef>
class packed_dynamic_reference_core
{
protected:
    typedef SampleSubbyteRef sample_t;
    sample_t _min_v, _max_v;

    typename sample_t::integer_t _min_buf, _max_buf;

    packed_dynamic_reference_core (int first_bit1=1, int first_bit2=2)
	: _min_v (&_min_buf, first_bit1)
	, _max_v (&_max_buf, first_bit2)
    {
        SampleMutableRef b1(&_min_buf,1), b2(&_max_buf,2);
        b1 = sample_traits<sample_t>::min_value();
        b2 = sample_traits<sample_t>::max_value();

        boost::function_requires<SampleConcept<SampleSubbyteRef> >();
    }
};

template <typename SampleValue>
void test_sample_value()
{ 
    do_test<value_core<SampleValue> >().test_all();
}

template <typename SampleRef>
void test_sample_reference()
{ 
    do_test<reference_core<SampleRef> >().test_all();
}

template <typename SampleSubbyteRef>
void test_packed_sample_reference()
{
    do_test<packed_reference_core<
	SampleSubbyteRef,SampleSubbyteRef> >().test_all();
}

template <typename SampleSubbyteRef, typename MutableRef>
void test_const_packed_sample_reference ()
{
    do_test<packed_reference_core<SampleSubbyteRef,MutableRef> >().test_all();
}

template <typename SampleSubbyteRef>
void test_packed_dynamic_sample_reference()
{
    do_test<packed_dynamic_reference_core<
	SampleSubbyteRef,SampleSubbyteRef> >().test_all();
}

template <typename SampleSubbyteRef, typename MutableRef>
void test_const_packed_dynamic_sample_reference()
{
    do_test<packed_dynamic_reference_core<
	SampleSubbyteRef,MutableRef> >().test_all();
}

template <typename SampleValue>
void test_sample_value_impl ()
{
    test_sample_value<SampleValue>();
    test_sample_reference<SampleValue&>();
    test_sample_reference<const SampleValue&>();
}


/*
 *
 *   A sample archetype
 *   - to test the minimum requirements of the concept
 *
 */

struct sample_value_archetype;

struct sample_archetype
{
    // equality comparable
    friend bool operator== (const sample_archetype&,const sample_archetype&)
    { return true; }

    friend bool operator!= (const sample_archetype&,const sample_archetype&)
    { return false; }

    // less-than comparable
    friend bool operator<(const sample_archetype&,const sample_archetype&)
    { return false; }

    // convertible to a scalar
    operator bits8() const { return 0; }

    sample_archetype& operator++() { return *this; }
    sample_archetype& operator--() { return *this; }
    sample_archetype  operator++(int) { return *this; }
    sample_archetype  operator--(int) { return *this; }
    
    template <typename Scalar>
    sample_archetype operator+=(Scalar) { return *this; }
    template <typename Scalar>
    sample_archetype operator-=(Scalar) { return *this; }
    template <typename Scalar>
    sample_archetype operator*=(Scalar) { return *this; }
    template <typename Scalar>
    sample_archetype operator/=(Scalar) { return *this; }

    typedef sample_value_archetype         value_type;
    typedef sample_archetype               reference;
    typedef const sample_archetype         const_reference;
    typedef sample_value_archetype*        pointer;
    typedef const sample_value_archetype*  const_pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable=true);

    static value_type min_value ();
    static value_type max_value ();
    static value_type zero_value ();
};

struct sample_value_archetype : public sample_archetype
{
    sample_value_archetype() {}
    // default constructible
    sample_value_archetype(const sample_value_archetype&) {}
    // copy constructible
    sample_value_archetype& operator = (const sample_value_archetype&)
    { return *this; } // assignable
    sample_value_archetype(bits8) {}
};

sample_value_archetype sample_archetype::min_value ()
{ return sample_value_archetype(); }
sample_value_archetype sample_archetype::max_value ()
{ return sample_value_archetype(); }
sample_value_archetype sample_archetype::zero_value ()
{ return sample_value_archetype(); }

BOOST_AUTO_TEST_CASE (test_packed_sample_reference_0)
{
    typedef packed_sample_reference<boost::uint16_t, 0,5,true>
	sample16_0_5_reference_t;
    typedef packed_sample_reference<boost::uint16_t, 5,6,true>
	sample16_5_6_reference_t;
    typedef packed_sample_reference<boost::uint16_t, 11,5,true>
	sample16_11_5_reference_t;

    boost::uint16_t data=0;
    sample16_0_5_reference_t   sample1(&data);
    sample16_5_6_reference_t   sample2(&data);
    sample16_11_5_reference_t  sample3(&data);

    sample1=sample_traits<sample16_0_5_reference_t>::max_value();
    sample2=sample_traits<sample16_5_6_reference_t>::max_value();
    sample3=sample_traits<sample16_11_5_reference_t>::max_value();
    BOOST_CHECK_EQUAL (data, 65535);

    test_packed_sample_reference<sample16_0_5_reference_t>();
    test_packed_sample_reference<sample16_5_6_reference_t>();
    test_packed_sample_reference<sample16_11_5_reference_t>();
}

BOOST_AUTO_TEST_CASE (test_packed_dynamic_sample_reference_0)
{
    typedef packed_dynamic_sample_reference<boost::uint16_t,5,true>
	sample16_5_reference_t;
    typedef packed_dynamic_sample_reference<boost::uint16_t,6,true>
	sample16_6_reference_t;

    boost::uint16_t data=0;
    sample16_5_reference_t  sample1(&data,0);
    sample16_6_reference_t  sample2(&data,5);
    sample16_5_reference_t  sample3(&data,11);

    sample1=sample_traits<sample16_5_reference_t>::max_value();
    sample2=sample_traits<sample16_6_reference_t>::max_value();
    sample3=sample_traits<sample16_5_reference_t>::max_value();
    BOOST_CHECK_EQUAL (data, 65535);

    test_packed_dynamic_sample_reference<sample16_5_reference_t>();
}

BOOST_AUTO_TEST_CASE (test_sample)
{
    test_sample_value_impl<bits8>();
    test_sample_value_impl<bits8s>();
    test_sample_value_impl<bits16>();
    test_sample_value_impl<bits16s>();
    test_sample_value_impl<bits32>();
    test_sample_value_impl<bits32s>();

    test_sample_value_impl<bits32f>();

    // Do only compile-time tests for the archetype (because asserts
    // like val1<val2 fail)
    boost::function_requires<MutableSampleConcept<sample_archetype> >();

    do_test<value_core<sample_value_archetype> >();
    do_test<reference_core<sample_archetype> >();
    do_test<reference_core<const sample_archetype&> >();
}

BOOST_AUTO_TEST_SUITE_END ()

/*
 *  TODO: 
 *  - provide algorithm performance overloads for scoped sample and
 *    packed samples
 *  - Update concepts and documentation
 *  - What to do about pointer types?!
 *  - Performance!!
 *     - is sample_convert the same as native?
 *     - is operator++ on bits32f the same as native? How about if
 *       operator++ is defined in scoped_sample to do _value++?
 */
