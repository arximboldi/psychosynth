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

#include "version.h"
#include "common/FileManager.h"
#include "psynth/FileManagerDirector.h"

using namespace std;

namespace psynth
{

bool FileManagerDirector::onConfNudge(ConfNode& node)
{
    FileManager& mgr = FileManager::instance().getChild("psychosynth").getChild(node.getName());
    string val;
    
    mgr.clear();
    for (ConfNode::ChildIter it = node.begin(); it != node.end(); ++it) {
	(*it)->get(val);
	mgr.addPath(val);
    }
    
    return true;
}

void FileManagerDirector::registerConfig()
{
    m_conf->getChild("samples").addNudgeEvent(MakeDelegate(this, &FileManagerDirector::onConfNudge));
    m_conf->getChild("samples").nudge();
}

void FileManagerDirector::unregisterConfig()
{
    m_conf->getChild("samples").deleteNudgeEvent(MakeDelegate(this, &FileManagerDirector::onConfNudge));
}

void FileManagerDirector::start(ConfNode& conf,
				const std::string& home_path)
{
    m_conf = &conf;
    m_home_path = home_path;
    
    registerConfig();
    defaults();
}

void FileManagerDirector::stop()
{
    unregisterConfig();
    m_conf = 0;
}

void FileManagerDirector::defaults()
{
    m_conf->getChild("samples").getChild("path0").def(string(PSYNTH_DATA_DIR) + "/samples");
    m_conf->getChild("samples").getChild("path1").def(m_home_path + "/samples");
}

} /* namespace psynth */
