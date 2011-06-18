/**
 *  Time-stamp:  <2011-06-17 12:02:28 raskolnikov>
 *
 *  @file        patch.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Thu Jun  9 21:33:31 2011
 *
 *  @brief A patch.
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

#ifndef PSYNTH_GRAPH_PATCH_HPP_
#define PSYNTH_GRAPH_PATCH_HPP_

#include <list>

#include <boost/range/iterator_range.hpp>
#include <boost/intrusive/list.hpp>

#include <psynth/new_graph/node.hpp>
#include <psynth/new_graph/core/patch_fwd.hpp>

namespace psynth
{
namespace graph
{
namespace core
{

PSYNTH_DECLARE_ERROR (error, patch_child_error);

class patch : public node
{
public:
    typedef std::list<graph::node_ptr> child_list;
    
    typedef boost::intrusive::list<
        node,
        boost::intrusive::member_hook<
            node,
            patch_child_hook,
            &node::_patch_child_hook> >
    rt_child_list;
    
    typedef child_list::iterator child_iterator;
    typedef child_list::const_iterator child_const_iterator;
    typedef boost::iterator_range<child_iterator> child_range;
    typedef boost::iterator_range<child_const_iterator> child_const_range;

    typedef rt_child_list::iterator rt_child_iterator;
    typedef rt_child_list::const_iterator rt_child_const_iterator;
    typedef boost::iterator_range<rt_child_iterator> rt_child_range;
    typedef boost::iterator_range<rt_child_const_iterator> rt_child_const_range;

    void rt_process (rt_process_context& ctx);
    void rt_context_update (rt_process_context& ctx);
    void rt_advance ();
    
    node_ptr add (node_ptr child);
    void remove (node_ptr child);
    
    child_range childs ()
    { return boost::make_iterator_range (_childs); }
    child_const_range cchilds () const
    { return boost::make_iterator_range (_childs); }

    rt_child_range rt_childs ()
    { return boost::make_iterator_range (_rt_childs); }
    rt_child_const_range rt_cchilds () const
    { return boost::make_iterator_range (_rt_childs); }
    
protected:
    child_list _childs;
    rt_child_list _rt_childs;
    child_list _rt_outputs;
};

} /* namespace core */
} /* namespace graph */
} /* namespace psynth */

#endif /* PSYNTH_GRAPH_PATCH_HPP_ */
