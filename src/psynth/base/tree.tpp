/**
 *  Time-stamp:  <2010-11-03 00:20:50 raskolnikov>
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

#ifndef PSYNTH_TREE_T_
#define PSYNTH_TREE_T_

namespace psynth
{
namespace base
{

template <class N, class K, class T, class P>
tree_node <N, K, T, P>::~tree_node ()
{
    uninit ();
    
    for (map_iterator<K, null_ptr<N> > it = m_childs.begin ();
	 it != m_childs.end ();
	 ++it)
	delete *it;
}

template <class N, class K, class T, class P>
bool tree_node<N, K, T, P>::attach (const K& name, N& node)
{
    tree_lock lock (this);
    
    if (node.m_isinit || m_childs.find (name) != end ())
	return false;

    node.init (name, dynamic_cast<N*> (this));
    m_childs.insert (std::make_pair (name, null_ptr<N>(&node)));
    on_new_child (node);

    return true;
}

template <class N, class K, class T, class P>
N& tree_node<N, K, T, P>::detach (iterator iter)
{    
    if (iter == end ())
	throw tree_node_error ("Invalid iterator.");
    
    N& node = *iter;
      
    m_childs.erase (iter);
    on_remove_child (*iter);
    iter->uninit ();
      
    return node;
}

template <class N, class K, class T, class P>
typename tree_node <N, K, T, P>::iterator
tree_node <N, K, T, P>::remove_child (iterator& iter)
{
    iterator next = iter;
    next ++;
    on_remove_child (*iter);
    delete &*iter;
    m_childs.erase (iter);

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
    
    null_ptr<N>& c = m_childs [name]; 
    
    if (!c) {
	c = new N ();
	c->init (name, dynamic_cast<N*>(this));
	on_new_child (*c);
    }
	
    return *c;
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::existing_child (const K& name)
{
    tree_lock lock (this);
    
    iterator it = m_childs.find (name);

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
const N& tree_node <N, K, T, P>::existing_path (const K& p) const
{
    return existing_path (p.begin (), p.end ());
}

template <class N, class K, class T, class P>
N& tree_node <N, K, T, P>::existing_path (const K& p)
{
    return existing_path (p.begin (), p.end ());
}

/*
  Private methods
*/

template <class N, class K, class T, class P>
void tree_node <N, K, T, P>::path_name (K& prefix) const
{
    tree_lock lock (this);
    
    if (m_parent)
    {
	if (m_parent->m_parent) {
	    m_parent->path_name (prefix);
	    prefix.insert (prefix.end (), T::separator);
	}
	prefix.insert (prefix.end (), m_name.begin (), m_name.end ());
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

#endif /* PSYNTH_TREE_T_ */
