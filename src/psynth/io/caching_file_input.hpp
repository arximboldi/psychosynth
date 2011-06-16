/**
 *  Time-stamp:  <2011-06-11 22:28:54 raskolnikov>
 *
 *  @file        caching_file_reader.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 17:57:15 2011
 *
 *  Caching file reader that prefetches in a separate thread the data
 *  such that the operation happens fast and can be done in the
 *  realtime thread.
 *
 *  @todo Make sure the fetcher does not overrun!
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

#ifndef PSYNTH_IO_CACHING_FILE_READER_H_
#define PSYNTH_IO_CACHING_FILE_READER_H_

#include <type_traits>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <boost/pointee.hpp>

#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/ring_buffer.hpp>
#include <psynth/io/file_common.hpp>
#include <psynth/io/async_base.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class Range,
          class InputPtr> // Models file_input_base
class caching_file_input_impl : public file_input_base<Range>
                              , private boost::noncopyable
{
public:
    typedef Range range;

    typedef typename boost::pointee<InputPtr>::type  input_type;
    typedef typename input_type::range               input_range;

    /*
     * High default values for lower number of seeks when reading
     * backwards which is in our default usecase.
     */
    static constexpr std::size_t default_chunk_size  = 1024;
    static constexpr std::size_t default_buffer_size = 16384;
    static constexpr std::size_t default_threshold   = 4096; 
   
    caching_file_input_impl (
        InputPtr    input       = 0,
        std::size_t chunk_size  = default_chunk_size,
        std::size_t buffer_size = default_buffer_size,
        std::size_t threshold   = default_threshold);

    caching_file_input_impl (
        caching_file_input_impl&& other) = default;

    ~caching_file_input_impl ();
    
#if 0 // TODO
    caching_file_input_impl& operator= (
        caching_file_input_impl&& other) = default;
#endif
    
    void start ();
    void stop ();

    std::size_t frame_rate () const
    { return _input->frame_rate (); }

    std::size_t length () const
    { return _input->length (); }

    const input_type& input () const
    { return *_input; }
    
    bool is_backwards () const
    { return _backwards; }
    
    void set_backwards (bool backwards);

    /** @todo Synchronization? */
    void set_chunk_size (std::size_t cs)
    { _chunk_size = cs; }

    void soft_seek (std::size_t pos);
    std::size_t seek (std::ptrdiff_t pos, seek_dir dir);
    
    std::size_t take (const range& buf)
    { return this->template take<range> (buf); }

    template <class Range2>
    std::size_t take (const Range2& buf);

protected:
    InputPtr      _input;
    
public:
    typedef typename sound::buffer_from_range<input_range>::type
    input_buffer_type;

    typedef typename sound::buffer_from_range<range>::type
    buffer_type;
    
    typedef sound::ring_buffer<buffer_type> ring_buffer_type;

    void set_input (InputPtr ptr);

    void do_seek (std::ptrdiff_t offst, seek_dir dir);
    
    std::size_t   _chunk_size; // TODO: This substitutes
                               // get_info().block_size, but there
                               // might be problems.
    std::size_t   _buffer_size;
    std::size_t   _threshold;
    
    input_buffer_type                   _tmp_buffer;
    ring_buffer_type                    _buffer;
    typename ring_buffer_type::range&   _range;
    typename ring_buffer_type::position _read_ptr;
    
    bool           _backwards;
    std::ptrdiff_t _read_pos;
    std::ptrdiff_t _new_read_pos;

    std::atomic<bool> _finished;

    std::thread             _thread;
    std::mutex              _input_mutex;
    std::mutex              _buffer_mutex;
    std::condition_variable _cond;
    
    void run ();
};

} /* namespace detail */

template <class Range, class InputPtr>
class caching_file_input_adapter :
        public detail::caching_file_input_impl<Range, InputPtr>
{
public:
    typedef detail::caching_file_input_impl<Range, InputPtr> base;
    
    caching_file_input_adapter (
        InputPtr    input       = 0,
        std::size_t chunk_size  = base::default_chunk_size,
        std::size_t buffer_size = base::default_buffer_size,
        std::size_t threshold   = base::default_threshold)
        : base (input, chunk_size, buffer_size, threshold)
    {}

    caching_file_input_adapter (caching_file_input_adapter&& other)
        : base (std::move (other))
    {}

    void set_input (InputPtr input)
    { base::set_input (input); }
};

} /* namespace io */
} /* namespace psynth */

#include <psynth/io/caching_file_input.tpp>

#endif /* PSYNTH_IO_CACHING_FILE_INPUT_H_ */

