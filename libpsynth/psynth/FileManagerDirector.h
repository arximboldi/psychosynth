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

#include <libpsynth/common/Config.h>
#include <libpsynth/common/FileManager.h>

namespace psynth
{

class FileManagerDirector
{
    ConfNode* m_conf;
    std::string m_home_path;
    
    bool onConfNudge(ConfNode& node);
    void registerConfig();
    void unregisterConfig();
    
public:
    void start(ConfNode& conf,
	       const std::string& home_path);
    void stop();
    void defaults();
};

} /* namespace psynth */


#endif /* PSYNTH_FILEMANAGERDIRECTOR_H */
