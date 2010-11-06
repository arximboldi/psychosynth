/**
 *  Time-stamp:  <2010-11-05 02:16:22 raskolnikov>
 *
 *  @file        dynamic_buffer_view_factory.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Nov  4 13:58:06 2010
 *
 *  Methods for constructing any image views from other any image
 *  views
 */

#ifndef PSYNTH_SOUND_DYNAMIC_BUFFER_VIEWS_HPP
#define PSYNTH_SOUND_DYNAMIC_BUFFER_VIEWS_HPP

#include <psynth/sound/dynamic_buffer_view.hpp>
#include <psynth/sound/buffer_view_factory.hpp>

namespace psynth
{
namespace sound
{

namespace detail
{

template <typename Result>
struct flipped_view_fn
{
    typedef Result result_type;
    template <typename View>
    result_type operator() (const View& src) const
    {
	return result_type (flipped_view (src));
    }
};

template <typename Result>
struct sub_buffer_view_fn
{
    typedef Result result_type;
    sub_buffer_view_fn (std::ptrdiff_t start, std::ptrdiff_t size)
	: _start (start)
	, _size (size) {}
    
    template <typename View>
    result_type operator () (const View& src) const
    {
	return result_type (sub_buffer_view (src, _start, _size));
    }

    std::ptrdiff_t _start, _size;
};

template <typename Result>
struct sub_sampled_view_fn
{
    typedef Result result_type;
    sub_sampled_view_fn (std::ptrdiff_t step)
	: _step (step) {}
    
    template <typename View>
    result_type operator () (const View& src) const
    {
	return result_type (sub_sampled_view (src,_step));
    }

    std::ptrdiff_t _step;
};

template <typename Result>
struct nth_sample_view_fn
{
    typedef Result result_type;

    nth_sample_view_fn(int n)
	: _n (n) {}
    
    template <typename View>
    result_type operator () (const View& src) const
    {
	return result_type (nth_sample_view (src, _n));
    }

    int _n;
};

template <typename DstP, typename Result,
	  typename CC = default_channel_converter>
struct channel_converted_view_fn
{
    typedef Result result_type;

    channel_converted_view_fn (CC cc = CC()):
	_cc (cc) {}

    template <typename View>
    result_type operator () (const View& src) const
    {
	return result_type (channel_converted_view<DstP> (src, _cc));
    }

private:
    CC _cc;
};


template <typename Result>
struct flipped_fn
{
    typedef Result result_type;
    template <typename View>
    result_type operator() (const View& src) const
    {
	return result_type (flipped_view (src));
    }
};

} /* namespace detail */


/**
 *  \ingroup BufferViewTransformationsFlipUD
 */
template <typename ViewTypes> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename dynamic_step_type<dynamic_buffer_view<ViewTypes> >::type
flipped_view (const dynamic_buffer_view<ViewTypes>& src)
{ 
    return apply_operation (
	src, detail::flipped_fn<
	    typename dynamic_step_type<
		dynamic_buffer_view<ViewTypes> >::type>()); 
}

/** \ingroup BufferViewTransformationsSubbuffer */
template <typename ViewTypes> inline
// Models MPL Random Access Container of models of BufferViewConcept
dynamic_buffer_view <ViewTypes>
sub_buffer_view (const dynamic_buffer_view<ViewTypes>& src,
		 std::ptrdiff_t start,
		 std::ptrdiff_t size)
{ 
    return apply_operation (src, detail::sub_buffer_view_fn<
				dynamic_buffer_view<ViewTypes> >(start, size)); 
}

/** \ingroup BufferViewTransformationsSub_Sampled */
template <typename ViewTypes> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename dynamic_step_type<dynamic_buffer_view<ViewTypes> >::type
sub_sampled_view (const dynamic_buffer_view<ViewTypes>& src, std::ptrdiff_t step)
{ 
    return apply_operation (
	src, detail::sub_sampled_view_fn<
	    typename dynamic_step_type<
		dynamic_buffer_view<ViewTypes> >::type> (step)); 
}

namespace detail
{

template <typename View>
struct get_nthsample_type
{
    typedef typename nth_sample_view_type<View>::type type;
};

template <typename Views>
struct views_get_nthsample_type : public boost::mpl::transform<
    Views, get_nthsample_type<boost::mpl::_1> > {};

} /* namespace detail */

/**
 * \ingroup BufferViewTransformationsNthSample
 * \brief Given a runtime source buffer view, returns the type of a
 * runtime buffer view over a single sample of the source view
 */
template <typename ViewTypes>
struct nth_sample_view_type<dynamic_buffer_view<ViewTypes> >
{
    typedef dynamic_buffer_view<
	typename detail::views_get_nthsample_type<ViewTypes>::type> type;
};

/** \ingroup BufferViewTransformationsNthSample */
template <typename ViewTypes> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename nth_sample_view_type<dynamic_buffer_view<ViewTypes> >::type
nth_sample_view (const dynamic_buffer_view<ViewTypes>& src, int n)
{ 
    return apply_operation (
	src, detail::nth_sample_view_fn<
	    typename nth_sample_view_type<
		dynamic_buffer_view<ViewTypes> >::type>(n)); 
}

namespace detail
{

template <typename View, typename DstP, typename CC>
struct get_ccv_type : public channel_converted_view_type<View, DstP, CC> {};

template <typename Views, typename DstP, typename CC>
struct views_get_ccv_type : public boost::mpl::transform<
    Views, get_ccv_type<boost::mpl::_1, DstP, CC> > {};

} /* namespace detail */

/**
 * \ingroup BufferViewTransformationsChannelConvert
 * \brief Returns the type of a runtime-specified view,
 * channel-converted to a given frame type with user specified channel
 * converter
 */
template <typename ViewTypes, typename DstP, typename CC>
struct channel_converted_view_type<dynamic_buffer_view<ViewTypes>,DstP,CC>
{
    typedef dynamic_buffer_view<
	typename detail::views_get_ccv_type<ViewTypes, DstP, CC>::type> type;
};

/**
 * \ingroup BufferViewTransformationsChannelConvert
 * \brief overload of generic channel_converted_view with user defined
 * channel-converter
 */
template <typename DstP, typename ViewTypes, typename CC> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename channel_converted_view_type<
    dynamic_buffer_view<ViewTypes>, DstP, CC>::type
channel_converted_view (const dynamic_buffer_view<ViewTypes>& src,CC cc)
{ 
    return apply_operation (
	src, detail::channel_converted_view_fn<
	    DstP,typename channel_converted_view_type<
		dynamic_buffer_view<ViewTypes>, DstP, CC>::type >()); 
}

/**
 * \ingroup BufferViewTransformationsChannelConvert
 * \brief Returns the type of a runtime-specified view,
 * channel-converted to a given frame type with the default coor
 * converter
 */
template <typename ViewTypes, typename DstP>
struct channel_converted_view_type<dynamic_buffer_view<ViewTypes>,DstP>
{
    typedef dynamic_buffer_view<
	typename detail::views_get_ccv_type<
	    ViewTypes, DstP, default_channel_converter>::type> type;
};

/**
 * \ingroup BufferViewTransformationsChannelConvert
 * \brief overload of generic channel_converted_view with the default
 * channel-converter
 */
template <typename DstP, typename ViewTypes> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename channel_converted_view_type<dynamic_buffer_view<ViewTypes>, DstP>::type
channel_converted_view (const dynamic_buffer_view<ViewTypes>& src)
{ 
    return apply_operation (
	src, detail::channel_converted_view_fn <
	    DstP,typename channel_converted_view_type<
		dynamic_buffer_view<ViewTypes>, DstP>::type >()); 
}

/**
 * \ingroup BufferViewTransformationsChannelConvert
 * \brief overload of generic channel_converted_view with user defined
 * channel-converter
 *
 * These are workarounds for GCC 3.4, which thinks
 * channel_converted_view is ambiguous with the same method for
 * templated views (in gil/buffer_view_factory.hpp)
 */
template <typename DstP, typename ViewTypes, typename CC> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename channel_converted_view_type<
    dynamic_buffer_view<ViewTypes>, DstP, CC>::type
dynamic_channel_converted_view (const dynamic_buffer_view<ViewTypes>& src, CC cc)
{ 
    return apply_operation (
	src, detail::channel_converted_view_fn<
	    DstP,typename channel_converted_view_type<
		dynamic_buffer_view<ViewTypes>, DstP, CC>::type >()); 
}

/**
 * \ingroup BufferViewTransformationsChannelConvert
 * \brief overload of generic channel_converted_view with the default
 * channel-converter
 *
 * These are workarounds for GCC 3.4, which thinks
 * channel_converted_view is ambiguous with the same method for
 * templated views (in sound/buffer_view_factory.hpp)
 */
template <typename DstP, typename ViewTypes> inline
// Models MPL Random Access Container of models of BufferViewConcept
typename channel_converted_view_type<dynamic_buffer_view<ViewTypes>, DstP>::type
dynamic_channel_converted_view (const dynamic_buffer_view<ViewTypes>& src)
{ 
    return apply_operation (
	src, detail::channel_converted_view_fn<
	    DstP, typename channel_converted_view_type<
		dynamic_buffer_view<ViewTypes>, DstP>::type >()); 
}

/** \} */

} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_BUFFER_VIEW_FACTORY */
