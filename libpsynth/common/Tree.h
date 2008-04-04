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
#include <libpsynth/common/MapIterator.h>
#include <libpsynth/common/Deleter.h>

namespace psynth
{

/**
 * @todo Fix copy operations.
 */
template <class KeyType, class NodeType> 
class TreeNode
{
    std::map<KeyType, NodeType*> m_childs;
    NodeType* m_parent;
    KeyType m_name;
    bool m_isinit;

    virtual void onNewChild(NodeType& node) {}
    virtual void onRemoveChild(NodeType& node) {}
    virtual void onInit() {}
    
    /**
     * Returns whether this node has been initialized.
     */
    bool isInit() const {
	return m_isinit;
    }

public:
    /**
     * Iterator to check the childs of this node.
     */
    typedef MapIterator<KeyType, NodeType*> ChildIter;

   /**
     * Iterator to check the childs of this node, const version.
     */
    typedef MapConstIterator<KeyType, NodeType*> ConstChildIter;

    
    /** Constuctor. */
    TreeNode() :
	m_parent(NULL), m_isinit(false)
	{};

    /** Destructor */
    ~TreeNode() {
	//for_each(begin(), end(), Deleter<NodeType*>());
	ChildIter iter = begin();
	while(iter != end())
	    delete *iter++;
    }

    /**
     * Initializes by setting the name and the parent of the config node.
     * @param name The name of the config node.
     * @param parent The paren of this config node.
     */
    void init(const KeyType& name, NodeType* parent) {
	m_isinit = true;
	m_name = name;
	m_parent = parent;
	onInit();
    }

    /**
     * Returns an iterator to the first child of this node.
     */
    ChildIter begin() {
	return m_childs.begin();
    }

    /**
     * Returns an iterator to the first child of this node. Const version.
     */
    ConstChildIter begin() const {
	return m_childs.begin();
    }

    /**
     * Returns an iterator to the end of this node childs.
     */
    ChildIter end() {
	return m_childs.end();
    }

    /**
     * Returns an iterator to the end of this node childs. Const version.
     */
    ConstChildIter end() const {
	return m_childs.end();
    }
    
    /**
     * Returns a reference to the child of this node mathing a name. The child
     * is created if it does not exist yet.
     * @param name The name of the child.
     */
    NodeType& getChild(const KeyType& name) {
	if (!m_childs[name]) {
	    m_childs[name] = new NodeType();
	    m_childs[name]->init(name, dynamic_cast<NodeType*>(this));
	    onNewChild(*m_childs[name]);
	}
	
	return *m_childs[name];
    }

    /**
     * Deletes a child of this node.
     * @return An iterator to the next child.
     */
    ChildIter removeChild(ChildIter& iter) {
	ChildIter next = iter;
	next++;
	onRemoveChild(**iter);
	delete *iter;
	m_childs.erase(iter);
	return next;
    }

    /**
     * Removes all childs.
     */
    void clearChilds() {
	ChildIter iter = begin();
	while(iter != end())
	    iter = removeChild(iter);
    }
    
    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    const NodeType* getParent() const {
	return m_parent;
    }

    /**
     * Returns a pointer to the parent of this node or @c null if this is a root
     * node.
     */
    NodeType* getParent() {
	return m_parent;
    }
    
    /**
     * Returns the name of this node.
     */
    const KeyType& getName() const {
	return m_name;
    }
    
    /**
     * Returns the path of this node from the root to the node.
     */
    KeyType getPathName() const {
	if (m_parent)
	    return m_parent->getPathName() + "/" + m_name;
	else
	    return m_name;
    }

    /**
     * Returns a reference to the child matching a path. The path is constructed
     * by separating consecutive childs by / like paths in filenames. Any
     * node in the path that does not exist yet is created.
     */
    NodeType& getPath(KeyType name);
};

template<class KeyType, class NodeType>
NodeType& TreeNode<KeyType, NodeType>::getPath(KeyType path)
{
    std::string base;
    for (size_t i = 0; i != path.size(); ++i)
	if (path[i] == '/') {
	    base.assign(path, 0, i);
	    while(path[++i] == '/');
	    path.erase(0, i);
	    break;
	}

    if (base.empty()) {
	return getChild(path);
    }

    return getChild(base).getPath(path);
}

} /* namespace psynth */

#endif /* PSYNTH_TREE_H  */
