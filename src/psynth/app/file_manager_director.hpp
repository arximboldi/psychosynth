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

#ifndef PSYNTH_FILEMANAGERDIRECTOR_H
#define PSYNTH_FILEMANAGERDIRECTOR_H

#include <psynth/base/config.hpp>
#include <psynth/base/file_manager.hpp>

namespace psynth
{

class file_manager_director
{
    base::conf_node* m_conf;
    boost::filesystem::path m_home_path;
    sigc::connection m_on_conf_nudge_slot;
    
    void on_conf_nudge (base::conf_node& node);
    void register_config ();
    void unregister_config ();
    
public:
    void start (base::conf_node& conf,
	        const boost::filesystem::path& home_path);
    void stop ();
    void defaults ();
};

} /* namespace psynth */


#endif /* PSYNTH_FILEMANAGERDIRECTOR_H */
