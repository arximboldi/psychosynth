/**
 *  Time-stamp:  <2009-04-27 16:48:10 raskolnikov>
 *
 *  @file        file_finder.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  Tool to find files in a filesystem.
 */

/*
 *  Copyright (C) 2008 Juan Pedro Bolívar Puente
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
#include "base/file_finder.hpp"
#include "base/misc.hpp"

using namespace std;
namespace bf = boost::filesystem;

namespace psynth
{

bf::path file_finder::find_in (const bf::path& dir_path, const bf::path& file) const
{    
    if (!bf::exists (dir_path))
	return bf::path ();

    bf::directory_iterator end_itr;
    for (bf::directory_iterator itr (dir_path); itr != end_itr; ++itr) {
	if (itr->path().leaf() == file)
	    return itr->path ();
    }

    return bf::path ();
}

void file_finder::cache_path (const bf::path& dir_path)
{    
    if (!bf::exists (dir_path))
	return;

    bf::directory_iterator end_itr;
    for (bf::directory_iterator itr (dir_path); itr != end_itr; ++itr)
	m_cache.insert (make_pair (itr->path().leaf(), itr->path ()));    
}

void file_finder::uncache_path (const bf::path& path)
{
    path_map::iterator iter;
    
    for (iter = m_cache.begin(); iter != m_cache.end();)
	if (path == iter->second)
	    m_cache.erase (iter++);
	else
	    ++iter;
}

void file_finder::add_path (const bf::path& path)
{
    m_paths.push_back (path);
    if (m_cache_auto)
	cache_path (path);
    else
	m_cache_updated = false;
}

void file_finder::del_path (const bf::path& path)
{
    m_paths.remove (path);
    if (m_cache_auto)
	uncache_path(path);
    else
	m_cache_updated = false;
}

void file_finder::build_cache (bool autoupdate)
{
    if (!m_cache_updated) {
	m_cache_updated = true;
	m_cache_auto = autoupdate;
	for (path_list::iterator it = m_paths.begin(); it != m_paths.end(); ++it)
	    cache_path (*it);
    }
}

void file_finder::clear_cache ()
{
    m_cache.clear();
    m_cache_updated = false;
    m_cache_auto = false;
}

void file_finder::clear ()
{
    m_paths.clear();
    clear_cache();
}

bf::path file_finder::find (const bf::path& file) const
{
    if (!m_cache_updated) {
	path_list::const_iterator iter;
	bf::path res;
	for (iter = m_paths.begin(); iter != m_paths.end(); ++iter) {
	    res = find_in (*iter, file);
	    if (!res.empty())
		return res;
	}
    } else {
	path_map::const_iterator iter;
	iter = m_cache.find (file);
	if (iter != m_cache.end ())
	    return iter->second;
    }
    
    return bf::path ();
}

} /* namespace psynth */

