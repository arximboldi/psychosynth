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
#include <typeinfo>

#include <psynth/base/iterator.hpp>
#include <psynth/base/deleter.hpp>
#include <psynth/base/exception.hpp>

namespace psynth
{

template <typename KeyType>
struct tree_node_traits
{
    typedef typename KeyType::value_type value_type;
    static const value_type separator = value_type ();
};

template <>
struct tree_node_traits<std::string>
{
    typedef char value_type;
    static const char separator = '.';
};

/**
 * Exception for this type.
 */
PSYNTH_ERROR_WHERE(base_error, tree_node_error, "psynth.base.tree");

/**
 * A tree node. The void sequence is a valid name, so the key string "..."
 * denotes the node "" -> "" -> "" -> "".
 *
 * @todo Fix copy operations. Fix string operations to be more generic.
 */
template <class NodeType,
	  class KeyType = std::string,
	  class Traits = tree_node_traits<KeyType> > 
class tree_node
{  
public:
    /**
     * Iterator to check the childs of this node.
     */
    typedef ptr_iterator<map_iterator<KeyType, null_ptr<NodeType> > > iterator;

    /**
     * Iterator to check the childs of this node, const version.
     */
    typedef ptr_const_iterator<map_const_iterator<KeyType, null_ptr<NodeType> > >
    const_iterator;

    /**
     * Traits being used in this instanciation.
     */
    typedef Traits traits;
    
    /** Constuctor. */
    tree_node () :
	m_parent(NULL), m_isinit(false)
    {
    };

    /** Destructor */
    ~tree_node ();

    /**
     * Returns an iterator to the first child of this node.
     */
    iterator begin ()
    {
	return m_childs.begin();
    }

    /**
     * Returns an iterator to the first child of this node. Const version.
     */
    const_iterator begin () const
    {
	return m_childs.begin ();
    }

    /**
     * Returns an iterator to the end of this node childs.
     */
    iterator end ()
    {
	return m_childs.end ();
    }

    /**
     * Returns an iterator to the end of this node childs. Const version.
     */
    const_iterator end () const
    {
	return m_childs.end ();
    }
    
    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    const NodeType* get_parent () const
    {
	return m_parent;
    }

    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    NodeType* get_parent ()
    {
	return m_parent;
    }
    
    /**
     * Returns the name of this node.
     */
    const KeyType& get_name () const
    {
	return m_name;
    }

    /**
     * Returns the path of this node from the root to the node.
     */
    KeyType get_path_name () const;
    
    /**
     * Deletes a child of this node.
     * @return An iterator to the next child.
     */
    iterator remove_child (iterator& iter);

    /**
     * Removes all childs.
     */
    void clear_childs ();
    
    /**
     * Returns a reference to the child of this node mathing a name. The child
     * is created if it does not exist yet.
     * @param name The name of the child.
     */
    NodeType& get_child (const KeyType& name);

    /**
     * Returns a reference to the child of this node mathing a name,
     * throwing an exception if the node does not exist.
     * @param name The name of the child.
     */
    NodeType& get_existing_child (const KeyType& name);


    /**
     * Returns a reference to the child of this node mathing a name,
     * throwing an exception if the node does not exist.
     * @param name The name of the child.
     */
    const NodeType& get_existing_child (const KeyType& name) const;
    
    /**
     * Returns a reference to the child matching a path. All nodes
     * in the path are created if they do not exist.
     */
    NodeType& get_path (const KeyType& name);
    
    /**
     * Returns a reference to the child matching a path, an
     * exception is thrown if the path does not exist.
     * @param name The path to find.
     */
    NodeType& get_existing_path (const KeyType& name);

    /**
     * Returns a reference to the child matching a path, an
     * exception is thrown if the path does not exist.
     * @param name The path to find.
     */
    const NodeType& get_existing_path (const KeyType& name) const;

protected:
    void set_name (const KeyType& name)
    {
	m_name = name;
    }
    
    virtual void on_new_child (NodeType& node) {}
    virtual void on_remove_child (NodeType& node) {}
    virtual void on_init () {}
    
private:
    std::map<KeyType, null_ptr<NodeType> > m_childs;
    NodeType* m_parent;
    KeyType m_name;
    bool m_isinit;

    template <typename InputIterator>
    static void find_base (InputIterator& base_b, InputIterator& base_e);

    const NodeType&
    get_existing_path (typename KeyType::const_iterator begin,
		       typename KeyType::const_iterator end) const;
    NodeType&
    get_existing_path (typename KeyType::const_iterator begin,
		       typename KeyType::const_iterator end);
    NodeType& get_path (typename KeyType::const_iterator begin,
			typename KeyType::const_iterator end);
    
    void get_path_name (KeyType& prefix) const;

    bool is_init() const
    {
	return m_isinit;
    }

    void init (const KeyType& name, NodeType* parent)
    {
	m_isinit = true;
	m_name = name;
	m_parent = parent;
	on_init ();
    }
};

} /* namespace psynth */

#include <psynth/base/tree.tpp>

#endif /* PSYNTH_TREE_H  */
