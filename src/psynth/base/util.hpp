/**
 *  Time-stamp:  <2011-03-17 00:19:11 raskolnikov>
 *
 *  @file        util.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Tue Apr  7 11:53:31 2009
 *
 *  Small utilities, most of them with almost no compilation overhead.
 *  Some of them where before in misc.hpp
 */

/*
 *  Copyright (C) 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_BASE_UTIL_H_
#define PSYNTH_BASE_UTIL_H_

#include <psynth/base/compat.hpp>
#include <boost/utility.hpp>

namespace psynth
{
namespace base
{

template <typename T>
void ignore_unused_variable_warning (T&&) {}

template <template <class, class> class F, typename Y>
struct tpl_bind_snd
{
    template <class X>
    struct type : public F<X, Y>
    {};
};

template <template <class, class> class F, typename X>
struct tpl_bind_fst
{
    template <class Y>
    struct type : public F<X, Y>
    {};
};

struct nop_ostream {};
template <typename T> PSYNTH_FORCEINLINE
const nop_ostream& operator<< (const nop_ostream& s, const T&) { return s; }

/**
 * Utility to compare for equality pointers with weird comparison
 * semantics like std::shared_ptr.
 * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1590.html
 */
template <typename Ptr1, typename Ptr2>
bool equal_ptr (Ptr1 a, Ptr2 b)
{
    return &*a == &*b;
}

} /* namespace base */
} /* namespace psynth */


#endif /* PSYNTH_BASE_UTIL_H_ */
