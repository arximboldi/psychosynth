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

#ifndef PSYNTH_FILE_FINDER_H
#define PSYNTH_FILE_FINDER_H

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <cstdlib>

#include <dirent.h>

#include <libpsynth/common/misc.h>

namespace psynth
{

class string_predicate
{
public:
    virtual bool operator () (const std::string& str) = 0;
};

template <class IterType>
class has_extension : public string_predicate
{
    IterType m_first;
    IterType m_last;

public:
    has_extension (IterType first, IterType last) :
	m_first(first),
	m_last(last)
	{};
    
    bool operator () (const std::string& str) {
	const char* ext = get_extension(str.c_str());
	for (IterType i = m_first; i != m_last; ++i)
	    if (!strcmp_i(ext, std::string(*i).c_str()))
		return true;
	return false;
    }
};


template<class IterType>
has_extension <IterType> make_has_extension (IterType first, IterType last)
{
    return has_extension <IterType> (first, last);
}

class file_finder
{
    std::list<std::string> m_paths;
    std::map<std::string, std::string> m_cache;
    bool m_cache_updated;
    bool m_cache_auto;

    void cache_path (const std::string& path);
    void uncache_path (const std::string& path);

    std::string find_in (const std::string& path,
			 const std::string& file) const;
    
    template<class string_predicate>
    int get_file_list (const std::string& folder,
		       std::list<std::string>& res,
		       string_predicate pred) const;
    
public:
    file_finder() :
	m_cache_updated(false),
	m_cache_auto(false)
	{}
    
    void add_path (const std::string& path);
    void del_path (const std::string& path);
    void build_cache (bool autoupdate);
    void clear_cache ();
    void clear ();

    std::string find (const std::string& file) const;

    template<class string_predicate>
    void find_if (string_predicate pred,
		 std::list<std::string>& res) const; 
};

template<class string_predicate>
int file_finder::get_file_list(const std::string& folder,
			     std::list<std::string>& res,
			     string_predicate pred) const
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
		pred (std::string(namelist[n]->d_name))) {
		res.push_back(curr);
	    }
	    
	    free(namelist[n]);
       	}
	
       	free(namelist);
    }
    
    return n;
}

template<class string_predicate>
void file_finder::find_if(string_predicate pred,
			 std::list<std::string>& result) const
{
    if (!m_cache_updated) {
	std::list<std::string>::const_iterator iter;
	for (iter = m_paths.begin(); iter != m_paths.end(); ++iter) {
	    get_file_list (*iter, result, pred);
	}
    } else {
	std::map<std::string, std::string>::const_iterator iter;
	for (iter = m_cache.begin(); iter != m_cache.end(); ++iter)
	    if (pred (iter->first))
		result.push_back(iter->second);
    }
}

} /* namespace psynth */

#endif /* PSYNTH_FILEFINDER_H */

