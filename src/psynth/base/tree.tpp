/**
 *  Time-stamp:  <2011-06-13 15:48:32 raskolnikov>
 *
 *  @file        tree.tpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2009
 *
 *  The tree_node implementation.
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

#ifndef PSYNTH_BASE_TREE_T_
#define PSYNTH_BASE_TREE_T_

#include <psynth/base/tree.hpp>

namespace psynth
{
namespace base
{

template <class N, class K, class T, class P>
tree_node <N, K, T, P>::~tree_node ()
{
    uninit ();
}

template <class N, class K, class T, class P>
bool tree_node<N, K, T, P>::attach (const K& name, node_ptr node)
{
    tree_lock lock (this);

    N& ref = *node;
    if (ref._isinit || _childs.find (name) != end ())
	return false;

    ref.init (name, dynamic_cast<N*> (this));
    _childs [name] = std::move (node);
    on_new_child (ref);

    return true;
}

template <class N, class K, class T, class P>
typename tree_node<N, K, T, P>::node_ptr
tree_node<N, K, T, P>::detach (iterator iter)
{
    if (iter == end ())
	throw tree_node_error ("Invalid iterator.");

    decltype (_childs.begin ()) iter_map = iter;
    node_ptr node = std::move (iter_map->second);
    _childs.erase (iter_map);

    on_remove_child (*node);
    node->uninit ();

    return node;
}

template <class N, class K, class T, class P>
typename tree_node <N, K, T, P>::iterator
tree_node <N, K, T, P>::remove_child (iterator iter)
{
    iterator next = std::next (iter);

    on_remove_child (*iter);
    _childs.erase (iter);

    return next;
}

template <class N, class K, class T, class P>
void tree_node <N, K, T, P>::clear_childs ()
{
    tree_lock lock (this);

    iterator iter = begin ();
    while (iter != end ())
	iter = remove_child (iter);
}

template <class N, class K, class T, class P>
K tree_node <N, K, T, P>::path_name () const
{
    tree_lock lock (this);

    K prefix;
    path_name (prefix);
    return prefix;
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::child (const K& name)
{
    tree_lock lock (this);

    iterator it = _childs.find (name);

    if (it == _childs.end ()) {
	it = _childs.insert (make_pair (name, std::unique_ptr<N> (new N ()))).first;

	it->init (name, dynamic_cast<N*>(this));
	on_new_child (*it);
    }

    return *it;
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::existing_child (const K& name)
{
    return const_cast<N&> (
        const_cast <const tree_node*> (this)->existing_child (name));
}

template <class N, class K, class T, class P>
const N& tree_node <N, K, T, P>::existing_child (const K& name) const
{
    tree_lock lock (this);

    const_iterator it = _childs.find (name);
    if (it == end ())
	throw tree_node_error ("Can't find child node.");

    return *it;
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::path (const K& p)
{
    return path (p.begin (), p.end ());
}

template <class N, class K, class T, class P>
const N& tree_node <N, K, T, P>::existing_path (const K& path) const
{
    return existing_path (path.begin (), path.end ());
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::existing_path (const K& path)
{
    return existing_path (path.begin (), path.end ());
}

/*
  Private methods
*/

template <class N, class K, class T, class P>
void tree_node <N, K, T, P>::path_name (K& prefix) const
{
    tree_lock lock (this);

    if (_parent)
    {
	if (_parent->_parent) {
	    _parent->path_name (prefix);
	    prefix.insert (prefix.end (), T::separator);
	}
	prefix.insert (prefix.end (), _name.begin (), _name.end ());
    }
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::path (typename K::const_iterator begin,
				  typename K::const_iterator end)
{
    typename K::const_iterator base_end = end;
    typename K::const_iterator rest_begin;

    find_base (begin, base_end);
    rest_begin = base_end;
    rest_begin ++;

    return base_end == end ?
	child (K (begin, base_end)) :
	child (K (begin, base_end)).path (rest_begin, end);
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::
existing_path (typename K::const_iterator begin,
		   typename K::const_iterator end)
{
    typename K::const_iterator base_end = end;
    typename K::const_iterator rest_begin;

    find_base (begin, base_end);
    rest_begin = base_end;
    rest_begin ++;

    return base_end == end ?
	existing_child (K (begin, base_end)) :
	existing_child (K (begin, base_end))
	.existing_path (rest_begin, end);
}

template <class N, class K, class T, class P>
const N& tree_node <N, K, T, P>::
existing_path (typename K::const_iterator begin,
		   typename K::const_iterator end) const
{
    typename K::const_iterator base_end = end;
    typename K::const_iterator rest_begin;

    find_base (begin, base_end);
    rest_begin = base_end;
    rest_begin ++;

    return base_end == end ?
	existing_child (K (begin, base_end)) :
	existing_child (K (begin, base_end))
	.existing_path (rest_begin, end);
}

template <class N, class K, class T, class P>
template <typename InputIterator>
void tree_node <N, K, T, P>::find_base (InputIterator& base_b,
				     InputIterator& base_e)
{
    InputIterator end = base_e;
    base_e = base_b;
    while (base_e != end && *base_e != T::separator)
	++ base_e;
}

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_BASE_TREE_T_ */
