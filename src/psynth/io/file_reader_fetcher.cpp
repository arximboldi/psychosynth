/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
 *                                                                         *
 *   This program is free software: you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 *                                                                         *
 ***************************************************************************/

#include <iostream>
#include "io/file_reader_fetcher.hpp"

using namespace std;

namespace psynth
{

file_reader_fetcher::file_reader_fetcher (file_reader* reader,
					  size_t buffer_size,
					  size_t threshold)
    : m_reader(reader)
    , m_buffer_size(buffer_size)
    , m_threshold(threshold)
    , m_tmp_buffer(audio_info(), m_threshold)
    , m_buffer(audio_info(), m_buffer_size)
    , m_read_ptr(m_buffer.begin())
    , m_backwards(false)
    , m_read_pos(0)
    , m_new_read_pos(-1)
    , m_finished(false)
{
}

void file_reader_fetcher::set_backwards (bool backwards)
{
    size_t old_avail;
    size_t new_avail;
    size_t diff_avail;
    
    if (m_backwards != backwards) {
	m_buffer_mutex.lock();
	m_reader_mutex.lock();

	old_avail = m_buffer.availible(m_read_ptr);
	
	m_backwards = backwards;
	m_buffer.backwards();
	m_read_ptr = m_buffer.sync(m_read_ptr);

	new_avail = m_buffer.availible(m_read_ptr);
	diff_avail = old_avail - new_avail;

	/* Fix reading position. */
	if (m_backwards) {
	    m_new_read_pos -= diff_avail;
	    if (m_new_read_pos < 0)
		m_new_read_pos += get_info().block_size;
	} else {
	    m_new_read_pos += diff_avail;
	    if (m_new_read_pos >= get_info().block_size)
		m_new_read_pos -= get_info().block_size;
	}
	
	m_buffer_mutex.unlock();
	m_reader_mutex.unlock();
    }
}

void file_reader_fetcher::open (const std::string& file)
{
    m_buffer_mutex.lock();
    m_reader_mutex.lock();
    
    m_reader->open (file);

    set_is_open (m_reader->is_open ());
    set_info (m_reader->get_info ());

    if (m_reader->is_open()) {
	m_tmp_buffer.set_info (get_info(), m_threshold);
	m_buffer.set_info     (get_info(), m_buffer_size);
    }
    
    m_reader_mutex.unlock();
    m_cond.notify_all();
    m_buffer_mutex.unlock();
}

void file_reader_fetcher::seek (size_t pos)
{
    m_new_read_pos = pos;
}

void file_reader_fetcher::force_seek (size_t pos)
{
    m_buffer_mutex.lock();
    m_reader_mutex.lock();

    m_read_pos = pos;
    m_reader->seek(pos);

    m_read_ptr = m_buffer.end();
        
    m_reader_mutex.unlock();
    m_cond.notify_all();
    m_buffer_mutex.unlock();
}

size_t file_reader_fetcher::read (audio_buffer& buf, size_t n_samples)
{
    size_t n_read;

    {
	unique_lock<mutex> lock (m_buffer_mutex);
	
	while (m_buffer.availible(m_read_ptr) == 0)
	    m_cond.wait (lock);
	
	n_read = min(n_samples, m_buffer.availible (m_read_ptr));
	if (n_read) {
	    m_buffer.read (m_read_ptr, buf, n_samples);
	    
	    if (m_buffer.availible(m_read_ptr) < m_threshold)
		m_cond.notify_all();

	    lock.unlock();
	    if (m_backwards)
		buf.reverse(n_read);
	}
    }
    
    return n_read;
}

void file_reader_fetcher::close ()
{
    unique_lock<mutex> lock (m_reader_mutex);

    m_reader->close ();
    set_is_open (m_reader->is_open ());
}

void file_reader_fetcher::run ()
{
    size_t n_read;
    size_t must_read;
    
    do {
	/* Read the data. */
	{
	    n_read = 0;
	    unique_lock<mutex> lock (m_reader_mutex);
	
	    if (m_reader->is_open ()) {
		must_read = m_threshold;

		/* Do we have to seek? */
		if (m_new_read_pos >= 0) {
		    m_read_pos = m_new_read_pos;
		    m_new_read_pos = -1;
		    if (!m_backwards)
			m_reader->seek(m_read_pos);
		}

		/* Backwards reading needs seeking. */
		if (m_backwards) {
		    if (m_read_pos == 0)
			m_read_pos = get_info().block_size - must_read;
		    else if (must_read > m_read_pos) {
			must_read = m_read_pos;
			m_read_pos = 0;
		    } else
			m_read_pos -= must_read;
		    m_reader->seek(m_read_pos);
		}

		n_read = m_reader->read(m_tmp_buffer, must_read);

		/* Check wether whe have finished reading and loop. */
		if (!n_read && !m_backwards) {
		    m_read_pos += n_read;
		    if (n_read == 0)
			m_new_read_pos = 0;
		}
	    }
	}
	
	/* Add it to the buffer. */
	if (n_read) {
	    unique_lock<mutex> lock (m_buffer_mutex);
	    m_buffer.write (m_tmp_buffer, n_read);
	    m_cond.notify_all ();
	}

	/* Wait until more data is needed. */
	{
	    unique_lock<mutex> lock (m_buffer_mutex);
	    while (!m_finished &&
		   (m_buffer.availible (m_read_ptr) > m_threshold || !is_open()))
		m_cond.wait (lock);
	}	
    } while (!m_finished);
}

void file_reader_fetcher::finish ()
{
    m_finished = true;
    m_cond.notify_all ();
    m_thread.join ();
    m_finished = false;
}

} /* namespace psynth */
