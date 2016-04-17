/**
 *  Time-stamp:  <2011-06-22 22:55:01 raskolnikov>
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
#include <vector>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include <psynth/base/util.hpp>
#include <psynth/base/exception.hpp>


namespace psynth
{
namespace base
{

PSYNTH_DECLARE_ERROR (error, hetero_deque_empty);

namespace detail
{

template <class Base>
struct hetero_header
{
    Base*          access;
    hetero_header* next;
    hetero_header* prev;
};

template <class Base>
class hetero_iterator : public boost::iterator_facade <
    hetero_iterator<Base>,
    Base,
    boost::bidirectional_traversal_tag>
{
    typedef hetero_header<Base> header;

public:
    explicit hetero_iterator (header* curr)
        : _curr (curr) {}

    hetero_iterator () : _curr (0) {}
    hetero_iterator (const hetero_iterator&) = default;
    hetero_iterator& operator= (const hetero_iterator&) = default;

private:
    friend class boost::iterator_core_access;

    Base& dereference () const
    { return *_curr->access; }

    bool equal (const hetero_iterator& other) const
    { return _curr == other._curr; }

    void increment ()
    { _curr = _curr->next; }

    void decrement ()
    { _curr = _curr->prev; }

    header* _curr;
};

} /* namespace detail */

/**
 * A deque of constant size over polymorphic types with a common
 * base. This provides a way to store polymorphic objects without
 * allocating memory, which allows object oriented programming in many
 * real-time situations.
 *
 * Base is the common base of all the elements in the collection. Note
 * that, in the general case, Base should define a virtual destructor.
 *
 * @todo Pushing an element invalidates the end iterator. Maybe it is
 * desirable to change this behaviour?
 *
 * @todo Abstract raw memory management into a separate non template
 * class.
 *
 * @todo Parametrize allocator?
 */
template <class Base>
class hetero_deque : public boost::noncopyable
{
public:
    typedef detail::hetero_iterator<Base> iterator;
    typedef detail::hetero_iterator<const Base> const_iterator;
    typedef boost::reverse_iterator<iterator> reverse_iterator;
    typedef boost::reverse_iterator<const_iterator> const_reverse_iterator;

    typedef typename iterator::value_type value_type;
    typedef typename iterator::reference reference;
    typedef typename iterator::difference_type difference_type;
    typedef typename const_iterator::reference const_reference;

    explicit hetero_deque (std::size_t size = 0);
    hetero_deque (hetero_deque&&);
    hetero_deque& operator= (hetero_deque&&);
#if 0
    hetero_deque (const hetero_deque&);
    hetero_deque& operator= (const hetero_deque&);
#endif
    ~hetero_deque ();

    Base& back ();
    const Base& back () const;
    template <class Concrete, typename ...Args>
    bool push_back (Args&& ... args);
    template <class Concrete>
    bool push_back (Concrete&& arg)
    {
        return this->push_back<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }
    bool pop_back ();

    Base& front ();
    const Base& front () const;
    template <class Concrete, typename ...Args>
    bool push_front (Args&&... args);
    template <class Concrete>
    bool push_front (Concrete&& arg)
    {
        return this->push_front<
            typename std::decay<Concrete>::type,
            decltype (std::forward<Concrete> (arg))> (
                std::forward<Concrete> (arg));
    }
    bool pop_front ();

    void clear ();

    void swap (hetero_deque& other);

    bool empty () const
    { return !_front->access; }

    iterator begin ()
    { return iterator (_front); }
    const_iterator begin () const
    { return const_iterator (_front); }
    const_iterator cbegin () const
    { return const_iterator (_front); }
    reverse_iterator rbegin ()
    { return reverse_iterator (_front); }
    const_reverse_iterator rbegin () const
    { return const_reverse_iterator (_front); }

    iterator end ()
    { return iterator (_back->next ? _back->next : _back); }
    const_iterator end () const
    { return const_iterator (_back->next ? _back->next : _back); }
    const_iterator cend () const
    { return const_iterator (_back->next ? _back->next : _back); }
    reverse_iterator rend ()
    { return reverse_iterator (_back->next ? _back->next : _back); }
    const_reverse_iterator rend () const
    { return const_reverse_iterator (_back->next ? _back->next : _back); }

private:
    typedef detail::hetero_header<Base> header;
    static_assert (std::is_pod<header>::value,
                   "hetero_header must be a POD");

    template <class Concrete, typename ...Args>
    void construct (header* data, Args&&... args);

    const Base& access (const header* data) const;
    Base& access (header* data) const;

    typedef std::vector<char> memory_t;

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
