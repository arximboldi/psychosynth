/**
 *  Time-stamp:  <2009-04-07 19:42:32 raskolnikov>
 *
 *  @file        tree.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  A generic way of building composite objects. Interface.
 *  This file is part of the Psychosynth project.
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_TREE_H
#define PSYNTH_TREE_H

#include <map>
#include <typeinfo>

#include <psynth/base/iterator.hpp>
#include <psynth/base/deleter.hpp>
#include <psynth/base/exception.hpp>
#include <psynth/base/threads.hpp>

namespace psynth
{

/**
 * Traits that provide additional information about how to
 * make paths out of a key.
 */
template <typename Key>
struct tree_node_traits
{
    /** The value of the contained type inside key. */
    typedef typename Key::value_type value_type;

    /** The value of the character used as separator for paths. */
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
PSYNTH_DECLARE_ERROR(base_error, tree_node_error);

/**
 * A tree node. This class in intended to be used by inheriting from it,
 * so you directly gain a tree interface for a composite type.
 * You should pass it the derived class as type parameter.
 *
 * Note that the void sequence is a valid name, so the key string "..."
 * denotes the node "" -> "" -> "" -> "".
 *
 * @param Node    The type of the class that is inheriting from us.
 * @param Key     They type of the class that identifies tree nodes.
 *                It must be some kind of compound type and in the current
 *                implementation it must have std::string semantics.
 * @param Traits  Additional information about the key type that is
 *                used to compose the paths.
 * @param ThreadingPolicy
 *                Wether you want your tree type to be thread-safe.
 *                Note that to avoid problems when creating mix-ins
 *                tree_node won't inherit from ThreadingPolicy but it
 *                will expect you to do so (it uses Node::lock).
 *
 * @todo Fix copy operations.
 * @todo Fix string operations to be more generic.
 */
template <class Node,
	  class Key = std::string,
	  class Traits = tree_node_traits<Key>,
	  class ThreadingPolicy = PSYNTH_DEFAULT_THREADING <Node> >
class tree_node
{   
public:
    /**
     * Iterator to check the childs of this node.
     */
    typedef ptr_iterator<map_iterator<Key, null_ptr<Node> > > iterator;

    /**
     * Iterator to check the childs of this node, const version.
     */
    typedef ptr_const_iterator<map_const_iterator<Key, null_ptr<Node> > >
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
     * @note Using iterators is not thread safe with any threading model.
     */
    iterator begin ()
    {
	return m_childs.begin();
    }

    /**
     * Returns an iterator to the first child of this node. Const version.
     * @note Using iterators is not thread safe with any threading model.
     */
    const_iterator begin () const
    {
	return m_childs.begin ();
    }
    
    /**
     * Returns an iterator to the end of this node childs.
     * @note Using iterators is not thread safe with any threading model.
     */
    iterator end ()
    {
	return m_childs.end ();
    }

    /**
     * Returns an iterator to the end of this node childs. Const version.
     * @note Using iterators is not thread safe with any threading model.
     */
    const_iterator end () const
    {
	return m_childs.end ();
    }
    
    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    const Node* get_parent () const
    {
	tree_lock lock (this);
	return m_parent;
    }

    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    Node* get_parent ()
    {
	tree_lock lock (this);
	return m_parent;
    }
    
    /**
     * Returns the name of this node.
     */
    const Key& get_name () const
    {
	tree_lock lock (this);
	return m_name;
    }
   
    /**
     * Returns an iterator to a child given by its key or end () otherwise.
     * @param name The key name of the child.
     */
    iterator find_child (const Key& name)
    {
	tree_lock lock (this);
	return m_childs.find (name);
    }
 
    /**
     * Returns an iterator to a child given by its key or end () otherwise.
     * @param name The key name of the child.
     */
    const_iterator find_child (const Key& name) const
    {
	tree_lock lock (this);
	return m_childs.find (name);
    }

    /**
     * Detaches a node from the tree. Once detached you are responsible
     * of freeing the node memory. If the key does not refer to a node
     * it throws an exception.
     *
     * @return A pointer to the detached node. 
     */
    Node& detach (const Key& name)
    {
	tree_lock lock (this);
	return detach (m_childs.find (name));
    }

    /**
     * Deletes a child of this node.
     * @return An iterator to the next child.
     */
    iterator remove_child (const Key& name)
    {
	tree_lock lock (this);
	return remove_child (m_childs.find (name));
    }

    /**
     * Attaches a node to a tree. Note that this wil fail if the node is
     * already attached to another tree or if the selected key is already
     * used by another child of this tree.
     *
     * @return true If the node was successfully attached and false otherwise.
     */
    bool attach (const Key& name, Node& node);
    
    /**
     * Detaches a node from the tree. Once detached you are responsible
     * of freeing the node memory. If the iterator points to end ()
     * it throws an exception.
     *
     * @return A pointer to the detached node.
     * @note This version of detach is not thread-safe.
     */
    Node& detach (iterator iter);
    
    /**
     * Returns the path of this node from the root to the node.
     */
    Key get_path_name () const;
    
    /**
     * Deletes a child of this node.
     * @return An iterator to the next child.
     * @return This version of remove child is not thread-safe.
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
    Node& get_child (const Key& name);

    /**
     * Returns a reference to the child of this node mathing a name,
     * throwing an exception if the node does not exist.
     * @param name The name of the child.
     */
    Node& get_existing_child (const Key& name);


    /**
     * Returns a reference to the child of this node mathing a name,
     * throwing an exception if the node does not exist.
     * @param name The name of the child.
     */
    const Node& get_existing_child (const Key& name) const;
    
    /**
     * Returns a reference to the child matching a path. All nodes
     * in the path are created if they do not exist.
     */
    Node& get_path (const Key& name);
    
    /**
     * Returns a reference to the child matching a path, an
     * exception is thrown if the path does not exist.
     * @param name The path to find.
     */
    Node& get_existing_path (const Key& name);

    /**
     * Returns a reference to the child matching a path, an
     * exception is thrown if the path does not exist.
     * @param name The path to find.
     */
    const Node& get_existing_path (const Key& name) const;

protected:    
    virtual void on_new_child (Node& node) {}
    virtual void on_remove_child (Node& node) {}
    virtual void on_init () {}
    virtual void on_uninit () {}
    
private:
    std::map<Key, null_ptr<Node> > m_childs;
    Node* m_parent;
    Key m_name;
    bool m_isinit;

    template <typename InputIterator>
    static void find_base (InputIterator& base_b, InputIterator& base_e);

    const Node&
    get_existing_path (typename Key::const_iterator begin,
		       typename Key::const_iterator end) const;

    Node&
    get_existing_path (typename Key::const_iterator begin,
		       typename Key::const_iterator end);

    Node& get_path (typename Key::const_iterator begin,
		    typename Key::const_iterator end);
    
    void get_path_name (Key& prefix) const;

    bool is_init() const
    {
	return m_isinit;
    }

    void init (const Key& name, Node* parent)
    {
	m_isinit = true;
	m_name = name;
	m_parent = parent;
	on_init ();
    }

    void uninit ()
    {
	if (m_isinit) {
	    m_isinit = false;
	    m_name = Key ();
	    m_parent = 0;
	    on_uninit ();
	}
    }

    struct tree_lock : public ThreadingPolicy::lock
    {
	tree_lock (const tree_node* self)
	    : ThreadingPolicy::lock (dynamic_cast<const Node*> (self))
	{}
    };
};

} /* namespace psynth */

#include <psynth/base/tree.tpp>

#endif /* PSYNTH_TREE_H  */
