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
#include "version.h"
#include "common/file_manager.h"
#include "psynth/file_manager_director.h"

using namespace std;

namespace psynth
{

bool file_manager_director::on_conf_nudge (conf_node& node)
{
    file_manager& mgr =
	file_manager::instance()
	.get_child ("psychosynth")
	.get_child (node.get_name());
    
    string val;
    
    mgr.clear();
    for (conf_node::iterator it = node.begin(); it != node.end(); ++it) {
	(*it)->get(val);
	mgr.add_path(val);
    }
    
    return true;
}

void file_manager_director::register_config ()
{
    m_conf->get_child ("samples")
	.add_nudge_event (MakeDelegate(this, &file_manager_director::on_conf_nudge));
    m_conf->get_child ("samples").nudge();
}

void file_manager_director::unregister_config()
{
    m_conf->get_child ("samples")
	.delete_nudge_event (MakeDelegate(this, &file_manager_director::on_conf_nudge));
}

void file_manager_director::start (conf_node& conf,
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
    m_conf->get_child ("samples").get_child ("path0").def(
	(boost::filesystem::path (PSYNTH_DATA_DIR) / "samples").file_string ());
    m_conf->get_child ("samples").get_child ("path1").def(
	(m_home_path / "samples").file_string ());
}

} /* namespace psynth */
