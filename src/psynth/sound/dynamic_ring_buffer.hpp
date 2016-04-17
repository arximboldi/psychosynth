/**
 *  Time-stamp:  <2011-03-08 20:09:02 raskolnikov>
 *
 *  @file        dynamic_ring_buffer.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Nov 10 14:11:54 2010
 *
 *  Dynamic ring buffer.
 */

#ifndef PSYNTH_SOUND_DYNAMIC_RING_BUFFER_H_
#define PSYNTH_SOUND_DYNAMIC_RING_BUFFER_H_

#include <psynth/sound/ring_buffer.hpp>

#include <psynth/sound/dynamic_buffer.hpp>
#include <psynth/sound/dynamic_ring_buffer_range.hpp>

namespace psynth
{
namespace sound
{

template <class DBuffer>
class dynamic_ring_buffer : public ring_buffer_base<
    DBuffer, dynamic_ring_buffer_range>
{
    typedef ring_buffer_base<DBuffer, dynamic_ring_buffer_range> parent_type;

public:
    typedef typename parent_type::range_base       range;
    typedef typename parent_type::const_range_base const_range;

    /**
     * Create with size and optional initial value and
     * alignment.
     * @todo When will GCC support inheriting constructors? :(
     */
    explicit dynamic_ring_buffer ()
	: parent_type ()
    {}

    explicit dynamic_ring_buffer (const DBuffer& buf)
	: parent_type (buf)
    {}

    template <typename F2, bool IP2, typename Alloc2>
    explicit dynamic_ring_buffer (const buffer<F2, IP2, Alloc2>& buf)
	: parent_type (buf)
    {}

    dynamic_ring_buffer& operator= (const dynamic_ring_buffer& buf)
    {
	parent_type::operator= (buf);
        return *this;
    }

    template <class DBuffer2>
    dynamic_ring_buffer& operator= (const dynamic_ring_buffer<DBuffer2>& buf)
    {
	parent_type::operator= (buf);
        return *this;
    }

private:
    template <typename B> friend
    const typename dynamic_ring_buffer<B>::range&
    range (dynamic_ring_buffer<B>& buf);

    template <typename B> friend
    const typename dynamic_ring_buffer<B>::const_range
    const_range (const dynamic_ring_buffer<B>& buf);
};


/*
 *
 *  @todo BufferConcept ?
 *
 */
template <typename B>
const typename dynamic_ring_buffer<B>::range&
range (dynamic_ring_buffer<B>& buf)
{
    return buf._range;
}

template <typename B>
const typename dynamic_ring_buffer<B>::const_range
const_range (const dynamic_ring_buffer<B>& buf)
{
    return buf._range;
}


} /* namespace sound */
} /* namespace psynth */

#endif /* PSYNTH_SOUND_DYNAMIC_RING_BUFFER_H_ */
