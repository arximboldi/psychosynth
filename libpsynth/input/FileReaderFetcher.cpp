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
#include "input/FileReaderFetcher.h"

using namespace std;

namespace psynth
{

FileReaderFetcher::FileReaderFetcher(FileReader* reader,
				     int buffer_size,
				     int threshold) :
    m_reader(reader),
    m_buffer_size(buffer_size),
    m_threshold(threshold),
    m_tmp_buffer(AudioInfo(), m_threshold),
    m_buffer(AudioInfo(), m_buffer_size),
    m_read_ptr(m_buffer.begin()),
    m_backwards(false),
    m_read_pos(0),
    m_new_read_pos(-1),
    m_finished(false)
{
}

void FileReaderFetcher::setBackwards(bool backwards)
{
    int old_avail;
    int new_avail;
    int diff_avail;
    
    if (m_backwards != backwards) {
	m_buffer_lock.lock();
	m_reader_lock.lock();

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
		m_new_read_pos += getInfo().block_size;
	} else {
	    m_new_read_pos += diff_avail;
	    if (m_new_read_pos >= getInfo().block_size)
		m_new_read_pos -= getInfo().block_size;
	}
	
	m_buffer_lock.unlock();
	m_reader_lock.unlock();
    }
}

void FileReaderFetcher::open(const char* file)
{
    m_buffer_lock.lock();
    m_reader_lock.lock();
    
    m_reader->open(file);

    setIsOpen(m_reader->isOpen());
    setInfo(m_reader->getInfo());

    if (m_reader->isOpen()) {
	m_tmp_buffer.setInfo(getInfo(), m_threshold);
	m_buffer.setAudioInfo(getInfo(), m_buffer_size);
    }
    
    m_reader_lock.unlock();
    m_cond.broadcast();
    m_buffer_lock.unlock();
}

void FileReaderFetcher::seek(size_t pos)
{
    m_new_read_pos = pos;
}

void FileReaderFetcher::forceSeek(size_t pos)
{
    m_buffer_lock.lock();
    m_reader_lock.lock();

    m_read_pos = pos;
    m_reader->seek(pos);

    m_read_ptr = m_buffer.end();
        
    m_reader_lock.unlock();
    m_cond.broadcast();
    m_buffer_lock.unlock();
}

int FileReaderFetcher::read(AudioBuffer& buf, int n_samples)
{
    int n_read;
    
    m_buffer_lock.lock();
    while (m_buffer.availible(m_read_ptr) == 0)
	m_cond.wait(m_buffer_lock);
	
    n_read = min(n_samples, m_buffer.availible(m_read_ptr));
    if (n_read) {
	m_buffer.read(m_read_ptr, buf, n_samples);
    
	if (m_buffer.availible(m_read_ptr) < m_threshold)
	    m_cond.broadcast();
	m_buffer_lock.unlock();

	if (m_backwards)
	    buf.reverse(n_read);
    } else
      m_buffer_lock.unlock();

    return n_read;
}

void FileReaderFetcher::close()
{
    m_reader_lock.lock();

    m_reader->close();
    setIsOpen(m_reader->isOpen());

    m_reader_lock.unlock();
}

void FileReaderFetcher::run()
{
    int n_read;
    int must_read;
    do {
	/* Read the data. */
	n_read = 0;
	m_reader_lock.lock();
	if (m_reader->isOpen()) {
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
		    m_read_pos = getInfo().block_size - must_read;
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
	m_reader_lock.unlock();

	/* Add it to the buffer. */
	if (n_read) {
	    m_buffer_lock.lock();
	    m_buffer.write(m_tmp_buffer, n_read);
	    m_cond.broadcast();
	    m_buffer_lock.unlock();
	}

	/* Wait until more data is needed. */
	m_buffer_lock.lock();
	while(!m_finished &&
	      (m_buffer.availible(m_read_ptr) > m_threshold || !isOpen()))
	    m_cond.wait(m_buffer_lock);
	m_buffer_lock.unlock();
	
    } while (!m_finished);
}

void FileReaderFetcher::finish()
{
    m_finished = true;
    m_cond.broadcast();
    join();
    m_finished = false;
}

} /* namespace psynth */
