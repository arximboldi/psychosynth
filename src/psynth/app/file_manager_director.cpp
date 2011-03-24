/**
 *  Time-stamp:  <2011-03-24 18:26:19 raskolnikov>
 *
 *  @file        file_manager_director.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  A file management facility. Implementation.
 *  This file is part of the Psychosynth project.
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
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

#include "version.hpp"
#include "base/file_manager.hpp"
#include "app/file_manager_director.hpp"

using namespace std;

namespace psynth
{

void file_manager_director::on_conf_nudge (base::conf_node& node)
{
    base::file_mgr_node& mgr =
	base::file_manager::self ()
	.child ("psychosynth")
	.child (node.name());
    
    string val;
    
    mgr.clear();
    for (base::conf_node::iterator it = node.begin(); it != node.end(); ++it) {
	it->get (val);
	mgr.add_path(val);
    }
}

void file_manager_director::register_config ()
{
    m_on_conf_nudge_slot = 
	m_conf->child ("samples").on_nudge.connect
	(boost::bind (&file_manager_director::on_conf_nudge, this, _1));
    m_conf->child ("samples").nudge ();
}

void file_manager_director::unregister_config()
{
    m_on_conf_nudge_slot.disconnect ();
}

void file_manager_director::start (base::conf_node& conf,
				   const boost::filesystem::path& home_path)
{
    m_conf = &conf;
    m_home_path = home_path;
    
    register_config ();
    defaults();
}

void file_manager_director::stop ()
{
    unregister_config ();
    m_conf = 0;
}

void file_manager_director::defaults ()
{
    m_conf->child ("samples").child ("path0").def(
	(boost::filesystem::path (PSYNTH_DATA_DIR) / "samples").string ());
    m_conf->child ("samples").child ("path1").def(
	(m_home_path / "samples").string ());
}

} /* namespace psynth */
