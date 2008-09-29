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

#include "version.hpp"

#include "common/logger.hpp"
#include "common/arg_parser.hpp"
#include "common/option_conf.hpp"
#ifdef PSYNTH_HAVE_XML
#include "common/conf_backend_xml.hpp"
#endif
#ifdef PSYNTH_HAVE_ALSA
#include "psynth/output_director_alsa.hpp"
#endif
#ifdef PSYNTH_HAVE_OSS
#include "psynth/output_director_oss.hpp"
#endif
#ifdef PSYNTH_HAVE_JACK
#include "psynth/output_director_jack.hpp"
#endif

#include "psynth/psynth_app.hpp"

using namespace std;
namespace bf = boost::filesystem;

namespace psynth
{

void psynth_app::generate_paths ()
{
#ifdef WIN32
    m_cfg_dir = ".";
#else
    bf::path home_dir = getenv  ("HOME");
    m_cfg_dir = home_dir / ".psychosynth";
    if (!bf::exists (m_cfg_dir)) {
	bf::create_directory (m_cfg_dir);
	bf::create_directory (m_cfg_dir / "samples");
    }
#endif

    m_data_dir = PSYNTH_DATA_DIR;
}

bool psynth_app::parse_args (int argc, const char* argv[])
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

    prepare (ap);
    ap.parse (argc, argv);

    if (show_help) {
	print_help ();
	return false;
    }

    if (show_version) {
	print_version ();
	return false;
    }

    return true;
}

bf::path psynth_app::get_config_path ()
{
    return m_cfg_dir;
}

bf::path psynth_app::get_data_path ()
{
    return m_data_dir;
}

void psynth_app::print_base_options (ostream& out)
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

void psynth_app::setup_synth()
{
    m_director.start (config::instance ().get_child ("psychosynth"),
		      get_config_path ());
}
    
void psynth_app::close_synth ()
{
    m_director.stop();
}

int psynth_app::run (int argc, const char* argv[])
{   
    int ret_val;
 
    conf_node& conf = config::instance().get_child ("psychosynth");
    logger::instance().attach_sink (new log_default_sink);

    if (!parse_args (argc, argv))
	return ERR_GENERIC;

    generate_paths();
    
#ifdef PSYNTH_HAVE_XML
    conf.attach_backend (new conf_backend_xml ((get_config_path () / "psychosynth.xml").file_string ()));
#endif
    conf.def_load();

#ifdef PSYNTH_HAVE_ALSA
    m_director.attach_output_director_factory (new output_director_alsa_factory);
#endif
#ifdef PSYNTH_HAVE_OSS
    m_director.attach_output_director_factory (new output_director_oss_factory);
#endif
#ifdef PSYNTH_HAVE_JACK
    m_director.attach_output_director_factory (new output_director_jack_factory);
#endif
    
    ret_val = execute();

    if (ret_val == SUCCESS)
	conf.save ();
    
    return ret_val;
}

} /* namespace psynth */
