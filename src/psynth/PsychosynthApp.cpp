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

#include "common/Logger.h"
#include "common/ConfBackendXML.h"

using namespace std;

void PsychosynthApp::printPsynthOptions(ostream& out)
{
    out <<
	"  -h, --help                 Display this information.\n"
	"  -v, --version              Display program version.\n"
	"  -s, --sample-rate <value>  Set synth sample rate.\n"
	"  -b, --buffer-size <value>  Set buffer size. Low values may cause clicking but\n"
	"                             high values rise latency.\n"
	"  -c, --channels <value>     Set the number of channels.\n"
	"  -o, --output <system>      Set the preferred audio output system.\n"
	"  --alsa-device <device>     Set the ALSA playback device.\n"
	"  --oss-device <device>      Set the OSS playback device.\n"
	"  --jack-server <server>     Set the Jack server daemon.\n";
}

int PsychosynthApp::run(int argc, const char* argv[])
{
    ArgParser arg_parser;
    int ret_val;
    
    bool is_help;
    bool is_version;

    Logger::instance().attachSink(new LogDefaultSink);
    Config::instance().getChild("psychosynth").attachBackend(new ConfBackendXML("test_config.xml"));

    Config::instance().defLoad();
    
    m_director.attachOutputDirectorFactory(new OutputDirectorAlsaFactory);
    m_director.attachOutputDirectorFactory(new OutputDirectorOssFactory);
    m_director.attachOutputDirectorFactory(new OutputDirectorJackFactory);
      
    m_director.start(Config::instance().getChild("psychosynth"));
    
    ret_val = execute();

    m_director.stop();

    Config::instance().save();
    
    return ret_val;
}
