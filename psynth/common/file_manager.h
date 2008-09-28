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

#ifndef PSYNTH_FILEMANAGER_H
#define PSYNTH_FILEMANAGER_H

#include <libpsynth/common/file_finder.h>
#include <libpsynth/common/tree.h>
#include <libpsynth/common/singleton.h>

namespace psynth
{

class file_manager : public file_finder,
		     public tree_node <std::string, file_manager>,
		     public singleton <file_manager>
{
    friend class singleton <file_manager>;
    friend class tree_node <std::string, file_manager>;

    /* TODO: Fix the tre node class so it does not require constructor or destructor
     * calling on foreign classes. */
    
    /** Hidden constructor. */
    file_manager () {}

    /** Hidden destructor. */
    ~file_manager () {}
    
public:    
    template <class PathPredicate>
    void find_if_all (PathPredicate pred,
		      file_finder::path_list& res) const;

    boost::filesystem::path find_all (const boost::filesystem::path& file) const;
};


template<class StringPredicate>
void file_manager::find_if_all (StringPredicate pred,
				file_finder::path_list& res) const
{
    const_iterator it;

    res = find_if (pred, res);
    
    for (it = begin(); it != end(); ++it)
	(*it)->find_if_all (pred, res);
}

} /* namespace psynth */

#endif /* PSYNTH_FILEMANAGER_H */
