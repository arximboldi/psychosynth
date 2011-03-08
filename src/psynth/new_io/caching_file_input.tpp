/**
 *  Time-stamp:  <2011-03-08 19:05:57 raskolnikov>
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

namespace psynth
{
namespace io
{

namespace detail
{

template <class R, class I>
void caching_file_input_impl<R, I>::start ()
{
    if (_thread.get_id () == std::thread::id ())
        _thread = std::thread (
            std::bind (&caching_file_input_impl::run, this));
}

caching_file_input_impl<R, I>::caching_file_input_impl (InputPtr    input,
                                                        std::size_t buffer_size,
                                                        std::size_t threshold)
    : _input (input)
    , _buffer_size (buffer_size)
    , _threshold (threshold)
    , _tmp_buffer (threshold)
    , _buffer (_buffer_size)
    , _read_ptr (_buffer.begin())
    , _backwards(false)
    , _read_pos(0)
    , _new_read_pos(-1)
    , _finished(false)
{
}

void caching_file_input_impl<R, I>::set_backwards (bool backwards)
{
    std::size_t old_avail;
    std::size_t new_avail;
    std::size_t diff_avail;
    
    if (_backwards != backwards)
    {
	std::mutex::scoped_lock buffer_lock (_buffer_mutex);
        std::mutex::scoped_lock input_lock  (_input_mutex);

	old_avail = _buffer.available (_read_ptr);
	
	_backwards = backwards;
	_buffer.set_backwards ();
	_read_ptr = _buffer.sync (_read_ptr);

	new_avail = _buffer.available (_read_ptr);
	diff_avail = old_avail - new_avail;

	/* Fix reading position. */
	if (_backwards) {
	    _new_read_pos -= diff_avail;
	    if (_new_read_pos < 0)
		_new_read_pos += _buffer_size;
	} else {
	    _new_read_pos += diff_avail;
	    if (_new_read_pos >= _buffer_size)
		_new_read_pos -= _buffer_size;
	}
    }
}

void caching_file_input_impl<R, I>::soft_seek (std::size_t pos)
{
    _new_read_pos = pos;
}

std::size_t caching_file_input_impl<R, I>::seek (
    std::ptrdiff_t offset, seek_dir dir)
{
    std::mutex::scoped_lock buffer_lock (_buffer_mutex);
    {
        std::mutex::scoped_lock input_lock  (_input_mutex);

        _read_pos = _input->seek (pos, dir);
        _read_ptr = _buffer.end ();
    }
    _cond.notify_all();
}

/**
 * @todo Fix potential performance lost with the std::reverse, think
 * of an override in psynth::sound.
 */
std::size_t caching_file_input_impl<R, I>::take (range& buf)
{
    std::size_t nsamples = buf.size ();
    std::size_t nread;

    {
	std::mutex::scoped_lock lock (_buffer_mutex);
	
	while (_buffer.available (_read_ptr) == 0)
	    _cond.wait (lock);
	
	nread = std::min (n_samples, _buffer.available (_read_ptr));

        if (nread)
        {
	    _buffer.read (_read_ptr, buf);
	    
	    if (_buffer.availible (_read_ptr) < _threshold)
		_cond.notify_all ();

	    lock.unlock ();
	    if (_backwards)
                std::reverse (buf.begin (), buf.begin () + nread);
	}
    }
    
    return nread;
}

void caching_file_input_impl<R, I>::run ()
{
    std::size_t nread;
    std::size_t must_read;
    
    do {
	/* Read the data. */
	{
	    nread = 0;
	    boost::mutex::scoped_lock lock (_input_mutex);
	
	    if (_input)
            {
		must_read = _threshold;

		/* Do we have to seek? */
		if (_new_read_pos >= 0) {
		    _read_pos = _new_read_pos;
		    _new_read_pos = -1;
		    if (!_backwards)
			_input->seek(_read_pos);
		}

		/* Backwards reading needs seeking. */
		if (_backwards) {
		    if (_read_pos == 0)
			_read_pos = get_info().block_size - must_read;
		    else if (must_read > _read_pos) {
			must_read = _read_pos;
			_read_pos = 0;
		    } else
			_read_pos -= must_read;
		    _input->seek(_read_pos);
		}

                auto block = sound::sub_range (_tmp_buffer, 0, must_read);
		nread = _input->take ();

		/* Check wether whe have finished reading and loop. */
		if (!nread && !_backwards) {
		    _read_pos += nread;
		    if (nread == 0)
			_new_read_pos = 0;
		}
	    }
	}
	
	/* Add it to the buffer. */
	if (nread) {
	    boost::mutex::scoped_lock lock (_buffer_mutex);
            auto block = sound::sub_range (_tmp_buffer, 0, nread);
	    _buffer.write (block);
	    _cond.notify_all ();
	}

	/* Wait until more data is needed. */
	{
	    boost::mutex::scoped_lock lock (_buffer_mutex);
	    while (!_finished &&
		   (_buffer.availible (_read_ptr) > _threshold || !_input))
		_cond.wait (lock);
	}	
    } while (!_finished);
}

void caching_file_input_impl<R, I>::stop ()
{
    _finished = true;
    _cond.notify_all ();
    _thread.join ();
    _finished = false;
}

/** @todo use this to implement set_input */
void caching_file_input_impl<R, I>::set_input (I input)
{
    std::mutex::scoped_lock buffer_lock (_buffer_mutex);
    {
        std::mutex::scoped_lock input_lock  (_input_mutex);
        
        _input = input;
    }
    _cond.notify_all();
}

} /* namespace detail */

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_CACHING_FILE_INPUT_TPP_ */
