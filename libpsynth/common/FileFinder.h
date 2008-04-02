/***************************************************************************
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

#ifndef PSYNTH_FILEFINDER_H
#define PSYNTH_FILEFINDER_H

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <libpsynth/common/Misc.h>
#include <dirent.h>

namespace psynth
{

class StringPredicate
{
public:
    virtual bool operator() (const std::string& str) = 0;
};

template <class IterType>
class HasExtension
{
    IterType m_first;
    IterType m_last;

public:
    HasExtension(IterType first, IterType last) :
	m_first(first),
	m_last(last)
	{};
    
    bool operator() (const std::string& str) {
	const char* ext = getExtension(str.c_str());
	for (IterType i = m_first; i != m_last; ++i)
	    if (!strcmp_i(ext, std::string(*i).c_str()))
		return true;
	return false;
    }
};


template<class IterType>
HasExtension<IterType>  MakeHasExtension(IterType first, IterType last)
{
    return HasExtension<IterType>(first, last);
}

class FileFinder
{
    std::list<std::string> m_paths;
    std::map<std::string, std::string> m_cache;
    bool m_cache_updated;
    bool m_cache_auto;

    void cachePath(const std::string& path);
    void uncachePath(const std::string& path);

    std::string findIn(const std::string& path,
		       const std::string& file) const;
    
    template<class StringPredicate>
    int getFileList(const std::string& folder,
		    std::list<std::string>& res,
		    StringPredicate pred) const;
    
public:
    FileFinder() :
	m_cache_updated(false),
	m_cache_auto(false)
	{}
    
    void addPath(const std::string& path);
    void delPath(const std::string& path);
    void buildCache(bool autoupdate);
    void clearCache();
    void clear();

    std::string find(const std::string& file) const;

    template<class StringPredicate>
    void findIf(StringPredicate pred,
		std::list<std::string>& res) const; 
};

template<class StringPredicate>
int FileFinder::getFileList(const std::string& folder,
			    std::list<std::string>& res,
			    StringPredicate pred) const
{
    struct dirent **namelist;
    std::string curr;
    int n;
    
    n = scandir(folder.c_str(), &namelist, 0, alphasort);
    
    if (n >= 0) {
	while(n--) {
	    curr = folder + "/" + namelist[n]->d_name;

	    if ((strcmp(namelist[n]->d_name, ".") != 0) &&
		(strcmp(namelist[n]->d_name, "..") != 0) &&
		pred(std::string(namelist[n]->d_name))) {
		res.push_back(curr);
	    }   
	    free(namelist[n]);
       	}
       	free(namelist);
    }
    
    return n;
}

template<class StringPredicate>
void FileFinder::findIf(StringPredicate pred,
			std::list<std::string>& result) const
{   
    if (!m_cache_updated) {
	std::list<std::string>::const_iterator iter;
	for (iter = m_paths.begin(); iter != m_paths.end(); ++iter) {
	    getFileList(*iter, result, pred);
	}
    } else {
	std::map<std::string, std::string>::const_iterator iter;
	for (iter = m_cache.begin(); iter != m_cache.end(); ++iter)
	    if (pred(iter->first))
		result.push_back(iter->second);
    }
}

} /* namespace psynth */

#endif /* PSYNTH_FILEFINDER_H */

