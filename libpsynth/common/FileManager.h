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

#include <libpsynth/common/FileFinder.h>
#include <libpsynth/common/Tree.h>
#include <libpsynth/common/Singleton.h>

namespace psynth
{

class FileManager : public FileFinder,
		    public TreeNode<std::string, FileManager>,
		    public Singleton<FileManager>
{
    friend class Singleton<FileManager>;
    friend class TreeNode<std::string, FileManager>;

    /* TODO: Fix the tre node class so it does not require constructor or destructor
     * calling on foreign classes. */
    
    /** Hidden constructor. */
    FileManager() {}

    /** Hidden destructor. */
    ~FileManager() {}
    
public:    
    template<class StringPredicate>
    void findIfAll(StringPredicate pred,
		   std::list<std::string>& res) const;

    std::string findAll(const std::string& file) const;
};


template<class StringPredicate>
void FileManager::findIfAll(StringPredicate pred,
			    std::list<std::string>& res) const
{
    ConstChildIter it;

    res = findIf(pred, res);
    for (it = begin(); it != end(); ++it)
	(*it)->findIfAll(pred, res);
}

} /* namespace psynth */

#endif /* PSYNTH_FILEMANAGER_H */
