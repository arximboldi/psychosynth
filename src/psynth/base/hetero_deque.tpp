/**
 *  Time-stamp:  <2011-06-14 20:19:51 raskolnikov>
 *
 *  @file        hetero_queque.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun  8 12:33:35 2011
 *
 *  @brief Implementation of the heterogeneous deques.
 *
 *  @note The hetero deque is maintained with a doubly linked
 *  list. There is always an empty element at the end. The constant
 *  size space has a ring-buffer shape. The _back pointer points to the
 *  last actual element, thus _back == _front means that there is only
 *  one element or that the deque is empty (if this element is the
 *  null element). The empty element at the back is contiguous to the
 *  last actual element.
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

#ifndef PSYNTH_BASE_HETERO_DEQUE_TPP_
#define PSYNTH_BASE_HETERO_DEQUE_TPP_

#include <cassert>
#include <psynth/base/scope_guard.hpp>
#include <psynth/base/hetero_deque.hpp>

namespace psynth
{
namespace base
{

template <class B>
hetero_deque<B>::hetero_deque (std::size_t size)
    : _memory (size + sizeof (header))
    , _front (reinterpret_cast<header*> (&_memory [0]))
    , _back (reinterpret_cast<header*> (&_memory [0]))
{
    *_front = header { 0, 0, 0 };
}

template <class B>
hetero_deque<B>::hetero_deque (hetero_deque&& other)
    : _memory (0)
    , _front (0)
    , _back (0)
{
    std::swap (*this, other);
}

template <class B>
hetero_deque<B>& hetero_deque<B>::operator= (hetero_deque&& other)
{
    if (&other != this)
        std::swap (*this, other);
    return *this;
}

#if 0

template <class B>
hetero_deque<B>::hetero_deque (const hetero_deque& other)
    : _memory (other._memory)
    , _front (!other._front ? 0 :
              (header*) &_memory[0] +
              (other._front - (header*) &other._memory[0]))
    , _back (!other._back ? 0 :
             (header*) &_memory[0] +
             (other._back - (header*) &other._memory[0]))
{
}

template <class B>
hetero_deque<B>& hetero_deque<B>::operator= (const hetero_deque& other)
{
    if (&other != this)
    {
        hetero_deque aux (other);
        std::swap (*this, aux);
    }
    
    return *this;
}

#endif

template <class B>
hetero_deque<B>::~hetero_deque ()
{
    clear ();
}

template <class B>
B& hetero_deque<B>::back ()
{
    return this->access (_back);
}

template <class B>
const B& hetero_deque<B>::back () const
{
    return this->access (_back);
}

template <class B>
B& hetero_deque<B>::front ()
{
    return this->access (_front);
}

template <class B>
const B& hetero_deque<B>::front () const
{
    return this->access (_front);
}

template <class B>
B& hetero_deque<B>::access (header* data) const
{
    if (!data || !data->access)
        throw hetero_deque_empty ();
    return * data->access;
}

template <class B>
const B& hetero_deque<B>::access (const header* data) const
{
    if (!data || !data->access)
        throw hetero_deque_empty ();
    return * data->access;
}

/**
 * Constructs a new object of type concrete in the given
 * address. The header 'next' will point to first memory address
 * after the object and the 'prev' to the previous one.
 */
template <class B>
template <class Concrete, typename ...Args>
void hetero_deque<B>::construct (header* data, Args&&... args)
{
    static_assert (std::is_base_of<B, Concrete>::value,
                   "Elements should derived from Base.");
        
    void* obj_data = data + 1;
    Concrete* obj  = new (obj_data) Concrete (std::forward<Args> (args) ...);
    *data = header { obj, reinterpret_cast<header*> (obj + 1), 0 };
}

template <class B>
template <class Concrete, typename ...Args>
bool hetero_deque<B>::push_back (Args&& ... args)
{
    std::ptrdiff_t req_size  = 2 * sizeof (header) + sizeof (Concrete);
    header*        old_back  = _back;
    
    if (_back->next) // Non empty deque
    {
        _back = _back->next;

        if (_front < _back &&
            req_size > &_memory[0] + _memory.size () - (char*) _back)
            _back   = reinterpret_cast<header*> (&_memory[0]);
        
        if (_back < _front && (char*) _front - (char*) _back < req_size) {
            _back  = old_back;
            return false;
        }
    }
    else if (req_size > (std::ptrdiff_t) _memory.size ())
        return false;
    
    auto construct_guard = make_guard ([&] { _back  = old_back; });
    this->construct<Concrete> (_back, std::forward<Args> (args) ...);
    *_back->next = header { 0, 0, _back };
    if (_back != old_back) {
        _back->prev = old_back;
        _back->prev->next = _back;
    }
    
    construct_guard.dismiss ();
    return true;
}

template <class B>
bool hetero_deque<B>::pop_back ()
{    
    if (_back->next) // Non empty deque
    {
        _back->access->~B ();
        if (_back->prev) { // More than one element
            _back = _back->prev;
            *_back->next = header { 0, 0, _back };
        } else 
            *_back = header { 0, 0, 0 };

        return true;
    }
    
    return false;
}

template <class B>
template <class Concrete, typename ...Args>
bool hetero_deque<B>::push_front (Args&& ... args)
{
    std::ptrdiff_t req_size  = sizeof (header) + sizeof (Concrete);
    header*        old_front = _front;
    
    if (_front->next) // Non empty deque
    {
        _front = reinterpret_cast<header*> ((char*) _front - req_size);

        if (old_front > _back)
        {
            if (_front < _back->next + 1) {
                _front = old_front;
                return false;
            }
        }
        else if ((char*) _front < &_memory[0])
        {
            _front = reinterpret_cast<header*> (
                &_memory[0] + _memory.size () - req_size);
            
            if (_front < _back->next + 1) {
                _front = old_front;
                return false;
            }
        }
    }
    else // Empty deque. Same as pushing back.
    {
        return this->push_back<Concrete> (std::forward<Args> (args) ...);
    }
    
    auto construct_guard = make_guard ([&] { _front  = old_front; });
    this->construct<Concrete> (_front, std::forward<Args> (args) ...);
    _front->next = old_front;
    _front->next->prev = _front;
    
    construct_guard.dismiss ();
    return true;
}

template <class B>
bool hetero_deque<B>::pop_front ()
{
    if (_front->next) // Non empty deque
    {
        _front->access->~B ();
        if (_front->next->next) { // More than one element
            _front = _front->next;
            _front->prev = 0;
        } else
            *_front = header { 0, 0, 0 };
        
        return true;
    }
    
    return false;
}

template <class B>
void hetero_deque<B>::clear ()
{
    if (!_front) // Move constructor.
        return;
    for (auto it = _front; it->next != 0; it = it->next)
        it->access->~B ();
    _front = _back = reinterpret_cast<header*> (&_memory[0]);
    *_front = header { 0, 0, 0 };
}

template <class B>
void hetero_deque<B>::swap (hetero_deque& other)
{
    std::swap (_memory, other._memory);
    std::swap (_front, other._front);
    std::swap (_back, other._back);
    assert (_front >= &_memory [0]); // TEST
}
    
} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_HETERO_DEQUE_TPP_ */

