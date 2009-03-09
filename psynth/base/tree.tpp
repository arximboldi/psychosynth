/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2009                          *
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

namespace psynth
{

template <class N, class K, class T>
tree_node <N, K, T>::~tree_node ()
{
    for (map_iterator<K, null_ptr<N> > it = m_childs.begin ();
	 it != m_childs.end ();
	 ++it)
	delete *it;
}

template <class N, class K, class T>
typename tree_node <N, K, T>::iterator
tree_node <N, K, T>::remove_child (iterator& iter)
{
    iterator next = iter;
    next ++;
    on_remove_child (*iter);
    delete &*iter;
    m_childs.erase (iter);

    return next;
}

template <class N, class K, class T>
void tree_node <N, K, T>::clear_childs ()
{
    iterator iter = begin ();
    while (iter != end ())
	iter = remove_child (iter);
}

template <class N, class K, class T>
K tree_node <N, K, T>::get_path_name () const
{
    K prefix;
    get_path_name (prefix);
    return prefix;
}

template <class N, class K, class T>
void tree_node <N, K, T>::get_path_name (K& prefix) const
{
    if (!m_parent)
	prefix.insert (prefix.end (), m_name.begin (), m_name.end ());
    else {
	m_parent->get_path_name (prefix);
	prefix.insert (prefix.end (), T::separator);
	prefix.insert (prefix.end (), m_name.begin (), m_name.end ());
    }
}

template <class N, class K, class T>
N& tree_node <N, K, T>::get_child (const K& name)
{
    null_ptr<N>& c = m_childs [name]; 
    
    if (!c) {
	c = new N ();
	c->init (name, dynamic_cast<N*>(this));
	on_new_child (*c);
    }
	
    return *c;
}

template <class N, class K, class T>
N& tree_node <N, K, T>::get_existing_child (const K& name)
{
    iterator it = m_childs.find (name);

    if (it == end ())
	throw tree_node_error ("Can't find child node.");
	
    return *it;
}

template <class N, class K, class T>
template <typename InputIterator>
void tree_node <N, K, T>::find_base (InputIterator& base_b,
				     InputIterator& base_e)
{
    InputIterator end = base_e;
    base_e = base_b;
    while (base_e != end && *base_e != T::separator)
	++ base_e;
}

template <class N, class K, class T>
N& tree_node <N, K, T>::get_path (typename K::const_iterator begin,
				  typename K::const_iterator end)
{
    typename K::const_iterator base_end = end;
    typename K::const_iterator rest_begin;
    
    find_base (begin, base_end);
    rest_begin = base_end;
    rest_begin ++;
    
    return base_end == end ?
	get_child (K (begin, base_end)) :
	get_child (K (begin, base_end)).get_path (rest_begin, end);
}

template <class N, class K, class T>
N& tree_node <N, K, T>::get_path (const K& path)
{
    return get_path (path.begin (), path.end ());
}

template <class N, class K, class T>
N& tree_node <N, K, T>::
get_existing_path (typename K::const_iterator begin,
		   typename K::const_iterator end)
{
    typename K::const_iterator base_end = end;
    typename K::const_iterator rest_begin;
    
    find_base (begin, base_end);
    rest_begin = base_end;
    rest_begin ++;
    
    return base_end == end ?
	get_existing_child (K (begin, base_end)) :
	get_existing_child (K (begin, base_end))
	.get_existing_path (++ base_end, end);
}

template <class N, class K, class T>
N& tree_node <N, K, T>::get_existing_path (const K& path)
{
    return get_existing_path (path.begin (), path.end ());
}

template <class N, class K, class T>
const N& tree_node <N, K, T>::
get_existing_path (typename K::const_iterator begin,
		   typename K::const_iterator end) const
{
    typename K::const_iterator base_end = end;
    typename K::const_iterator rest_begin;
    
    find_base (begin, base_end);
    rest_begin = base_end;
    rest_begin ++;
    
    return base_end == end ?
	get_existing_child (K (begin, base_end)) :
	get_existing_child (K (begin, base_end))
	.get_existing_path (base_end, end);
}

template <class N, class K, class T>
const N& tree_node <N, K, T>::get_existing_path (const K& path) const
{
    return get_existing_path (path.begin (), path.end ());
}

} /* namespace psynth */
