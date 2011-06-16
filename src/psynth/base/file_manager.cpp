/**
 *  Time-stamp:  <2011-06-13 15:47:22 raskolnikov>
 *
 *  @file        file_manager.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  Hierarchical file management system. Implementation.
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
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

#include <cstring>
#include <cstdlib>

#include "tree.tpp"
#include "file_manager.hpp"

namespace bf = boost::filesystem;

namespace psynth
{
namespace base
{

template class singleton_holder <file_mgr_node>;
template class tree_node <file_mgr_node>;
template class PSYNTH_DEFAULT_THREADING <file_mgr_node>;

bf::path file_mgr_node::find_all (const bf::path& file) const
{
    lock lock (this);
    
    bf::path res;

    res = find (file);
    for (const_iterator it = begin(); res.empty() && it != end(); ++it)
	res = it->find_all (file);
    
    return res;
}

} /* namespace base */
} /* namespace psynth */
