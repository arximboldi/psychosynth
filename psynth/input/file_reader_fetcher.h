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

#ifndef PSYNTH_FILEREADERFETCHER_H
#define PSYNTH_FILEREADERFETCHER_H

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include <libpsynth/input/file_reader.h>
#include <libpsynth/common/ring_audio_buffer.h>

namespace psynth
{

class file_reader_fetcher : public file_reader
{
    /* High default values for lower number of seeks when reading backwards. */
    static const int DEFAULT_BUFFER_SIZE = 16384;
    static const int DEFAULT_THRESHOLD  = 4096; 

    file_reader* m_reader;
    int m_buffer_size;
    int m_threshold;
    audio_buffer m_tmp_buffer;
    ring_audio_buffer m_buffer;
    ring_audio_buffer::read_ptr m_read_ptr;

    bool m_backwards;
    int m_read_pos;
    int m_new_read_pos;

    bool m_finished;

    boost::thread m_thread;
    boost::mutex m_reader_mutex;
    boost::mutex m_buffer_mutex;
    boost::condition_variable m_cond;

    void run ();

    void stop () {
	m_thread.join ();
    }
    
public:
    file_reader_fetcher (file_reader* reader = NULL,
			int buffer_size = DEFAULT_BUFFER_SIZE,
			int threshold  = DEFAULT_THRESHOLD);

    void start () {
	if (m_thread.get_id () == boost::thread::id ())
	    m_thread = boost::thread (boost::bind (&file_reader_fetcher::run, this));
    }
    
    void set_file_reader (file_reader* reader) {
	m_reader = reader;
    }

    file_reader* get_file_reader () {
	return m_reader;
    }

    bool get_backwards () const {
	return m_backwards;
    }
    
    void set_backwards (bool backwards);
    
    void open (const std::string& file);
    void seek (size_t pos);
    void force_seek (size_t pos);
    int read (audio_buffer& buf, int n_samples);
    void close ();
    void finish ();
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADERFETCHER_H */

