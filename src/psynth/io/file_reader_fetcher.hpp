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

#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

#include <psynth/io/file_reader.hpp>
#include <psynth/synth/ring_audio_buffer.hpp>

namespace psynth
{

class file_reader_fetcher : public file_reader
{
    /* High default values for lower number of seeks when reading backwards. */
    static const size_t DEFAULT_BUFFER_SIZE = 16384;
    static const size_t DEFAULT_THRESHOLD  = 4096; 

    file_reader* m_reader;
    size_t m_buffer_size;
    size_t m_threshold;
    audio_buffer m_tmp_buffer;
    ring_audio_buffer m_buffer;
    ring_audio_buffer::read_ptr m_read_ptr;

    bool m_backwards;
    size_t m_read_pos;
    size_t m_new_read_pos;

    bool m_finished;

    std::thread m_thread;
    std::mutex m_reader_mutex;
    std::mutex m_buffer_mutex;
    std::condition_variable m_cond;

    void run ();

    void stop () {
	m_thread.join ();
    }
    
public:
    file_reader_fetcher (file_reader* reader = NULL,
			size_t buffer_size = DEFAULT_BUFFER_SIZE,
			size_t threshold  = DEFAULT_THRESHOLD);

    void start () {
	if (m_thread.get_id () == std::thread::id ())
	    m_thread = std::thread (std::bind (&file_reader_fetcher::run, this));
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
    size_t read (audio_buffer& buf, size_t n_samples);
    void close ();
    void finish ();
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADERFETCHER_H */

