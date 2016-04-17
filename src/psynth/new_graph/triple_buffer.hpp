/**
 *  Time-stamp:  <2011-06-13 17:45:55 raskolnikov>
 *
 *  @file        triple_buffer.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 18:04:48 2011
 *
 *  @brief Triple buffer type.
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

#ifndef PSYNTH_GRAPH_TRIPLE_BUFFER_HPP_
#define PSYNTH_GRAPH_TRIPLE_BUFFER_HPP_

#include <cassert>
#include <tuple>
#include <psynth/base/util.hpp>

namespace psynth
{
namespace graph
{

template <class Buffer, class GuardPolicy>
class double_buffer : private boost::noncopyable
{
public:
    typedef Buffer buffer_type;

    double_buffer ();
    double_buffer (const Buffer&);
    double_buffer (Buffer&&, Buffer&&);

    GuardPolicy& back_policy () const
    { return _back_lock; };

    buffer_type& front ()
    { return *_front; }

    buffer_type& back ()
    { return *_back; }

    const buffer_type& front () const
    { return *_front; }

    const buffer_type& back () const
    { return *_back; }

    bool flip_back ();

protected:
    std::tuple<buffer_type, buffer_type> _storage;

    buffer_type* _front;
    buffer_type* _back;
    mutable GuardPolicy  _back_lock;
};


template <class Buffer, class BackGuardPolicy, class LocalGuardPolicy>
class triple_buffer : public double_buffer<Buffer, BackGuardPolicy>
{
public:
    typedef std::unique_lock<std::mutex> guard;
    typedef Buffer buffer_type;

    triple_buffer ();
    triple_buffer (const Buffer&);
    triple_buffer (Buffer&&, Buffer&&, Buffer&&);

    LocalGuardPolicy& local_policy () const
    { return _local_lock; };

    bool flip_local ();

    buffer_type& local ()
    { return *_local; }

    const buffer_type& local () const
    { return *_local; }

protected:
    buffer_type  _local_storage;
    buffer_type* _local;
    mutable LocalGuardPolicy  _local_lock;
};

} /* namespace graph */
} /* namespace psynth */

#include <psynth/new_graph/triple_buffer.tpp>

#endif /* PSYNTH_GRAPH_TRIPLE_BUFFER_HPP_ */
