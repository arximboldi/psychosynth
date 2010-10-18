/**
 *  Time-stamp:  <2010-10-17 19:52:29 raskolnikov>
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

#ifndef PSYNTH_FILE_FINDER_H
#define PSYNTH_FILE_FINDER_H

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include <psynth/base/misc.hpp>

namespace psynth
{
namespace base
{

class path_predicate
{
public:
    virtual bool operator () (const boost::filesystem::path& str) = 0;
};

template <class IterType>
class has_extension : public path_predicate
{
    IterType m_first;
    IterType m_last;

public:
    has_extension (IterType first, IterType last) :
	m_first(first),
	m_last(last)
	{};
    
    bool operator () (const boost::filesystem::path& path) {
	for (IterType i = m_first; i != m_last; ++i)
	    if (extension (path) != *i)
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
public:
    typedef std::list <boost::filesystem::path> path_list;
    typedef std::map <boost::filesystem::path,
		      boost::filesystem::path> path_map;

    file_finder ()
	: m_cache_updated (false)
	, m_cache_auto (false)
    {}
    
    void add_path (const boost::filesystem::path& path);
    void del_path (const boost::filesystem::path& path);
    void build_cache (bool autoupdate);
    void clear_cache ();
    void clear ();

    boost::filesystem::path
    find (const boost::filesystem::path& file) const;

    template <class PathPredicate>
    void find_if (PathPredicate pred, path_list & res) const; 

private:
    path_list m_paths;
    path_map m_cache;
    
    bool m_cache_updated;
    bool m_cache_auto;

    void cache_path (const boost::filesystem::path& path);
    void uncache_path (const boost::filesystem::path& path);

    boost::filesystem::path find_in (const boost::filesystem::path& path,
				     const boost::filesystem::path& file) const;

    template<class path_predicate>
    int get_file_list (const boost::filesystem::path& folder,
		       path_list& res,
		       path_predicate pred) const;
};


template<class path_predicate>
int file_finder::get_file_list (const boost::filesystem::path& folder,
				path_list& res,
				path_predicate pred) const
{
    int n = 0;
    
    if (!boost::filesystem::exists (folder))
	return n;

    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr (folder);
	 itr != end_itr;
	 ++itr) {
	if (pred (itr->path ())) {
	    res.push_back (itr->path ());
	    ++n;
	}
    }
    
    return n;
}

template<class path_predicate>
void file_finder::find_if (path_predicate pred, path_list& result) const
{
    if (!m_cache_updated) {
	path_list::const_iterator iter;
	for (iter = m_paths.begin(); iter != m_paths.end(); ++iter)
	    get_file_list (*iter, result, pred);
    } else {
	path_map::const_iterator iter;
	for (iter = m_cache.begin(); iter != m_cache.end(); ++iter)
	    if (pred (iter->first))
		result.push_back (iter->second);
    }
}

} /* namespace base */
} /* namespace psynth */

#endif /* PSYNTH_FILEFINDER_H */
