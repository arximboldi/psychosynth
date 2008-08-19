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

#include <cstring>
#include <cstdlib>
#include "common/file_finder.h"
#include "common/misc.h"

using namespace std;

namespace psynth
{

std::string file_finder::find_in(const std::string& path,
			       const std::string& file) const
{
    struct dirent **namelist;
    std::string result;
    int n;
    bool finished = false;
    
    n = scandir(path.c_str(), &namelist, 0, alphasort);
    
    if (n >= 0) {
	while(n-- && !finished) {
	    if ((strcmp(namelist[n]->d_name, ".") != 0) &&
		(strcmp(namelist[n]->d_name, "..") != 0) &&
		(strcmp(namelist[n]->d_name, file.c_str()) == 0)) {
		result = path + "/" + namelist[n]->d_name;
		finished = true;
	    }
	    free(namelist[n]);
       	}
       	free(namelist);
    }

    return result;
}

void file_finder::cache_path (const std::string& path)
{
    struct dirent **namelist;
    int n;
    
    n = scandir(path.c_str(), &namelist, 0, alphasort);
    
    if (n >= 0) {
	while(n--) {
	    if ((strcmp(namelist[n]->d_name, ".") != 0) &&
		(strcmp(namelist[n]->d_name, "..") != 0)) {
		m_cache.insert(make_pair(string(namelist[n]->d_name),
					 string(path + namelist[n]->d_name)));
	    }
	    free(namelist[n]);
       	}
       	free(namelist);
    }
}

void file_finder::uncache_path (const std::string& path)
{
    map<string,string>::iterator iter;
    
    for (iter = m_cache.begin(); iter != m_cache.end();)
	if (path == dirname_str (iter->second))
	    m_cache.erase(iter++);
	else
	    ++iter;
}

void file_finder::add_path(const std::string& path)
{
    m_paths.push_back(path);
    if (m_cache_auto)
	cache_path(path);
    else
	m_cache_updated = false;
}

void file_finder::del_path(const std::string& path)
{
    m_paths.remove(path);
    if (m_cache_auto)
	uncache_path(path);
    else
	m_cache_updated = false;
}

void file_finder::build_cache(bool autoupdate)
{
    if (!m_cache_updated) {
	m_cache_updated = true;
	m_cache_auto = autoupdate;
	for (list<string>::iterator it = m_paths.begin(); it != m_paths.end(); ++it)
	    cache_path(*it);
    }
}

void file_finder::clear_cache()
{
    m_cache.clear();
    m_cache_updated = false;
    m_cache_auto = false;
}

void file_finder::clear()
{
    m_paths.clear();
    clear_cache();
}

std::string file_finder::find(const std::string& file) const
{
    if (!m_cache_updated) {
	list<string>::const_iterator iter;
	std::string res;
	for (iter = m_paths.begin(); iter != m_paths.end(); ++iter) {
	    res = find_in(*iter, file);
	    if (!res.empty())
		return res;
	}
    } else {
	map<string,string>::const_iterator iter;
	iter = m_cache.find(file);
	if (iter != m_cache.end())
	    return iter->second;
    }
    
    return string();
}

} /* namespace psynth */

