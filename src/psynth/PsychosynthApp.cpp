/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007, 2008                    *
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

#include "psynth/OutputDirectorAlsa.h"
#include "psynth/OutputDirectorOss.h"
#include "psynth/OutputDirectorJack.h"
#include "psynth/PsychosynthApp.h"

#include "common/ConfBackendXML.h"

using namespace std;

int PsychosynthApp::run(int argc, const char* argv[])
{
    int ret_val;

    Config::instance().attachBackend(new ConfBackendXML("test_config.xml"));
    Config::instance().load();
    
    m_director.attachOutputDirectorFactory(new OutputDirectorAlsaFactory);
    m_director.attachOutputDirectorFactory(new OutputDirectorOssFactory);
    m_director.attachOutputDirectorFactory(new OutputDirectorJackFactory);
      
    m_director.start(Config::instance().getChild("psychosynth"));
    
    ret_val = execute();

    m_director.stop();

    Config::instance().save();
    
    return ret_val;
}

