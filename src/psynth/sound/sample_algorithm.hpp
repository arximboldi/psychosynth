/**
 *  Time-stamp:  <2011-06-18 20:15:44 raskolnikov>
 *
 *  @file        sample_algorithm.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Oct 26 17:21:42 2010
 *
 *  Algorithms over sample types.
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

#ifndef PSYNTH_SAMPLE_ALGORITHM_HPP
#define PSYNTH_SAMPLE_ALGORITHM_HPP

#include <psynth/base/compat.hpp>
#include <psynth/sound/sample.hpp>

#include <boost/mpl/less.hpp>
#include <boost/mpl/integral_c.hpp>
#include <boost/mpl/greater.hpp>
#include <boost/type_traits.hpp>

namespace psynth
{
namespace sound
{

//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable: 4309)
// disable truncation of constant value warning (using -1 to get the
// max value of an integral)
//#endif

namespace detail
{

/* some forward declarations */
template <typename SrcSampleV, typename DstSampleV,
	  bool SrcIsIntegral, bool DstIsIntegral>
struct sample_converter_unsigned_impl;
template <typename SrcSampleV, typename DstSampleV,
	  bool SrcIsGreater>
struct sample_converter_unsigned_integral;
template <typename SrcSampleV, typename DstSampleV,
	  bool SrcLessThanDst, bool SrcDivisible>
struct sample_converter_unsigned_integral_impl;
template <typename SrcSampleV, typename DstSampleV,
	  bool SrcLessThanDst, bool CannotFitInInteger>
struct sample_converter_unsigned_integral_nondivisible;

/**
 * unsigned_integral_max_value - given an unsigned integral sample
 * type, returns its maximum value as an MPL integral constant
 */

template <typename UnsignedIntegralSample>
struct unsigned_integral_max_value :
	public boost::mpl::integral_c<UnsignedIntegralSample, -1> {};

template <>
struct unsigned_integral_max_value<uint8_t> :
	public boost::mpl::integral_c<uint32_t, 0xFF> {};
template <>
struct unsigned_integral_max_value<uint16_t> :
	public boost::mpl::integral_c<uint32_t, 0xFFFF> {};
template <>
struct unsigned_integral_max_value<uint32_t> :
	public boost::mpl::integral_c<uintmax_t, 0xFFFFFFFF> {};

template <int K>
struct unsigned_integral_max_value<packed_sample_value<K> >
    : public boost::mpl::integral_c<
    typename packed_sample_value<K>::integer_t, (1 << K) - 1> {};

/**
 * unsigned_integral_num_bits - given an unsigned integral sample
 * type, returns the minimum number of bits needed to represent it
 */

template <typename UnsignedIntegralSample>
struct unsigned_integral_num_bits :
	public boost::mpl::int_<sizeof(UnsignedIntegralSample)*8> {};

template <int K>
struct unsigned_integral_num_bits<packed_sample_value<K> >
    : public boost::mpl::int_<K> {};

} /* namespace detail */

/**
   \defgroup SampleConvertAlgorithm sample_convert
   \brief Converting from one sample type to another
   \ingroup SampleAlgorithm

   Conversion is done as a simple linear mapping of one sample range
   to the other, such that the minimum/maximum value of the source
   maps to the minimum/maximum value of the destination.  One
   implication of this is that the value 0 of signed samples may not
   be preserved!

   When creating new sample models, it is often a good idea to provide
   specializations for the sample conversion algorithms, for example,
   for performance optimizations. If the new model is an integral type
   that can be signed, it is easier to define the conversion only for
   the unsigned type (\p sample_converter_unsigned) and provide
   specializations of \p detail::sample_convert_to_unsigned and \p
   detail::sample_convert_from_unsigned to convert between the signed
   and unsigned type.

   Example:
   \code
   // bits32f is a floating point sample with range [0.0f ... 1.0f]
   bits32f src_sample = sample_traits<bits32f>::max_value();
   assert(src_sample == 1);

   // bits8 is 8-bit unsigned integral sample (typedef-ed from unsigned char)
   bits8 dst_sample = sample_convert<bits8>(src_sample);
   assert(dst_sample == 255);     // max value goes to max value
   \endcode
*/

/**
    \defgroup SampleConvertUnsignedAlgorithm sample_converter_unsigned
    \ingroup SampleConvertAlgorithm
    \brief Convert one unsigned/floating point sample to
    another. Converts both the sample type and range
 @{
 */

/**
 *  sample_converter_unsigned
 */

template <typename SrcSampleV, typename DstSampleV>
// Model SampleValueConcept
struct sample_converter_unsigned
    : public detail::sample_converter_unsigned_impl<
    SrcSampleV, DstSampleV,
    boost::is_integral<SrcSampleV>::value,
    boost::is_integral<DstSampleV>::value>
{};

/** \brief Converting a sample to itself - identity operation */
template <typename T> struct sample_converter_unsigned<T,T> :
	public detail::identity<T> {};


namespace detail
{

/**
 *  sample_converter_unsigned_impl
 */

/**
   \brief This is the default implementation. Performance
   specializatons are provided
*/
template <typename SrcSampleV, typename DstSampleV,
	  bool SrcIsIntegral, bool DstIsIntegral>
struct sample_converter_unsigned_impl :
	public std::unary_function <DstSampleV, SrcSampleV>
{
    DstSampleV operator () (SrcSampleV src) const
    {
        return DstSampleV (
	    sample_traits<DstSampleV>::min_value() +
	    (src - sample_traits<SrcSampleV>::min_value()) /
	    sample_range<SrcSampleV>() * sample_range<DstSampleV> ());
    }

private:
    template <typename C>
    static double sample_range ()
    {
        return double (sample_traits<C>::max_value ()) -
	    double (sample_traits<C>::min_value ());
    }
};

// When both the source and the destination are integral samples,
// perform a faster conversion
template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_impl<SrcSampleV, DstSampleV, true, true>
    : public sample_converter_unsigned_integral<SrcSampleV, DstSampleV,
    boost::mpl::less<unsigned_integral_max_value<SrcSampleV>,
	      unsigned_integral_max_value<DstSampleV> >::value > {};


/**
 *  sample_converter_unsigned_integral
 */

template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_integral<SrcSampleV, DstSampleV, true>
    : public sample_converter_unsigned_integral_impl<
    SrcSampleV, DstSampleV, true,
    !(unsigned_integral_max_value<DstSampleV>::value %
      unsigned_integral_max_value<SrcSampleV>::value)> {};

template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_integral<SrcSampleV,DstSampleV, false>
    : public sample_converter_unsigned_integral_impl<
    SrcSampleV, DstSampleV, false,
    !(unsigned_integral_max_value<SrcSampleV>::value %
      unsigned_integral_max_value<DstSampleV>::value) > {};


/**
 *  sample_converter_unsigned_integral_impl
 */

/*
  Both source and destination are unsigned integral samples, the src
  max value is less than the dst max value, and the dst max value is
  divisible by the src max value
*/
template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_integral_impl<SrcSampleV, DstSampleV, true, true>
{
    DstSampleV operator () (SrcSampleV src) const
    {
        typedef typename unsigned_integral_max_value<DstSampleV>::value_type
	    integer_t;
        static const integer_t mul =
	    unsigned_integral_max_value<DstSampleV>::value /
	    unsigned_integral_max_value<SrcSampleV>::value;
        return DstSampleV (src * mul);
    }
};

/*
  Both source and destination are unsigned integral samples, the dst
  max value is less than (or equal to) the src max value, and the src
  max value is divisible by the dst max value
*/
template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_integral_impl<SrcSampleV,DstSampleV, false, true>
{
    DstSampleV operator () (SrcSampleV src) const
    {
        typedef typename unsigned_integral_max_value<SrcSampleV>::value_type
	    integer_t;
        static const integer_t div =
	    unsigned_integral_max_value<SrcSampleV>::value /
	    unsigned_integral_max_value<DstSampleV>::value;
        static const integer_t div2 = div / 2;
        return DstSampleV ((src + div2) / div);
    }
};

/* Prevent overflow for the largest integral type */
template <typename DstSampleV>
struct sample_converter_unsigned_integral_impl<uintmax_t, DstSampleV, false, true>
{
    DstSampleV operator () (uintmax_t src) const
    {
        static const uintmax_t div =
	    unsigned_integral_max_value<bits32>::value /
	    unsigned_integral_max_value<DstSampleV>::value;
        static const uintmax_t div2 = div/2;
        if (src > unsigned_integral_max_value<uintmax_t>::value - div2)
            return unsigned_integral_max_value<DstSampleV>::value;
        return DstSampleV ((src + div2) / div);
    }
};

/*
  Both source and destination are unsigned integral samples, and the
  dst max value is not divisible by the src max value See if you can
  represent the expression (src * dst_max) / src_max in integral form
*/
template <typename SrcSampleV, typename DstSampleV, bool SrcLessThanDst>
struct sample_converter_unsigned_integral_impl<SrcSampleV, DstSampleV,
					       SrcLessThanDst, false>
    : public sample_converter_unsigned_integral_nondivisible<
    SrcSampleV, DstSampleV,
    SrcLessThanDst,
    boost::mpl::greater<
        boost::mpl::plus<unsigned_integral_num_bits<SrcSampleV>,
		  unsigned_integral_num_bits<DstSampleV> >,
        unsigned_integral_num_bits<uintmax_t> >::value>
{};


/*
  Both source and destination are unsigned integral samples, the src
  max value is less than the dst max value, and the dst max value is
  not divisible by the src max value The expression (src * dst_max) /
  src_max fits in an integer
*/
template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_integral_nondivisible<
    SrcSampleV, DstSampleV, true, false>
{
    DstSampleV operator () (SrcSampleV src) const
    {
        typedef typename detail::min_fast_uint<
	    unsigned_integral_num_bits<SrcSampleV>::value +
	    unsigned_integral_num_bits<DstSampleV>::value>::type integer_t;

        return DstSampleV (
	    integer_t (src * unsigned_integral_max_value<DstSampleV>::value) /
	    unsigned_integral_max_value<SrcSampleV>::value);
    }
};

/*
  Both source and destination are unsigned integral samples, the src
  max value is less than the dst max value, and the dst max value is
  not divisible by the src max value The expression (src * dst_max) /
  src_max cannot fit in an integer (overflows). Use a double
*/
template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_unsigned_integral_nondivisible <
    SrcSampleV, DstSampleV, true, true>
{
    DstSampleV operator () (SrcSampleV src) const
    {
        static const double mul =
	    unsigned_integral_max_value<DstSampleV>::value /
	    double (unsigned_integral_max_value<SrcSampleV>::value);
        return DstSampleV (src * mul);
    }
};

/*
  Both source and destination are unsigned integral samples, the dst
  max value is less than (or equal to) the src max value, and the src
  max value is not divisible by the dst max value
*/
template <typename SrcSampleV, typename DstSampleV, bool CannotFit>
struct sample_converter_unsigned_integral_nondivisible<
    SrcSampleV, DstSampleV, false, CannotFit>
{
    DstSampleV operator () (SrcSampleV src) const
    {
        typedef typename unsigned_integral_max_value<SrcSampleV>::value_type
	    integer_t;
        static const double div =
	    unsigned_integral_max_value<SrcSampleV>::value /
	    double (unsigned_integral_max_value<DstSampleV>::value);
        static const integer_t div2 = integer_t (div/2);
        return DstSampleV ((src + div2) / div);
    }
};

} /* namespace detail */


/**
 *    bits32f conversion
 *    @todo We changed the range, we have to change it back here.
 */

template <typename DstSampleV>
struct sample_converter_unsigned<bits32f,DstSampleV> :
    public std::unary_function<bits32f,DstSampleV>
{
    DstSampleV   operator () (bits32f x) const
    {
	return DstSampleV (x * sample_traits<DstSampleV>::max_value () + 0.5f);
    }
};

template <typename SrcSampleV>
struct sample_converter_unsigned<SrcSampleV, bits32f> :
    public std::unary_function<SrcSampleV, bits32f>
{
    bits32f operator () (SrcSampleV x) const
    {
	return bits32f (x / float (sample_traits<SrcSampleV>::max_value ()));
    }
};

template <>
struct sample_converter_unsigned<bits32f, bits32f> :
    public std::unary_function<bits32f, bits32f>
{
    bits32f operator() (bits32f x) const { return x; }
};


/** \brief 32 bit <-> float sample conversion */
template <>
struct sample_converter_unsigned<bits32, bits32f> :
    public std::unary_function<bits32, bits32f>
{
    bits32f operator () (bits32 x) const
    {
        // unfortunately without an explicit check it is possible to
        // get a round-off error. We must ensure that max_value of
        // bits32 matches max_value of bits32f
        if (x >= sample_traits<bits32>::max_value ())
	    return sample_traits<bits32f>::max_value ();
        return float (x) / float (sample_traits<bits32>::max_value ());
    }
};

/** \brief 32 bit <-> float sample conversion */
template <> struct sample_converter_unsigned<bits32f, bits32> :
    public std::unary_function<bits32f, bits32>
{
    bits32 operator () (bits32f x) const
    {
        // unfortunately without an explicit check it is possible to
        // get a round-off error. We must ensure that max_value of
        // bits32 matches max_value of bits32f
        if (x >= sample_traits<bits32f>::max_value ())
	    return sample_traits<bits32>::max_value();
        return bits32 (x * sample_traits<bits32>::max_value () + 0.5f);
    }
};

/** @} */

namespace detail
{

/*
  Converting from signed to unsigned integral sample.
  It is both a unary function, and a metafunction (thus requires the
  'type' nested typedef, which equals result_type)
*/
template <typename SampleValue> // Model SampleValueConcept
struct sample_convert_to_unsigned : public detail::identity<SampleValue>
{
    typedef SampleValue type;
};

template <>
struct sample_convert_to_unsigned<bits8s> :
	public std::unary_function<bits8s, bits8>
{
    typedef bits8 type;
    type operator () (bits8s val) const { return val + 128; }
};

template <>
struct sample_convert_to_unsigned <bits16s> :
    public std::unary_function<bits16s, bits16>
{
    typedef bits16 type;
    type operator () (bits16s  val) const { return val + 32768; }
};

template <> struct sample_convert_to_unsigned<bits32s> :
	public std::unary_function<bits32s, bits32>
{
    typedef bits32 type;
    type operator () (bits32s x) const
    { return static_cast<bits32>(x + (1 << 31)); }
};

template <> struct sample_convert_to_unsigned<bits32sf> :
	public std::unary_function<bits32sf, bits32f>
{
    typedef bits32f type;
    type operator () (bits32sf x) const
    { return static_cast<float>((x + 1.0f) * .5f); }
};

/*
  Converting from unsigned to signed integral sample
  It is both a unary function, and a metafunction (thus requires the
  'type' nested typedef, which equals result_type)
*/
template <typename SampleValue>  // Model SampleValueConcept
struct sample_convert_from_unsigned : public detail::identity<SampleValue>
{
    typedef SampleValue type;
};

template <>
struct sample_convert_from_unsigned<bits8s> :
    public std::unary_function<bits8, bits8s>
{
    typedef bits8s type;
    type  operator () (bits8  val) const
    { return val-128; }
};

template <>
struct sample_convert_from_unsigned<bits16s> :
    public std::unary_function<bits16,bits16s>
{
    typedef bits16s type;
    type operator () (bits16 val) const
    { return val-32768; }
};

template <>
struct sample_convert_from_unsigned<bits32s> :
    public std::unary_function<bits32, bits32s>
{
    typedef bits32s type;
    type operator () (bits32 x) const
    { return static_cast<bits32s>(x-(1<<31)); }
};

template <>
struct sample_convert_from_unsigned<bits32sf> :
    public std::unary_function<bits32f, bits32sf>
{
    typedef bits32sf type;
    type operator () (bits32f x) const {
        return static_cast<float>(x * 2.0f - 1.0f);
    }
};

template <typename SrcSampleV, typename DstSampleV, bool DstIsUnsigned>
struct sample_converter_impl
{
    PSYNTH_FORCEINLINE
    DstSampleV operator () (const SrcSampleV& src) const
    {
       typedef detail::sample_convert_to_unsigned<SrcSampleV> to_unsigned;
       typedef sample_converter_unsigned<typename to_unsigned::result_type,
                                         DstSampleV> converter_unsigned;
       return converter_unsigned () (to_unsigned () (src));
    }
};

template <typename SrcSampleV, typename DstSampleV>
struct sample_converter_impl<SrcSampleV, DstSampleV, false>
{
    PSYNTH_FORCEINLINE
    DstSampleV operator () (const SrcSampleV& src) const
    {
        typedef detail::sample_convert_to_unsigned<SrcSampleV> to_unsigned;
        typedef detail::sample_convert_from_unsigned<DstSampleV> from_unsigned;
        typedef sample_converter_unsigned<typename to_unsigned::result_type,
                                          typename from_unsigned::argument_type>
	    converter_unsigned;
        return from_unsigned () (converter_unsigned () (to_unsigned () (src)));
    }
};

}  /* namespace detail */

/**
   \ingroup SampleConvertAlgorithm
   \brief A unary function object converting between sample types
*/
template <typename SrcSampleV, typename DstSampleV> // Model SampleValueConcept
struct sample_converter : public std::unary_function<SrcSampleV, DstSampleV>
{
    typedef detail::sample_convert_from_unsigned<DstSampleV> from_unsigned;
    typedef detail::sample_converter_impl<
        SrcSampleV, DstSampleV,
        std::is_same<typename from_unsigned::argument_type,
                     DstSampleV>::value> converter_impl;

    PSYNTH_FORCEINLINE
    DstSampleV operator () (const SrcSampleV& src) const
    {
        return converter_impl () (src);
    }
};


/**
   \ingroup SampleConvertAlgorithm
   \brief Converting from one sample type to another.
*/
template <typename DstSample, typename SrcSample>
// Model SampleConcept (could be sample references)
inline typename sample_traits<DstSample>::value_type
sample_convert (const SrcSample& src)
{
    return sample_converter<
	typename sample_traits<SrcSample>::value_type,
	typename sample_traits<DstSample>::value_type>() (src);
}

/**
   \ingroup SampleConvertAlgorithm

   \brief Same as sample_converter, except it takes the destination
   sample by reference, which allows us to move the templates from the
   class level to the method level. This is important when invoking it
   on heterogeneous pixels.
*/

struct default_sample_converter
{
    template <typename Ch1, typename Ch2>
    void operator () (const Ch1& src, Ch2& dst) const
    {
        dst = sample_convert<Ch2> (src);
    }
};

namespace detail
{

/* fast integer division by 255 */
inline uint32_t div255 (uint32_t in)
{
    uint32_t tmp = in + 128;
    return (tmp + (tmp >> 8)) >> 8;
}

/* fast integer divison by 32768 */
inline uint32_t div32768 (uint32_t in)
{
    return (in + 16384) >> 15;
}

} /* namespace detail */

/**
   \defgroup SampleMultiplyAlgorithm sample_multiply
   \ingroup SampleAlgorithm

    \brief Multiplying unsigned sample values of the same
    type. Performs scaled multiplication result = a * b / max_value

    Example:
    \code
    bits8 x=128;
    bits8 y=128;
    bits8 mul = sample_multiply(x,y);
    assert(mul == 64);    // 64 = 128 * 128 / 255
    \endcode
*/

/** @{
    \brief This is the default implementation. Performance specializatons
    are provided.
*/

template <typename SampleValue>
struct sample_multiplier_unsigned :
    public std::binary_function<SampleValue,SampleValue,SampleValue>
{
    SampleValue operator () (SampleValue a, SampleValue b) const
    {
        return SampleValue (
	    a / double (sample_traits<SampleValue>::max_value()) * b);
    }
};

/**
   \brief Specialization of sample_multiply for 8-bit unsigned samples
*/
template<>
struct sample_multiplier_unsigned<bits8> :
    public std::binary_function<bits8, bits8, bits8>
{
    bits8 operator () (bits8 a, bits8 b) const
    {
	return bits8 (detail::div255 (uint32_t (a) * uint32_t (b)));
    }
};

/**
   \brief Specialization of sample_multiply for 16-bit unsigned
   samples
*/
template<>
struct sample_multiplier_unsigned<bits16> :
    public std::binary_function<bits16,bits16,bits16>
{
    bits16 operator () (bits16 a, bits16 b) const
    {
	return bits16 ((uint32_t (a) * uint32_t (b)) / 65535);
    }
};

/**
   \brief Specialization of sample_multiply for float 0..1 samples
*/
template<>
struct sample_multiplier_unsigned<bits32f> :
    public std::binary_function<bits32f, bits32f, bits32f>
{
    bits32f operator () (bits32f a, bits32f b) const { return a * b; }
};

/**
   \brief A function object to multiply two samples. result = a * b /
   max_value
*/
template <typename SampleValue>
struct sample_multiplier :
    public std::binary_function<SampleValue, SampleValue, SampleValue>
{
    SampleValue operator () (SampleValue a, SampleValue b) const
    {
        typedef detail::sample_convert_to_unsigned<SampleValue> to_unsigned;
        typedef detail::sample_convert_from_unsigned<SampleValue> from_unsigned;
        typedef sample_multiplier_unsigned<typename to_unsigned::result_type>
	    multiplier_unsigned;
        return from_unsigned () (multiplier_unsigned () (
				     to_unsigned () (a), to_unsigned () (b)));
    }
};

/** @todo Is this needed? */
template <>
struct sample_multiplier<bits32f> :
    public std::binary_function<bits32f, bits32f, bits32f>
{
    bits32f operator () (bits32f a, bits32f b) const
    { return a * b; }
};

/**
   \brief A function multiplying two samples. result = a * b /
   max_value
*/
template <typename Sample> // Models SampleConcept (could be a sample reference)
inline typename sample_traits<Sample>::value_type
sample_multiply (Sample a, Sample b)
{
    return sample_multiplier<
        typename sample_traits<Sample>::value_type> () (a, b);
}

/** @} */

/**
   \defgroup SampleInvertAlgorithm sample_invert
   \ingroup SampleAlgorithm
   \brief Returns the inverse of a sample. result = max_value - x +
   min_value

   Example:
   \code
   // bits8 == uint8_t == unsigned char
   bits8 x=255;
   bits8 inv = sample_invert(x);
   assert(inv == 0);
   \endcode
*/

/**
   \brief Default implementation. Provide overloads for performance
   \ingroup SampleInvertAlgorithm sample_invert
*/
template <typename Sample> // Models SampleConcept (could be a sample reference)
inline typename sample_traits<Sample>::value_type sample_invert (Sample x)
{
    return
	sample_traits<Sample>::max_value() - x +
	sample_traits<Sample>::min_value();
}

//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif

}  /* namespace sound */
}  /* namespace psynth */

#endif
