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

#include <libpsynth/common/config.h>
#include <libpsynth/common/file_manager.h>

namespace psynth
{

class file_manager_director
{
    conf_node* m_conf;
    std::string m_home_path;
    
    bool on_conf_nudge (conf_node& node);
    void register_config ();
    void unregister_config ();
    
public:
    void start (conf_node& conf,
	        const std::string& home_path);
    void stop ();
    void defaults ();
};

} /* namespace psynth */


#endif /* PSYNTH_FILEMANAGERDIRECTOR_H */
