/**************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef PSYNTH_TREE_H
#define PSYNTH_TREE_H

#include <map>
#include <libpsynth/common/map_iterator.h>
#include <libpsynth/common/deleter.h>

namespace psynth
{

/**
 * @todo Fix copy operations.
 */
template <class KeyType, class NodeType> 
class tree_node
{
    std::map<KeyType, NodeType*> m_childs;
    NodeType* m_parent;
    KeyType m_name;
    bool m_isinit;

    virtual void on_new_child(NodeType& node) {}
    virtual void on_remove_child(NodeType& node) {}
    virtual void on_init() {}
    
    /**
     * Returns whether this node has been initialized.
     */
    bool is_init() const {
	return m_isinit;
    }

public:
    /**
     * Iterator to check the childs of this node.
     */
    typedef map_iterator<KeyType, NodeType*> iterator;

   /**
     * Iterator to check the childs of this node, const version.
     */
    typedef map_const_iterator<KeyType, NodeType*> const_iterator;

    
    /** Constuctor. */
    tree_node () :
	m_parent(NULL), m_isinit(false)
	{};

    /** Destructor */
    ~tree_node () {
	iterator iter = begin ();
	while(iter != end ())
	    delete *iter++;
    }

    /**
     * Initializes by setting the name and the parent of the config node.
     * @param name The name of the config node.
     * @param parent The paren of this config node.
     */
    void init (const KeyType& name, NodeType* parent) {
	m_isinit = true;
	m_name = name;
	m_parent = parent;
	on_init ();
    }

    /**
     * Returns an iterator to the first child of this node.
     */
    iterator begin () {
	return m_childs.begin();
    }

    /**
     * Returns an iterator to the first child of this node. Const version.
     */
    const_iterator begin() const {
	return m_childs.begin();
    }

    /**
     * Returns an iterator to the end of this node childs.
     */
    iterator end() {
	return m_childs.end();
    }

    /**
     * Returns an iterator to the end of this node childs. Const version.
     */
    const_iterator end() const {
	return m_childs.end();
    }
    
    /**
     * Returns a reference to the child of this node mathing a name. The child
     * is created if it does not exist yet.
     * @param name The name of the child.
     */
    NodeType& get_child (const KeyType& name) {
	if (!m_childs[name]) {
	    m_childs[name] = new NodeType();
	    m_childs[name]->init (name, dynamic_cast<NodeType*>(this));
	    on_new_child (*m_childs[name]);
	}
	
	return *m_childs[name];
    }

    /**
     * Deletes a child of this node.
     * @return An iterator to the next child.
     */
    iterator remove_child (iterator& iter) {
	iterator next = iter;
	next++;
	on_remove_child(**iter);
	delete *iter;
	m_childs.erase(iter);
	return next;
    }

    /**
     * Removes all childs.
     */
    void clear_childs () {
	iterator iter = begin ();
	while(iter != end ())
	    iter = remove_child (iter);
    }
    
    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    const NodeType* get_parent () const {
	return m_parent;
    }

    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    NodeType* get_parent () {
	return m_parent;
    }
    
    /**
     * Returns the name of this node.
     */
    const KeyType& get_name () const {
	return m_name;
    }
    
    /**
     * Returns the path of this node from the root to the node.
     */
    KeyType get_path_name () const {
	if (m_parent)
	    return m_parent->get_path_name() + "/" + m_name;
	else
	    return m_name;
    }

    /**
     * Returns a reference to the child matching a path. The path is constructed
     * by separating consecutive childs by / like paths in filenames. Any
     * node in the path that does not exist yet is created.
     */
    NodeType& get_path(KeyType name);
};

template <class KeyType, class NodeType>
NodeType& tree_node <KeyType, NodeType>::get_path (KeyType path)
{
    std::string base;
    for (size_t i = 0; i != path.size(); ++i)
	if (path[i] == '/') {
	    base.assign(path, 0, i);
	    while(path[++i] == '/');
	    path.erase(0, i);
	    break;
	}

    if (base.empty ()) {
	return get_child (path);
    }

    return get_child (base).get_path (path);
}

} /* namespace psynth */

#endif /* PSYNTH_TREE_H  */
