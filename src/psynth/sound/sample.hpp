/**
 *  Time-stamp:  <2010-11-05 13:39:47 raskolnikov>
 *
 *  @file        sample.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Oct 25 19:26:09 2010
 *
 *  Sample type traits and metafunctions.
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

#ifndef PSYNTH_SAMPLE_HPP_
#define PSYNTH_SAMPLE_HPP_

#include <limits>
#include <cassert>
#include <boost/cstdint.hpp>

#include <psynth/base/compat.hpp>
#include <psynth/sound/util.hpp>

namespace psynth
{
namespace sound
{

/**
  sample_traits
  
  \ingroup SampleModel
  \class sample_traits
  \brief defines properties of samples, such as their range and
  associated types

  The sample traits must be defined for every model of SampleConcept
  Default traits are provided. For built-in types the default traits
  use built-in pointer and reference and the sample range is the
  physical range of the type. For classes, the default traits forward
  the associated types and range to the class.
*/

namespace detail
{
template <typename T, bool is_class> struct sample_traits_impl;

/* sample traits for custom class */
template <typename T> 
struct sample_traits_impl<T, true>
{
    typedef typename T::value_type      value_type;
    typedef typename T::reference       reference;
    typedef typename T::pointer         pointer;
    typedef typename T::const_reference const_reference;
    typedef typename T::const_pointer   const_pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable = T::is_mutable);
    static value_type min_value () { return T::min_value(); }
    static value_type max_value () { return T::max_value(); }
    static value_type zero_value () { return T::zero_value(); }
};

/* sample traits implementation for built-in integral or floating
   point sample type */
template <typename T> 
struct sample_traits_impl<T, false>
{
    typedef T           value_type;
    typedef T&          reference;
    typedef T*          pointer;
    typedef const T&    const_reference;
    typedef T const*    const_pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable=true);
    static value_type min_value () { return (std::numeric_limits<T>::min) (); }
    static value_type max_value () { return (std::numeric_limits<T>::max) (); }
    static value_type zero_value () { return T(0); }
};

/* sample traits implementation for constant built-in scalar or
   floating point type */
template <typename T> 
struct sample_traits_impl<const T, false> :
	public sample_traits_impl<T, false>
{
        typedef const T&    reference;
        typedef const T*    pointer;
        BOOST_STATIC_CONSTANT(bool, is_mutable = false);
};

} /* namespace detail */

/**
   \ingroup SampleModel
   \brief Traits for samples. Contains the following members:

   \code
template <typename Sample>
struct sample_traits {
    typedef ... value_type;
    typedef ... reference;
    typedef ... pointer;
    typedef ... const_reference;
    typedef ... const_pointer;
    
    static const bool is_mutable;
    static value_type min_value();
    static value_type max_value();
};
\endcode

*/
template <typename T>
struct sample_traits :
	public detail::sample_traits_impl<T, boost::is_class<T>::value> {};

// Sample traits for C++ reference type - remove the reference
template <typename T>
struct sample_traits<T&> : public sample_traits<T> {};

// Sample traits for constant C++ reference type
template <typename T> struct sample_traits<const T&> : public sample_traits<T>
{
    typedef typename sample_traits<T>::const_reference reference;
    typedef typename sample_traits<T>::const_pointer   pointer;
    BOOST_STATIC_CONSTANT(bool, is_mutable = false);
};


/*
 *
 *    scoped_sample_value
 *
 */

/**
   \defgroup ScopedSampleValue scoped_sample_value
   \ingroup SampleModel
   \brief A sample adaptor that modifies the range of the source
   sample. Models: SampleValueConcept

Example:
\code
// Create a double sample with range [-0.5 .. 0.5]
struct double_minus_half  { static double apply() { return -0.5; } };
struct double_plus_half   { static double apply() { return  0.5; } };
typedef scoped_sample_value<double, double_minus_half, double_plus_half> bits64custom_t;

// sample_convert its maximum should map to the maximum
bits64custom_t x = sample_traits<bits64custom_t>::max_value();
assert(x == 0.5);
bits16 y = sample_convert<bits16>(x);
assert(y == 65535);
\endcode
*/

/**
   \ingroup ScopedSampleValue
   \brief A sample adaptor that modifies the range of the source
   sample. Models: SampleValueConcept
*/
template <typename BaseSampleValue,
	  // base sample (models SampleValueConcept)
          typename MinVal,
	  typename MaxVal,
	  typename ZeroVal
	  // classes with a static apply() function returning the
	  // minimum/maximum sample values
	  >
struct scoped_sample_value
{
    typedef scoped_sample_value    value_type;
    typedef value_type&            reference;
    typedef value_type*            pointer;
    typedef const value_type&      const_reference;
    typedef const value_type*      const_pointer;
    
    BOOST_STATIC_CONSTANT(bool, is_mutable =
			  sample_traits<BaseSampleValue>::is_mutable);

    typedef BaseSampleValue base_sample;

    static value_type min_value () { return MinVal::apply (); }
    static value_type max_value () { return MaxVal::apply (); }
    static value_type zero_value () { return ZeroVal::apply (); }

    scoped_sample_value () {}
    scoped_sample_value (const scoped_sample_value& c) : _value(c._value) {}
    scoped_sample_value (BaseSampleValue val) : _value(val) {}

    scoped_sample_value& operator++ () { ++_value; return *this; }
    scoped_sample_value& operator-- () { --_value; return *this; }

    scoped_sample_value operator ++ (int)
    {
	scoped_sample_value tmp = *this;
	this->operator++ ();
	return tmp;
    }
    
    scoped_sample_value operator--(int)
    {
	scoped_sample_value tmp = *this;
	this->operator-- ();
	return tmp;
    }

    template <typename Scalar2>
    scoped_sample_value& operator+=(Scalar2 v) { _value+=v; return *this; }
    template <typename Scalar2>
    scoped_sample_value& operator-=(Scalar2 v) { _value-=v; return *this; }
    template <typename Scalar2>
    scoped_sample_value& operator*=(Scalar2 v) { _value*=v; return *this; }
    template <typename Scalar2>
    scoped_sample_value& operator/=(Scalar2 v) { _value/=v; return *this; }

    scoped_sample_value& operator= (BaseSampleValue v)
    { _value=v; return *this; }

    operator BaseSampleValue () const { return _value; }

private:
    BaseSampleValue _value;
};

struct float_zero { static float apply () { return 0.0f; } };
struct float_one  { static float apply () { return 1.0f; } };
struct float_minus_one  { static float apply () { return -1.0f; } };
struct float_half  { static float apply () { return 0.5f; } };



/*
 *
 *  Support for sub-byte samples. These are integral samples whose
 *  value is contained in a range of bits inside an integral type
 *
 */

/*
  It is necessary for packed samples to have their own value
  type. They cannot simply use an integral large enough to store the
  data. Here is why:

  - Any operation that requires returning the result by value will
    otherwise return the built-in integral type, which will have
    incorrect range That means that after getting the value of the
    sample we cannot properly do sample_convert, sample_invert, etc.

  - Two samples are declared compatible if they have the same value
    type. That means that a packed sample is incorrectly declared
    compatible with an integral type
*/
namespace detail
{
/* returns the smallest fast unsigned integral type that has at
   least NumBits bits */
template <int NumBits>
struct min_fast_uint :
	public boost::mpl::if_c<
    (NumBits<=8), 
	uint_least8_t, 
	typename boost::mpl::if_c<
	(NumBits<=16), 
	    uint_least16_t, 
	    typename boost::mpl::if_c<
	    (NumBits<=32), 
		uint_least32_t, 
		uintmax_t
		>::type
	    >::type
	    > {};
}

/**
   \defgroup PackedSampleValueModel packed_sample_value
   \ingroup SampleModel
   \brief Represents the value of an unsigned integral sample
   operating over a bit range. Models: SampleValueConcept
   
   Example:
   \code
   // A 4-bit unsigned integral sample.
   typedef packed_sample_value<4> bits4;
   
   assert(sample_traits<bits4>::min_value()==0);
   assert(sample_traits<bits4>::max_value()==15);
   assert(sizeof(bits4)==1);
   BOOST_STATIC_ASSERT((boost::is_integral<bits4>::value));
   \endcode
*/

/**
   \ingroup PackedSampleValueModel
   \brief The value of a subbyte sample. Models: SampleValueConcept
*/
template <int NumBits>
class packed_sample_value
{
    static const std::size_t num_values = 1 << NumBits;

public:
    typedef typename detail::min_fast_uint<NumBits>::type integer_t;

    typedef packed_sample_value   value_type;
    typedef value_type&           reference;
    typedef const value_type&     const_reference;
    typedef value_type*           pointer;
    typedef const value_type*     const_pointer;

    static value_type min_value () { return value_type (0); }
    static value_type zero_value () { return min_value () / 2 + max_value () / 2; }
    static value_type max_value () { return value_type (num_values - 1); }
    BOOST_STATIC_CONSTANT(bool, is_mutable = true);

    packed_sample_value () {}
    packed_sample_value (integer_t v) : _value(v % num_values) {}
    packed_sample_value (const packed_sample_value& v) : _value (v._value) {}
    template <typename Scalar>
    packed_sample_value (Scalar v) : _value (integer_t (v) % num_values) {}
    // suppress GCC implicit conversion warnings in sample regression file 

    operator integer_t () const { return _value; }

private:
    integer_t _value;
};

namespace detail
{

template <std::size_t K>
struct static_copy_bytes
{
    void operator () (const unsigned char* from, unsigned char* to) const
    {
        *to = *from;
        static_copy_bytes<K-1> () (++from, ++to);
    }
};

template <>
struct static_copy_bytes<0>
{
    void operator () (const unsigned char* , unsigned char*) const {}
};

template <typename Derived, typename BitField, int NumBits, bool Mutable>
class packed_sample_reference_base
{
protected:
    typedef typename boost::mpl::if_c<Mutable, void*, const void*>::type data_ptr_t;

public:
    typedef packed_sample_value<NumBits>   value_type;
    typedef const Derived                  reference;
    typedef value_type*                    pointer;
    typedef const value_type*              const_pointer;
    BOOST_STATIC_CONSTANT(int,  num_bits = NumBits);
    BOOST_STATIC_CONSTANT(bool, is_mutable = Mutable);

    static value_type min_value ()
    {
	return sample_traits<value_type>::min_value();
    }
    
    static value_type max_value ()
    {
	return sample_traits<value_type>::max_value();
    }

    static value_type zero_value ()
    {
	return sample_traits<value_type>::zero_value();
    }

    typedef BitField                       bitfield_t;
    typedef typename value_type::integer_t integer_t;

    packed_sample_reference_base (data_ptr_t data_ptr)
	: _data_ptr(data_ptr) {}
    packed_sample_reference_base (const packed_sample_reference_base& ref)
	: _data_ptr(ref._data_ptr) {}

    const Derived& operator= (integer_t v) const { set (v); return derived (); }
    const Derived& operator++ () const { set(get()+1); return derived (); }
    const Derived& operator-- () const { set(get()-1); return derived (); }

    Derived operator++(int) const
    {
	Derived tmp=derived();
	this->operator++();
	return tmp;
    }
    
    Derived operator--(int) const
    {
	Derived tmp=derived();
	this->operator--();
	return tmp;
    }

    template <typename Scalar2>
    const Derived& operator+= (Scalar2 v) const { set(get()+v); return derived(); }
    template <typename Scalar2>
    const Derived& operator-= (Scalar2 v) const { set(get()-v); return derived(); }
    template <typename Scalar2>
    const Derived& operator*= (Scalar2 v) const { set(get()*v); return derived(); }
    template <typename Scalar2>
    const Derived& operator/= (Scalar2 v) const { set(get()/v); return derived(); }

    operator integer_t () const { return get (); }
    data_ptr_t operator & () const {return _data_ptr;}

protected:
    data_ptr_t _data_ptr;
    // void* pointer to the first byte of the bit range
    static const integer_t max_val = (1 << NumBits) - 1;

#ifdef PSYNTH_NONWORD_POINTER_ALIGNMENT_SUPPORTED
    const bitfield_t& get_data () const
    {
	return *static_cast<const bitfield_t*> (_data_ptr);
    }
    
    void set_data(const bitfield_t& val) const
    {
	*static_cast<bitfield_t*> (_data_ptr) = val;
    }
#else
    bitfield_t get_data() const
    {
        bitfield_t ret;
        static_copy_bytes<sizeof(bitfield_t)> ()(
	    psynth_reinterpret_cast_c<const unsigned char*>(_data_ptr),
	    psynth_reinterpret_cast<unsigned char*>(&ret));
        return ret;
    }
    
    void set_data(const bitfield_t& val) const
    {
        static_copy_bytes<sizeof(bitfield_t)> ()(
	    psynth_reinterpret_cast_c<const unsigned char*>(&val),
	    psynth_reinterpret_cast<unsigned char*>(_data_ptr));
    }
#endif

private:
    void set(integer_t value) const
    {
	// TODO: can this be done faster??
        const integer_t num_values = max_val+1;
        this->derived ().set_unsafe (
	    ((value % num_values) + num_values) % num_values); 
    }
    
    integer_t get () const { return derived ().get (); }

    const Derived& derived() const
    {
	return static_cast<const Derived&>(*this);
    }
};

} /* namespace detail */

/**
   \defgroup PackedSampleReferenceModel packed_sample_reference
   \ingroup SampleModel
   \brief Represents a reference proxy to a sample operating over a
   bit range whose offset is fixed at compile time. Models
   SampleConcept
   
   Example:
   \code
   // Reference to a 2-bit sample starting at bit 1 (i.e. the second bit)
   typedef const packed_sample_reference<uint16_t,1,2,true> bits2_1_ref_t;

   uint16_t data=0;
   bits2_1_ref_t sample_ref(&data);
   sample_ref = sample_traits<bits2_1_ref_t>::max_value();   // == 3
   assert(data == 6);                                          // == 3<<1 == 6
   \endcode
*/

template <typename BitField,
	  // A type that holds the bits of the pixel from which the
	  // sample is referenced. Typically an integral type, like
	  // boost::uint16_t
          int FirstBit, int NumBits,
	  // Defines the sequence of bits in the data value that
	  // contain the sample
          bool Mutable
	  // true if the reference is mutable
	  >
class packed_sample_reference;

template <typename BitField,
	  // A type that holds the bits of the pixel from which the
	  // sample is referenced. Typically an integral type, like
	  // boost::uint16_t
          int NumBits,
	  // Defines the sequence of bits in the data value that
	  // contain the sample
          bool Mutable
	  // true if the reference is mutable
	  >
class packed_dynamic_sample_reference;

/**
   \ingroup PackedSampleReferenceModel
   \brief A constant subbyte sample reference whose bit offset is
   fixed at compile time. Models SampleConcept
*/
template <typename BitField, int FirstBit, int NumBits>
class packed_sample_reference<BitField, FirstBit, NumBits, false> :
    public detail::packed_sample_reference_base<
    packed_sample_reference<BitField, FirstBit, NumBits, false>,
    BitField, NumBits, false>
{
    typedef detail::packed_sample_reference_base<
	packed_sample_reference<BitField, FirstBit, NumBits, false>,
	BitField,NumBits,false> parent_t;
    friend class packed_sample_reference<BitField, FirstBit, NumBits, true>;

    static const BitField sample_mask =
	static_cast<BitField>(parent_t::max_val) << FirstBit;

    void operator= (const packed_sample_reference&);

public:
    typedef const packed_sample_reference<BitField,FirstBit,NumBits,false>
    const_reference;
    typedef const packed_sample_reference<BitField,FirstBit,NumBits,true>
    mutable_reference;
    typedef typename parent_t::integer_t integer_t;

    explicit packed_sample_reference (const void* data_ptr)
	: parent_t (data_ptr) {}
    packed_sample_reference(const packed_sample_reference& ref)
	: parent_t (ref._data_ptr) {}
    packed_sample_reference(const mutable_reference& ref)
	: parent_t (ref._data_ptr) {}

    unsigned first_bit () const { return FirstBit; }

    integer_t get () const
    {
	return integer_t ((this->get_data () & sample_mask) >> FirstBit);
    }
};

/**
   \ingroup PackedSampleReferenceModel
   \brief A mutable subbyte sample reference whose bit offset is fixed
   at compile time. Models SampleConcept
*/
template <typename BitField, int FirstBit, int NumBits>
class packed_sample_reference<BitField, FirstBit, NumBits, true> 
   : public detail::packed_sample_reference_base<
    packed_sample_reference<BitField, FirstBit, NumBits, true>,
    BitField, NumBits, true>
{
    typedef detail::packed_sample_reference_base<
	packed_sample_reference<BitField, FirstBit, NumBits, true>,
	BitField, NumBits, true> parent_t;
    friend class packed_sample_reference<BitField, FirstBit, NumBits, false>;

    static const BitField sample_mask =
	static_cast<BitField> (parent_t::max_val) << FirstBit;

public:
    typedef const packed_sample_reference<BitField,FirstBit,NumBits,false>
    const_reference;
    typedef const packed_sample_reference<BitField,FirstBit,NumBits,true>
    mutable_reference;
    typedef typename parent_t::integer_t integer_t;

    explicit packed_sample_reference (void* data_ptr)
	: parent_t (data_ptr) {}

    packed_sample_reference (const packed_sample_reference& ref)
	: parent_t (ref._data_ptr) {}

    const packed_sample_reference& operator= (integer_t value) const
    {
	assert (value<=parent_t::max_val);
	set_unsafe (value);
	return *this;
    }
    
    const packed_sample_reference& operator= (const mutable_reference& ref) const
    {
	set_from_reference (ref.get_data ());
	return *this;
    }
    
    const packed_sample_reference& operator= (const const_reference&   ref) const
    {
	set_from_reference (ref.get_data ());
	return *this;
    }

    template <bool Mutable1>
    const packed_sample_reference& operator= (
	const packed_dynamic_sample_reference<BitField,NumBits,Mutable1>& ref)
	const
    {
	set_unsafe (ref.get ());
	return *this;
    }

    unsigned first_bit () const { return FirstBit; }

    integer_t get () const
    {
	return integer_t((this->get_data()&sample_mask) >> FirstBit);
    }
    
    void set_unsafe (integer_t value) const
    {
	this->set_data ((this->get_data() & ~sample_mask) |
			(( static_cast< BitField >( value )<<FirstBit)));
    }
    
private:
    
    void set_from_reference (const BitField& other_bits) const
    {
	this->set_data((this->get_data() & ~sample_mask) |
		       (other_bits & sample_mask));
    }
};

} /* namespace sound */
} /* namespace psynth */

namespace std
{

/*
  We are forced to define swap inside std namespace because on some
  platforms (Visual Studio 8) STL calls swap qualified.
  
  swap with 'left bias': 
  - swap between proxy and anything
  - swap between value type and proxy
  - swap between proxy and proxy
*/

/**
   \ingroup PackedSampleReferenceModel
   \brief swap for packed_sample_reference
*/
template <typename BF, int FB, int NB, bool M, typename R> inline
void swap(const psynth::sound::packed_sample_reference<BF,FB,NB,M> x, R& y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::packed_sample_reference<BF,FB,NB,M>::value_type>(
	    x, y); 
}


/**
   \ingroup PackedSampleReferenceModel
   \brief swap for packed_sample_reference
*/
template <typename BF, int FB, int NB, bool M> inline
void swap(
    typename psynth::sound::packed_sample_reference<BF,FB,NB,M>::value_type& x,
    const psynth::sound::packed_sample_reference<BF,FB,NB,M> y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::packed_sample_reference<BF,FB,NB,M>::value_type>(
	    x, y); 
}


/**
   \ingroup PackedSampleReferenceModel
   \brief swap for packed_sample_reference
*/
template <typename BF, int FB, int NB, bool M> inline
void swap (
    const psynth::sound::packed_sample_reference<BF,FB,NB,M> x,
    const psynth::sound::packed_sample_reference<BF,FB,NB,M> y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::packed_sample_reference<BF,FB,NB,M>::value_type>(
	    x, y); 
}

} /* namespace std */

namespace psynth
{
namespace sound
{

/**
   \defgroup PackedSampleDynamicReferenceModel
   packed_dynamic_sample_reference
   
   \ingroup SampleModel
   \brief Represents a reference proxy to a sample operating over a
   bit range whose offset is specified at run time. Models
   SampleConcept

   Example:
   \code
   
   // Reference to a 2-bit sample whose offset is specified at
   // construction time

   typedef const packed_dynamic_sample_reference<uint8_t,2,true>
   bits2_dynamic_ref_t;

   uint16_t data=0;
   bits2_dynamic_ref_t sample_ref(&data,1);
   sample_ref = sample_traits<bits2_dynamic_ref_t>::max_value();     // == 3
   assert(data == 6);               // == (3<<1) == 6
\endcode
*/

/**
   \brief Models a constant subbyte sample reference whose bit offset
   is a runtime parameter. Models SampleConcept
   
   Same as packed_sample_reference, except that the offset is a
   runtime parameter
   
 \ingroup PackedSampleDynamicReferenceModel
*/
template <typename BitField, int NumBits> 
class packed_dynamic_sample_reference<BitField,NumBits,false>
   : public detail::packed_sample_reference_base<
    packed_dynamic_sample_reference<BitField, NumBits, false>,
    BitField, NumBits, false>
{
    typedef detail::packed_sample_reference_base<
	packed_dynamic_sample_reference<BitField, NumBits, false>,
	BitField,NumBits,false> parent_t;
    friend class packed_dynamic_sample_reference<BitField, NumBits, true>;

    unsigned _first_bit;     // 0..7

    void operator= (const packed_dynamic_sample_reference&);

public:
    typedef const packed_dynamic_sample_reference<BitField, NumBits, false>
    const_reference;
    typedef const packed_dynamic_sample_reference<BitField,NumBits,true>
    mutable_reference;
    typedef typename parent_t::integer_t
    integer_t;

    packed_dynamic_sample_reference (const void* data_ptr, unsigned first_bit)
	: parent_t(data_ptr)
	, _first_bit(first_bit) {}
    
    packed_dynamic_sample_reference (const const_reference& ref)
	: parent_t (ref._data_ptr)
	, _first_bit(ref._first_bit) {}
    
    packed_dynamic_sample_reference (const mutable_reference& ref)
	: parent_t (ref._data_ptr)
	, _first_bit (ref._first_bit) {}

    unsigned first_bit () const { return _first_bit; }

    integer_t get () const
    { 
        const BitField sample_mask = parent_t::max_val << _first_bit;
        return (static_cast<integer_t>(this->get_data () & sample_mask ) >>
		_first_bit);
    }
};

/**
   \brief Models a mutable subbyte sample reference whose bit offset
   is a runtime parameter. Models SampleConcept
   
   Same as packed_sample_reference, except that the offset is a
   runtime parameter
   
   \ingroup PackedSampleDynamicReferenceModel
*/
template <typename BitField, int NumBits> 
class packed_dynamic_sample_reference<BitField, NumBits, true>
   : public detail::packed_sample_reference_base<
    packed_dynamic_sample_reference<BitField, NumBits, true>,
    BitField, NumBits, true>
{
    typedef detail::packed_sample_reference_base<
	packed_dynamic_sample_reference<BitField, NumBits, true>,
	BitField, NumBits, true> parent_t;
    
    friend class packed_dynamic_sample_reference<BitField, NumBits, false>;

    unsigned _first_bit;

public:
    typedef const packed_dynamic_sample_reference<BitField,NumBits, false>
    const_reference;
    
    typedef const packed_dynamic_sample_reference<BitField,NumBits, true>
    mutable_reference;

    typedef typename parent_t::integer_t integer_t;

    packed_dynamic_sample_reference (void* data_ptr, unsigned first_bit)
	: parent_t (data_ptr)
	, _first_bit (first_bit) {}
    
    packed_dynamic_sample_reference (const packed_dynamic_sample_reference& ref)
	: parent_t(ref._data_ptr)
	, _first_bit (ref._first_bit) {}

    const packed_dynamic_sample_reference&
    operator= (integer_t value) const
    {
	assert (value <= parent_t::max_val);
	set_unsafe (value);
	return *this;
    }
    
    const packed_dynamic_sample_reference&
    operator= (const mutable_reference& ref) const
    {
	set_unsafe (ref.get());
	return *this;
    }
    
    const packed_dynamic_sample_reference&
    operator= (const const_reference& ref) const
    {
	set_unsafe (ref.get ());
	return *this;
    }

    template <typename BitField1, int FirstBit1, bool Mutable1>
    const packed_dynamic_sample_reference& operator= (
	const packed_sample_reference<BitField1, FirstBit1,
				      NumBits, Mutable1>& ref) const 
    {
	set_unsafe (ref.get());
	return *this;
    }

    unsigned first_bit () const { return _first_bit; }

    integer_t get() const
    { 
        const BitField sample_mask = parent_t::max_val<<_first_bit;
        return (static_cast<integer_t>(this->get_data() & sample_mask ) >>
		_first_bit);
    }
    
    void set_unsafe(integer_t value) const
    { 
        const BitField sample_mask = parent_t::max_val << _first_bit;
        this->set_data ((this->get_data() & ~sample_mask) | value << _first_bit); 
    }
};

} /* namespace sound */
} /* namespace psynth */

namespace std
{

/*
  We are forced to define swap inside std namespace because on some
  platforms (Visual Studio 8) STL calls swap qualified.
  
  swap with 'left bias': 
  - swap between proxy and anything
  - swap between value type and proxy
  - swap between proxy and proxy
*/

/**
   \ingroup PackedSampleDynamicReferenceModel
   \brief swap for packed_dynamic_sample_reference
*/
template <typename BF, int NB, bool M, typename R> inline
void swap(const psynth::sound::packed_dynamic_sample_reference<BF,NB,M> x, R& y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::packed_dynamic_sample_reference<
	    BF,NB,M>::value_type> (x, y); 
}


/**
   \ingroup PackedSampleDynamicReferenceModel
   \brief swap for packed_dynamic_sample_reference
*/
template <typename BF, int NB, bool M> inline
void swap (typename psynth::sound::packed_dynamic_sample_reference<BF,NB,M>::value_type& x,
	   const psynth::sound::packed_dynamic_sample_reference<BF,NB,M> y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::packed_dynamic_sample_reference<
	    BF, NB, M>::value_type> (x,y); 
}

/**
   \ingroup PackedSampleDynamicReferenceModel
   \brief swap for packed_dynamic_sample_reference
*/
template <typename BF, int NB, bool M> inline
void swap(const psynth::sound::packed_dynamic_sample_reference<BF,NB,M> x,
	  const psynth::sound::packed_dynamic_sample_reference<BF,NB,M> y)
{ 
    psynth::sound::swap_proxy<
	typename psynth::sound::packed_dynamic_sample_reference<BF,NB,M>::value_type> (x,y); 
}

} /* namespace std */


namespace psynth
{
namespace sound
{

/*
 *
 *    Built-in sample models
 *
 */

/**
   \defgroup bits8 bits8
   \ingroup SampleModel
   \brief 8-bit unsigned integral sample type (typedef from
   uint8_t). Models SampleValueConcept
   \ingroup bits8
*/
typedef boost::uint8_t  bits8;

/**
   \defgroup bits16 bits16
   \ingroup SampleModel
   \brief 16-bit unsigned integral sample type (typedef from
   uint16_t). Models SampleValueConcept
   \ingroup bits16
*/
typedef boost::uint16_t bits16;

/**
   \defgroup bits32 bits32
   \ingroup SampleModel
   \brief 32-bit unsigned integral sample type  (typedef from
   uint32_t). Models SampleValueConcept
   \ingroup bits32
*/
typedef boost::uint32_t bits32;

/**
   \defgroup bits8s bits8s
   \ingroup SampleModel
   \brief 8-bit signed integral sample type (typedef from
   int8_t). Models SampleValueConcept
   \ingroup bits8s
*/
typedef boost::int8_t   bits8s;

/**
   \defgroup bits16s bits16s
   \ingroup SampleModel
   \brief 16-bit signed integral sample type (typedef from
   int16_t). Models SampleValueConcept
   \ingroup bits16s
*/
typedef boost::int16_t  bits16s;

/**
   \defgroup bits32s bits32s
   \ingroup SampleModel
   \brief 32-bit signed integral sample type (typedef from
   int32_t). Models SampleValueConcept
   \ingroup bits32s
*/
typedef boost::int32_t  bits32s;

/**
   \defgroup bits32f bits32f
   \ingroup SampleModel
   \brief 32-bit floating point sample type with range [0.0f
   ... 1.0f]. Models SampleValueConcept
   \ingroup bits32f
*/
typedef scoped_sample_value<float, float_zero, float_one, float_half> bits32f;

/**
   \defgroup bits32sf bits32sf
   \ingroup SampleModel
   \brief 32-bit floating point sample type with range [-1.0f
   ... 1.0f]. Models SampleValueConcept
   \ingroup bits32fs
*/
typedef scoped_sample_value<float, float_minus_one, float_one, float_zero>
bits32sf;

} /* namespace sound */
} /* namespace psynth */

namespace boost
{

/**
 * @todo Integrate in C++0x type_traits?
 */

template <int NumBits>
struct is_integral<psynth::sound::packed_sample_value<NumBits> > :
	public boost::mpl::true_ {};

template <typename BitField, int FirstBit, int NumBits, bool IsMutable>
struct is_integral<psynth::sound::packed_sample_reference<
		       BitField, FirstBit, NumBits, IsMutable> > :
	public boost::mpl::true_ {};

template <typename BitField, int NumBits, bool IsMutable>
struct is_integral<psynth::sound::packed_dynamic_sample_reference<
		       BitField, NumBits, IsMutable> > :
	public boost::mpl::true_ {};

template <typename BaseSampleValue,
	  typename MinVal, typename MaxVal, typename ZeroVal> 
struct is_integral<psynth::sound::scoped_sample_value<
		       BaseSampleValue, MinVal, MaxVal, ZeroVal> > :
	public is_integral<BaseSampleValue> {};

} /* namespace boost */

#endif /* PSYNTH_SAMPLE_HPP_ */
