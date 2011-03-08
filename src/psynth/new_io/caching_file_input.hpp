/**
 *  Time-stamp:  <2011-03-08 18:46:47 raskolnikov>
 *
 *  @file        caching_file_reader.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 17:57:15 2011
 *
 *  Caching file reader that prefetches in a separate thread the data
 *  such that the operation happens fast and can be done in the
 *  realtime thread.
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

#include <psynth/sound/metafunctions.hpp>
#include <psynth/sound/ring_buffer.hpp>
#include <psynth/new_io/file_common.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class Range,
          class InputPtr> // Models file_input_base
class caching_file_input_impl : public file_input_base
{
public:
    typedef Range                       range;
    typedef std::decay<InputPtr>::type  input;
    typedef typename output::range      input_range;

    /*
     * High default values for lower number of seeks when reading
     * backwards which is in our default usecase.
     */
    static const std::size_t DEFAULT_BUFFER_SIZE = 16384;
    static const std::size_t DEFAULT_THRESHOLD   = 4096; 
   
    caching_file_input_impl (
        InputPtr    input = 0,
        std::size_t buffer_size = DEFAULT_BUFFER_SIZE,
        std::size_t threshold   = DEFAULT_THRESHOLD);

    void start ();
    void stop ();

    const InputPtr input () const
    { return _input; }
    
    bool is_backwards () const
    { return _backwards; }
    
    void set_backwards (bool backwards);
    
    void soft_seek (std::size_t pos);
    std::size_t seek (std::ptrdiff_t pos, seek_dir dir);
    
    std::size_t take (const range& buf);
    
private:
    typedef sound::buffer_type_from_range<output_range>::type buffer_type;
    typedef sound::ring_buffer<buffer_type> ring_buffer_type;

    InputPtr      _input;
    std::size_t   _buffer_size;
    std::size_t   _threshold;
    
    buffer_type                _tmp_buffer;
    ring_buffer_type           _buffer;
    ring_buffer_type::position _read_ptr;
    
    bool        _backwards;
    std::size_t _read_pos;
    std::size_t _new_read_pos;

    bool _finished;

    std::thread             _thread;
    std::mutex              _input_mutex;
    std::mutex              _buffer_mutex;
    std::condition_variable _cond;

    void run ();
};

} /* namespace detail */

template <class Range, class InputPtr>
class caching_file_input_adapter : public detail::caching_file_input_impl
{
public:
    void set_input (InputPtr input)
    { _input = input; }
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_CACHING_FILE_INPUT_H_ */

