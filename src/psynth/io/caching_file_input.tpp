/**
 *  Time-stamp:  <2011-03-23 00:38:28 raskolnikov>
 *
 *  @file        caching_file_reader.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Mar  8 17:58:35 2011
 *
 *  Caching file reader that prefetches data. Implementation.
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

#ifndef PSYNTH_IO_CACHING_FILE_READER_TPP_
#define PSYNTH_IO_CACHING_FILE_READER_TPP_

#include <psynth/io/caching_file_input.hpp>

namespace psynth
{
namespace io
{

namespace detail
{

template <class R, class I>
caching_file_input_impl<R, I>::~caching_file_input_impl ()
{
    // TODO!! 
    //if (!_finished)
    //    stop ();
}

template <class R, class I>
void caching_file_input_impl<R, I>::start ()
{
    // TODO: Error checking
    if (_thread.get_id () == std::thread::id ())
    {
        _finished = false;
        _thread = std::thread (
            std::bind (&caching_file_input_impl::run, this));
    }
}

template <class R, class I>
caching_file_input_impl<R, I>::caching_file_input_impl (
    I           input,
    std::size_t chunk_size,
    std::size_t buffer_size,
    std::size_t threshold)
    : _input (input)
    , _chunk_size (chunk_size)
    , _buffer_size (buffer_size)
    , _threshold (threshold)
    , _tmp_buffer (threshold)
    , _buffer (buffer_size)
    , _range (sound::range (_buffer))
    , _read_ptr (_range.end_pos ())
    , _backwards (false)
    , _read_pos (0)
    , _new_read_pos (-1)
    , _finished (true)
{
    assert (chunk_size < threshold && threshold < buffer_size);
}

template <class R, class I>
void caching_file_input_impl<R, I>::set_backwards (bool backwards)
{
    if (_backwards != backwards)
    {
 	std::unique_lock<std::mutex> buffer_lock (_buffer_mutex);
        std::unique_lock<std::mutex> input_lock  (_input_mutex);

        std::size_t old_avail = _range.available (_read_ptr);
	
	_backwards = backwards;
	_range.set_backwards ();
	_read_ptr = _range.sync (_read_ptr);
        
        std::size_t new_avail  = _range.available (_read_ptr);
        std::size_t diff_avail = old_avail - new_avail;
        
	/* Fix reading position. */
	if (_backwards) {
	    _new_read_pos -= diff_avail;
	    if (_new_read_pos < 0)
		_new_read_pos += _chunk_size;
	} else {
	    _new_read_pos += diff_avail;
	    if (_new_read_pos >= std::ptrdiff_t (_chunk_size))
		_new_read_pos -= _chunk_size;
	}
    }
}

template <class R, class I>
void caching_file_input_impl<R, I>::soft_seek (std::size_t pos)
{
    _new_read_pos = pos;
}

template <class R, class I>
std::size_t caching_file_input_impl<R, I>::seek (
    std::ptrdiff_t offset, seek_dir dir)
{
    std::unique_lock<std::mutex> buffer_lock (_buffer_mutex);
    {
        std::unique_lock<std::mutex> input_lock  (_input_mutex);

        _read_pos = _input->seek (offset, dir);
        _read_ptr = _range.end_pos ();
    }
    _cond.notify_all();
    return _read_pos;
}

/**
 * @todo Fix potential performance lost with the std::reverse, think
 * of an override in psynth::sound.
 */
template <class R, class I>
template <typename Range>
std::size_t caching_file_input_impl<R, I>::take (const Range& buf)
{
    std::size_t nsamples = buf.size ();
    std::size_t nread;

    {
	std::unique_lock<std::mutex> lock (_buffer_mutex);

	while (_range.available (_read_ptr) <= 0)
            _cond.wait (lock);
        
	nread = std::min<std::size_t> (nsamples, _range.available (_read_ptr));

        if (nread)
        {
            _range.read_and_convert (_read_ptr, sub_range (buf, 0, nread));
            
            // TODO: Why is ring_buffer_range::size_type signed?
	    if ((std::size_t) _range.available (_read_ptr) < _threshold)
		_cond.notify_all ();

	    lock.unlock ();
	    if (_backwards)
                std::reverse (buf.begin (), buf.begin () + nread);
	}
    }
    
    return nread;
}

template <class R, class I>
void caching_file_input_impl<R, I>::do_seek (std::ptrdiff_t offst,
                                             seek_dir dir)
{
    std::cout << "Seek: " << offst << std::endl;
    try
    {
        _input->seek (offst, dir);
    } catch (file_seek_error&) {
        /* We just rely on the next 'take'
         * failing. */
    }
}

template <class R, class I>
void caching_file_input_impl<R, I>::run ()
{
    std::size_t nread;
    std::size_t must_read;

    do {
	/* Read the data. */
	{
	    nread = 0;
	    std::unique_lock<std::mutex> lock (_input_mutex);
	
	    if (_input)
            {
		must_read = _threshold;

		/* Do we have to seek? */
		if (_new_read_pos >= 0) {
		    _read_pos = _new_read_pos;
		    _new_read_pos = -1;
		    if (!_backwards)
                        do_seek (_read_pos, seek_dir::beg);
                }

		/* Backwards reading needs seeking. */
		if (_backwards) {
                    if (_read_pos == 0)
			_read_pos = _chunk_size - must_read;
		    else if (must_read > _read_pos)
                    {
			must_read = _read_pos;
			_read_pos = 0;
		    }
                    else
			_read_pos -= must_read;
                    do_seek (_read_pos, seek_dir::beg);
                }
                
                auto block = sound::sub_range (
                    sound::range (_tmp_buffer), 0, must_read);
		nread = _input->take (block);
                
		/* Check wether whe have finished reading and loop. */
		if (!nread && !_backwards) {
		    _read_pos += nread;
		    if (nread == 0)
			_new_read_pos = 0;
		}
	    }
	}
	
	/* Add it to the buffer. */
	if (nread)
        {
	    std::unique_lock<std::mutex> lock (_buffer_mutex);
            auto block = sound::sub_range (sound::range (_tmp_buffer), 0, nread);
            _range.write_and_convert (block);
            _cond.notify_all ();
	}

	/* Wait until more data is needed. */
	{
	    std::unique_lock<std::mutex> lock (_buffer_mutex);
	    while (!_finished &&
		   ((std::size_t) _range.available (_read_ptr) > _threshold
                    || !_input))
		_cond.wait (lock);
	}
    } while (!_finished);
}

template <class R, class I>
void caching_file_input_impl<R, I>::stop ()
{
    _finished = true;
    _cond.notify_all ();
    _thread.join ();
    _finished = false;
}

/** @todo use this to implement set_input */
template <class R, class I>
void caching_file_input_impl<R, I>::set_input (I input)
{
    std::unique_lock<std::mutex> buffer_lock (_buffer_mutex);
    {
        std::unique_lock<std::mutex> input_lock  (_input_mutex);
        _input = input;
    }
    _cond.notify_all();
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_CACHING_FILE_INPUT_TPP_ */
