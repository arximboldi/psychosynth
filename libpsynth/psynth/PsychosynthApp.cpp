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

#include "common/Logger.h"
#include "common/ConfBackendXML.h"
#include "common/OptionConf.h"

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

std::string getConfigPath()
{
#ifdef WIN32
    return "";
#else
    char* home_dir = getenv("HOME");
    return std::string(home_dir) + ".psychosynth";
#endif
}

std::string getDataPath()
{
    return PSYNTH_DATA_DIR;
}

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

int PsychosynthApp::run(int argc, const char* argv[])
{   
    ArgParser arg_parser;
    int ret_val;
    
    bool show_help = false;
    bool show_version = false;

    ConfNode& conf = Config::instance().getChild("psychosynth");
    Logger::instance().attachSink(new LogDefaultSink);

    arg_parser.add('h', "help", &show_help);
    arg_parser.add('v', "version", &show_version);
    arg_parser.add('s', "sample-rate", new OptionConf<int>(conf.getChild("sample_rate")));
    arg_parser.add('b', "buffer-size", new OptionConf<int>(conf.getChild("buffer_size")));
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
	return -1;
    }

    if (show_version) {
	printVersion();
	return -1;
    }
    
    conf.attachBackend(new ConfBackendXML("test_config.xml"));
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
    
    m_director.start(conf);
    ret_val = execute();
    m_director.stop();

    if (ret_val == 0)
	conf.save();
    
    return ret_val;
}

} /* namespace psynth */
