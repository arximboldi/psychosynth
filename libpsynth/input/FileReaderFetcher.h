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

#include <libpsynth/input/FileReader.h>
#include <libpsynth/common/thread.h>
#include <libpsynth/common/mutex.h>
#include <libpsynth/common/ring_audio_buffer.h>

namespace psynth
{

class FileReaderFetcher : public FileReader,
			  public self_thread
{
    /* High default values for lower number of seeks when reading backwards. */
    static const int DEFAULT_BUFFER_SIZE = 16384;
    static const int DEFAULT_THRESHOLD  = 4096; 

    FileReader* m_reader;
    int m_buffer_size;
    int m_threshold;
    audio_buffer m_tmp_buffer;
    ring_audio_buffer m_buffer;
    ring_audio_buffer::read_ptr m_read_ptr;

    bool m_backwards;
    int m_read_pos;
    int m_new_read_pos;

    bool m_finished;
    
    mutex m_reader_lock;
    mutex m_buffer_lock;
    condition m_cond;

    void run();
    
public:
    FileReaderFetcher(FileReader* reader = NULL,
		      int buffer_size = DEFAULT_BUFFER_SIZE,
		      int threshold  = DEFAULT_THRESHOLD);

    void setFileReader(FileReader* reader) {
	m_reader = reader;
    }

    FileReader* getFileReader() {
	return m_reader;
    }

    bool getBackwards() const {
	return m_backwards;
    }
    
    void setBackwards(bool backwards);
    
    void open(const char* file);
    void seek(size_t pos);
    void forceSeek(size_t pos);
    int read(audio_buffer& buf, int n_samples);
    void close();
    void finish();
};

} /* namespace psynth */

#endif /* PSYNTH_FILEREADERFETCHER_H */

