/**
 *  Time-stamp:  <2011-03-09 01:12:41 raskolnikov>
 *
 *  @file        oss_raw_output.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Mar  4 20:00:51 2011
 *
 *  OSS raw output interface.
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

#ifndef PSYNTH_IO_OSS_RAW_OUTPUT_H_
#define PSYNTH_IO_OSS_RAW_OUTPUT_H_

#include <psynth/new_io/exception.hpp>
#include <psynth/new_io/thread_async.hpp>

namespace psynth
{
namespace io
{

PSYNTH_DECLARE_ERROR (error, oss_error);
PSYNTH_DECLARE_ERROR (oss_error, oss_open_error);
PSYNTH_DECLARE_ERROR (oss_error, oss_param_error);

class oss_raw_output : public thread_async,
                       public boost::noncopyable
{
public:
    /**
     * We do not check that sample_size and format match.
     */
    oss_raw_output (const char* device,
                    int         format,
                    int         sample_size,
                    int         buffer_size,
                    bool        interleaved,
                    int         rate,
                    int         channels);
    
    ~oss_raw_output ();

    std::size_t put_i (void*  data, std::size_t frames);
    std::size_t put_n (void** data, std::size_t frames);

    std::size_t buffer_size () const
    { return _buffer_size; }
    
private:
    void iterate ();

    int         _handle;
    std::size_t _frame_size;
    std::size_t _buffer_size;
};

} /* namespace io */
} /* namespace psynth */

#endif /* PSYNTH_IO_OSS_RAW_OUTPUT_H_ */

