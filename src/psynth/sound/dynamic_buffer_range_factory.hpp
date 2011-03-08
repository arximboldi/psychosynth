/**
 *  Time-stamp:  <2011-03-08 18:59:32 raskolnikov>
 *
 *  @file        dynamic_buffer_range_factory.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 13:58:06 2010
 *
 *  Methods for constructing any image ranges from other any image
 *  ranges
 */

#ifndef PSYNTH_SOUND_DYNAMIC_BUFFER_RANGES_HPP
#define PSYNTH_SOUND_DYNAMIC_BUFFER_RANGES_HPP

#include <psynth/sound/dynamic_buffer_range.hpp>
#include <psynth/sound/buffer_range_factory.hpp>

namespace psynth
{
namespace sound
{

namespace detail
{

template <typename Result>
struct flipped_range_fn
{
    typedef Result result_type;
    template <typename Range>
    result_type operator() (const Range& src) const
    {
	return result_type (flipped_range (src));
    }
};

template <typename Result>
struct sub_range_fn
{
    typedef Result result_type;
    sub_range_fn (std::ptrdiff_t start, std::ptrdiff_t size)
	: _start (start)
	, _size (size) {}
    
    template <typename Range>
    result_type operator () (const Range& src) const
    {
	return result_type (sub_range (src, _start, _size));
    }

    std::ptrdiff_t _start, _size;
};

template <typename Result>
struct sub_sampled_range_fn
{
    typedef Result result_type;
    sub_sampled_range_fn (std::ptrdiff_t step)
	: _step (step) {}
    
    template <typename Range>
    result_type operator () (const Range& src) const
    {
	return result_type (sub_sampled_range (src,_step));
    }

    std::ptrdiff_t _step;
};

template <typename Result>
struct nth_sample_range_fn
{
    typedef Result result_type;

    nth_sample_range_fn(int n)
	: _n (n) {}
    
    template <typename Range>
    result_type operator () (const Range& src) const
    {
	return result_type (nth_sample_range (src, _n));
    }

    int _n;
};

template <typename DstP, typename Result,
	  typename CC = default_channel_converter>
struct channel_converted_range_fn
{
    typedef Result result_type;

    channel_converted_range_fn (CC cc = CC()):
	_cc (cc) {}

    template <typename Range>
    result_type operator () (const Range& src) const
    {
	return result_type (channel_converted_range<DstP> (src, _cc));
    }

private:
    CC _cc;
};


template <typename Result>
struct flipped_fn
{
    typedef Result result_type;
    template <typename Range>
    result_type operator() (const Range& src) const
    {
	return result_type (flipped_range (src));
    }
};

} /* namespace detail */


/**
 *  \ingroup BufferRangeTransformationsFlipUD
 */
template <typename RangeTypes> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename dynamic_step_type<dynamic_buffer_range<RangeTypes> >::type
flipped_range (const dynamic_buffer_range<RangeTypes>& src)
{ 
    return apply_operation (
	src, detail::flipped_fn<
	    typename dynamic_step_type<
		dynamic_buffer_range<RangeTypes> >::type>()); 
}

/** \ingroup BufferRangeTransformationsSubbuffer */
template <typename RangeTypes> inline
// Models MPL Random Access Container of models of BufferRangeConcept
dynamic_buffer_range <RangeTypes>
sub_range (const dynamic_buffer_range<RangeTypes>& src,
           size_t start, size_t size)
{
    return apply_operation (src, detail::sub_range_fn<
				dynamic_buffer_range<RangeTypes> >(start, size)); 
}

/** \ingroup BufferRangeTransformationsSub_Sampled */
template <typename RangeTypes> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename dynamic_step_type<dynamic_buffer_range<RangeTypes> >::type
sub_sampled_range (const dynamic_buffer_range<RangeTypes>& src,
		   std::ptrdiff_t step)
{ 
    return apply_operation (
	src, detail::sub_sampled_range_fn<
	    typename dynamic_step_type<
		dynamic_buffer_range<RangeTypes> >::type> (step)); 
}

namespace detail
{

template <typename Range>
struct get_nthsample_type
{
    typedef typename nth_sample_range_type<Range>::type type;
};

template <typename Ranges>
struct ranges_get_nthsample_type : public boost::mpl::transform<
    Ranges, get_nthsample_type<boost::mpl::_1> > {};

} /* namespace detail */

/**
 * \ingroup BufferRangeTransformationsNthSample
 * \brief Given a runtime source buffer range, returns the type of a
 * runtime buffer range over a single sample of the source range
 */
template <typename RangeTypes>
struct nth_sample_range_type<dynamic_buffer_range<RangeTypes> >
{
    typedef dynamic_buffer_range<
	typename detail::ranges_get_nthsample_type<RangeTypes>::type> type;
};

/** \ingroup BufferRangeTransformationsNthSample */
template <typename RangeTypes> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename nth_sample_range_type<dynamic_buffer_range<RangeTypes> >::type
nth_sample_range (const dynamic_buffer_range<RangeTypes>& src, int n)
{ 
    return apply_operation (
	src, detail::nth_sample_range_fn<
        typename nth_sample_range_type<
        dynamic_buffer_range<RangeTypes> >::type>(n)); 
}

namespace detail
{

template <typename Range, typename DstP, typename CC>
struct get_ccv_type : public channel_converted_range_type<Range, DstP, CC> {};

template <typename Ranges, typename DstP, typename CC>
struct ranges_get_ccv_type : public boost::mpl::transform<
    Ranges, get_ccv_type<boost::mpl::_1, DstP, CC> > {};

} /* namespace detail */

/**
 * \ingroup BufferRangeTransformationsChannelConvert
 * \brief Returns the type of a runtime-specified range,
 * channel-converted to a given frame type with user specified channel
 * converter
 */
template <typename RangeTypes, typename DstP, typename CC>
struct channel_converted_range_type<dynamic_buffer_range<RangeTypes>,DstP,CC>
{
    typedef dynamic_buffer_range<
	typename detail::ranges_get_ccv_type<RangeTypes, DstP, CC>::type> type;
};

/**
 * \ingroup BufferRangeTransformationsChannelConvert
 * \brief overload of generic channel_converted_range with user defined
 * channel-converter
 */
template <typename DstP, typename RangeTypes, typename CC> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename channel_converted_range_type<
    dynamic_buffer_range<RangeTypes>, DstP, CC>::type
channel_converted_range (const dynamic_buffer_range<RangeTypes>& src,CC cc)
{ 
    return apply_operation (
	src, detail::channel_converted_range_fn<
        DstP,typename channel_converted_range_type<
        dynamic_buffer_range<RangeTypes>, DstP, CC>::type >()); 
}

/**
 * \ingroup BufferRangeTransformationsChannelConvert
 * \brief Returns the type of a runtime-specified range,
 * channel-converted to a given frame type with the default coor
 * converter
 */
template <typename RangeTypes, typename DstP>
struct channel_converted_range_type<dynamic_buffer_range<RangeTypes>,DstP>
{
    typedef dynamic_buffer_range<
	typename detail::ranges_get_ccv_type<
	    RangeTypes, DstP, default_channel_converter>::type> type;
};

/**
 * \ingroup BufferRangeTransformationsChannelConvert
 * \brief overload of generic channel_converted_range with the default
 * channel-converter
 */
template <typename DstP, typename RangeTypes> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename channel_converted_range_type<dynamic_buffer_range<RangeTypes>, DstP>::type
channel_converted_range (const dynamic_buffer_range<RangeTypes>& src)
{ 
    return apply_operation (
	src, detail::channel_converted_range_fn <
        DstP,typename channel_converted_range_type<
        dynamic_buffer_range<RangeTypes>, DstP>::type >()); 
}

/**
 * \ingroup BufferRangeTransformationsChannelConvert
 * \brief overload of generic channel_converted_range with user defined
 * channel-converter
 *
 * These are workarounds for GCC 3.4, which thinks
 * channel_converted_range is ambiguous with the same method for
 * templated ranges (in gil/buffer_range_factory.hpp)
 */
template <typename DstP, typename RangeTypes, typename CC> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename channel_converted_range_type<
    dynamic_buffer_range<RangeTypes>, DstP, CC>::type
dynamic_channel_converted_range (const dynamic_buffer_range<RangeTypes>& src, CC cc)
{ 
    return apply_operation (
	src, detail::channel_converted_range_fn<
        DstP,typename channel_converted_range_type<
        dynamic_buffer_range<RangeTypes>, DstP, CC>::type >()); 
}

/**
 * \ingroup BufferRangeTransformationsChannelConvert
 * \brief overload of generic channel_converted_range with the default
 * channel-converter
 *
 * These are workarounds for GCC 3.4, which thinks
 * channel_converted_range is ambiguous with the same method for
 * templated ranges (in sound/buffer_range_factory.hpp)
 */
template <typename DstP, typename RangeTypes> inline
// Models MPL Random Access Container of models of BufferRangeConcept
typename channel_converted_range_type<dynamic_buffer_range<RangeTypes>, DstP>::type
dynamic_channel_converted_range (const dynamic_buffer_range<RangeTypes>& src)
{ 
    return apply_operation (
	src, detail::channel_converted_range_fn<
        DstP, typename channel_converted_range_type<
        dynamic_buffer_range<RangeTypes>, DstP>::type >()); 
}

/** \} */

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_BUFFER_RANGE_FACTORY */
