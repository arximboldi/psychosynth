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

#include <cstring>
#include <algorithm>
#include <libpsynth/version.h>
#include <sys/stat.h>

#include "common/logger.h"
#include "common/arg_parser.h"
#include "common/option_conf.h"
#ifdef PSYNTH_HAVE_XML
#include "common/conf_backend_xml.h"
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
    if (access(m_cfg_dir.c_str(), F_OK) < 0) {
	mkdir(m_cfg_dir.c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
	mkdir((m_cfg_dir + "samples").c_str(), S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    }
#endif
}

bool PsychosynthApp::parseArgs(int argc, const char* argv[])
{
    arg_parser ap;
    conf_node& conf = config::instance().get_child ("psychosynth");
    bool show_help = false;
    bool show_version = false;
    
    ap.add('h', "help", &show_help);
    ap.add('v', "version", &show_version);
    ap.add('s', "sample-rate", new option_conf<int>(conf.get_child ("sample_rate")));
    ap.add('b', "buffer-size", new option_conf<int>(conf.get_child ("block_size")));
    ap.add('c', "channels", new option_conf<int>(conf.get_child ("num_channels")));
    ap.add('o', "output", new option_conf<string>(conf.get_child ("output")));

#ifdef PSYNTH_HAVE_ALSA
    ap.add(0, "alsa-device", new option_conf<string>(conf.get_path ("alsa/out_device")));
#endif
#ifdef PSYNTH_HAVE_OSS
    ap.add(0, "oss-device", new option_conf<string>(conf.get_path ("oss/out_device")));
#endif
#ifdef PSYNTH_HAVE_JACK
    ap.add(0, "jack-server", new option_conf<string>(conf.get_path ("jack/server")));
#endif

    prepare(ap);
    ap.parse(argc, argv);

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
    m_director.start(config::instance().get_child ("psychosynth"),
		     getConfigPath());
}
    
void PsychosynthApp::closeSynth()
{
    m_director.stop();
}

int PsychosynthApp::run(int argc, const char* argv[])
{   
    int ret_val;
 
    conf_node& conf = config::instance().get_child ("psychosynth");
    logger::instance().attach_sink (new log_default_sink);

    if (!parseArgs(argc, argv))
	return ERR_GENERIC;

    generatePaths();
    
#ifdef PSYNTH_HAVE_XML
    conf.attach_backend(new conf_backend_xml(getConfigPath() + "psychosynth.xml"));
#endif
    conf.def_load();

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
	conf.save ();
    
    return ret_val;
}

} /* namespace psynth */
