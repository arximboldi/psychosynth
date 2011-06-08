/**
 *  Time-stamp:  <2011-06-08 18:07:12 raskolnikov>
 *
 *  @file        hetero_queque.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Jun  8 12:32:51 2011
 *
 *  @brief Constant size deque over heterogeneous elements.
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

#ifndef PSYNTH_BASE_HETERO_DEQUE_HPP_
#define PSYNTH_BASE_HETERO_DEQUE_HPP_

#include <type_traits>
#include <memory>
#include <psynth/base/util.hpp>
#include <psynth/base/exception.hpp>

namespace psynth
{
namespace base
{

PSYNTH_DECLARE_ERROR (error, hetero_deque_empty);

/**
 * A deque of constant size over polymorphic types with a common
 * base. This provides a way to store polymorphic objects without
 * allocating memory, which allows object oriented programming in many
 * real-time situations.
 *
 * Base is the common base of all the elements in the collection. Note
 * that, in the general case, Base should define a virtual destructor.
 *
 * @todo Abstract raw memory management into a separate non template
 * class.
 */
template <class Base>
class hetero_deque : public boost::noncopyable
{
public:
    
    explicit hetero_deque (std::size_t size = 0);
    hetero_deque (hetero_deque&&);
    hetero_deque& operator= (hetero_deque&&);
    ~hetero_deque ();
    
    Base& back ();
    const Base& back () const;

    Base& front ();
    const Base& front () const;

    template <class Concrete, typename ...Args>
    bool push_back (Args&& ... args);
    bool pop_back ();
    
    template <class Concrete, typename ...Args>
    bool push_front (Args&&... args);
    bool pop_front ();

    void clear ();
    
    void swap (hetero_deque& other);

    bool empty () const
    { return !_front->access; }
    
private:
    struct header
    {
        Base*       access;
        header*     next;
        header*     prev;
    };
    static_assert (std::is_pod<header>::value,
                   "Why is this not a POD?");
    
    template <class Concrete, typename ...Args>
    void construct (header* data, Args&&... args);
    
    const Base& access (const header* data) const;
    Base& access (header* data) const;
        
    typedef std::unique_ptr<char[]> memory_t;
    
    std::size_t _size;
    memory_t    _memory;
    header*     _front;
    header*     _back;
};

} /* namespace base */
} /* namespace psynth */

namespace std
{

template <class B>
void swap (psynth::base::hetero_deque<B>& a,
           psynth::base::hetero_deque<B>& b)
{
    a.swap (b);
}

} /* namespace std */

#include <psynth/base/hetero_deque.tpp>

#endif /* PSYNTH_BASE_HETERO_DEQUE_HPP_ */

