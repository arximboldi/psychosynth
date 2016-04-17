/**
 *  Time-stamp:  <2011-06-27 22:54:18 raskolnikov>
 *
 *  @file        triple_buffer.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Fri Jun 10 18:27:23 2011
 *
 *  @brief Triple buffer implementation.
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

#ifndef PSYNTH_GRAPH_TRIPLE_BUFFER_TPP_
#define PSYNTH_GRAPH_TRIPLE_BUFFER_TPP_

#include <psynth/new_graph/triple_buffer.hpp>

namespace psynth
{
namespace graph
{

template <class B, class G>
double_buffer<B, G>::double_buffer ()
    : _storage ({ B (), B () })
{
    _front = & _storage [0];
    _back  = & _storage [1];
}

template <class B, class G>
double_buffer<B, G>::double_buffer (const B& other)
    : _storage (other, other)
{
    _front = & std::get<0> (_storage);
    _back  = & std::get<1> (_storage);
}

template <class B, class G>
double_buffer<B, G>::double_buffer (B&& x1, B&& x2)
    : _storage (std::move (x1), std::move (x2))
{
    _front = & std::get<0> (_storage);
    _back  = & std::get<1> (_storage);
}

template <class B, class G>
bool double_buffer<B, G>::flip_back ()
{
    auto g = _back_lock.default_guard ();
    if (g.owns_lock ())
    {
        std::swap (_back, _front);
        return true;
    }
    return false;
}

template <class B, class GB, class GL>
triple_buffer<B, GB, GL>::triple_buffer ()
{
    this->_front = & _local_storage;
}

template <class B, class GB, class GL>
triple_buffer<B, GB, GL>::triple_buffer (const B& other)
    : double_buffer <B, GB> (other)
    , _local_storage (other)
{
    _local = & _local_storage;
}

template <class B, class GB, class GL>
triple_buffer<B, GB, GL>::triple_buffer (B&& x1, B&& x2, B&& x3)
    : double_buffer <B, GB> (std::move (x1), std::move (x2))
    , _local_storage (std::move (x3))
{
    _local = & _local_storage;
}

template <class B, class GB, class GL>
bool triple_buffer<B, GB, GL>::flip_local ()
{
    auto g = _local_lock.default_guard ();
    if (g.owns_lock ())
    {
        std::swap (_local, this->_front);
        return true;
    }
    return false;
}

} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_TRIPLE_BUFFER_TPP_ */
