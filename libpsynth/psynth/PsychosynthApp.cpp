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

#include <libpsynth/version.h>
#include <sys/stat.h>

#include "common/Logger.h"
#include "common/OptionConf.h"
#ifdef PSYNTH_HAVE_XML
#include "common/ConfBackendXML.h"
#endif
#ifdef PSYNTH_HAVE_ALSA
#include "psynth/OutputDirectorAlsa.h"
#endif
#ifdef PSYNTH_HAVE_OSS
#include "psynth/OutputDirectorOss.h"
#endif
#ifdef PSYNTH_HAVE_JACK
#include "psynth/OutputDirectorJack.h"
#endif

#include "psynth/PsychosynthApp.h"

using namespace std;

namespace psynth
{

void PsychosynthApp::generatePaths()
{
#ifdef WIN32
    m_cfg_dir = "./";
#else
    char* home_dir = getenv("HOME");
    m_cfg_dir = std::string(home_dir) + "/.psychosynth/";
    if (access(m_cfg_dir.c_str(), F_OK) < 0)
	mkdir(m_cfg_dir.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
#endif
}

bool PsychosynthApp::parseArgs(int argc, const char* argv[])
{
    ArgParser arg_parser;
    ConfNode& conf = Config::instance().getChild("psychosynth");
    bool show_help = false;
    bool show_version = false;
    
    arg_parser.add('h', "help", &show_help);
    arg_parser.add('v', "version", &show_version);
    arg_parser.add('s', "sample-rate", new OptionConf<int>(conf.getChild("sample_rate")));
    arg_parser.add('b', "buffer-size", new OptionConf<int>(conf.getChild("block_size")));
    arg_parser.add('c', "channels", new OptionConf<int>(conf.getChild("num_channels")));
    arg_parser.add('o', "output", new OptionConf<string>(conf.getChild("output")));

#ifdef PSYNTH_HAVE_ALSA
    arg_parser.add(0, "alsa-device", new OptionConf<string>(conf.getPath("alsa/out_device")));
#endif
#ifdef PSYNTH_HAVE_OSS
    arg_parser.add(0, "oss-device", new OptionConf<string>(conf.getPath("oss/out_device")));
#endif
#ifdef PSYNTH_HAVE_JACK
    arg_parser.add(0, "jack-server", new OptionConf<string>(conf.getPath("jack/server")));
#endif

    prepare(arg_parser);
    arg_parser.parse(argc, argv);

    if (show_help) {
	printHelp();
	return false;
    }

    if (show_version) {
	printVersion();
	return false;
    }

    return true;
}

std::string PsychosynthApp::getConfigPath()
{
    return m_cfg_dir;
}

std::string PsychosynthApp::getDataPath()
{
    return string(PSYNTH_DATA_DIR) + "/";
}

void PsychosynthApp::printBaseOptions(ostream& out)
{
    out <<
	"  -h, --help                 Display this information.\n"
	"  -v, --version              Display program version.\n"
	"  -s, --sample-rate <value>  Set synth sample rate.\n"
	"  -b, --buffer-size <value>  Set buffer size. Low values may cause clicking but\n"
	"                             high values rise latency.\n"
	"  -c, --channels <value>     Set the number of channels.\n"
	"  -o, --output <system>      Set the preferred audio output system.\n"
#ifdef PSYNTH_HAVE_ALSA
	"  --alsa-device <device>     Set the ALSA playback device.\n"
#endif
#ifdef PSYNTH_HAVE_OSS
	"  --oss-device <device>      Set the OSS playback device.\n"
#endif
#ifdef PSYNTH_HAVE_JACK
	"  --jack-server <server>     Set the Jack server daemon.\n"
#endif
	;
}

void PsychosynthApp::setupSynth()
{
    m_director.start(Config::instance().getChild("psychosynth"),
		     getConfigPath());
}
    
void PsychosynthApp::closeSynth()
{
    m_director.stop();
}

int PsychosynthApp::run(int argc, const char* argv[])
{   
    int ret_val;
 
    ConfNode& conf = Config::instance().getChild("psychosynth");
    Logger::instance().attachSink(new LogDefaultSink);

    if (!parseArgs(argc, argv))
	return ERR_GENERIC;

    generatePaths();
    
#ifdef PSYNTH_HAVE_XML
    conf.attachBackend(new ConfBackendXML(getConfigPath() + "psychosynth.xml"));
#endif
    conf.defLoad();

#ifdef PSYNTH_HAVE_ALSA
    m_director.attachOutputDirectorFactory(new OutputDirectorAlsaFactory);
#endif
#ifdef PSYNTH_HAVE_OSS
    m_director.attachOutputDirectorFactory(new OutputDirectorOssFactory);
#endif
#ifdef PSYNTH_HAVE_JACK
    m_director.attachOutputDirectorFactory(new OutputDirectorJackFactory);
#endif
    
    ret_val = execute();

    if (ret_val == SUCCESS)
	conf.save();
    
    return ret_val;
}

} /* namespace psynth */
