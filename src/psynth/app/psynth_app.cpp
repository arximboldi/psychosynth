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

#include "base/logger.hpp"
#include "base/arg_parser.hpp"
#include "base/option_conf.hpp"
#ifdef PSYNTH_HAVE_XML
#include "base/conf_backend_xml.hpp"
#endif
#ifdef PSYNTH_HAVE_ALSA
#include "app/output_director_alsa.hpp"
#endif
#ifdef PSYNTH_HAVE_OSS
#include "app/output_director_oss.hpp"
#endif
#ifdef PSYNTH_HAVE_JACK
#include "app/output_director_jack.hpp"
#endif

#include "app/psynth_app.hpp"

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
    base::arg_parser ap;
    base::conf_node& conf = base::config::self ().child ("psychosynth");
    bool show_help = false;
    bool show_version = false;
    
    ap.add('h', "help", &show_help);
    ap.add('v', "version", &show_version);
    ap.add('s', "sample-rate", new base::option_conf<int>(conf.child ("sample_rate")));
    ap.add('b', "buffer-size", new base::option_conf<int>(conf.child ("block_size")));
    ap.add('c', "channels", new base::option_conf<int>(conf.child ("num_channels")));
    ap.add('o', "output", new base::option_conf<string>(conf.child ("output")));

#ifdef PSYNTH_HAVE_ALSA
    ap.add(0, "alsa-device", new base::option_conf<string>(conf.path ("alsa.out_device")));
#endif
#ifdef PSYNTH_HAVE_OSS
    ap.add(0, "oss-device", new base::option_conf<string>(conf.path ("oss.out_device")));
#endif
#ifdef PSYNTH_HAVE_JACK
    ap.add(0, "jack-server", new base::option_conf<string>(conf.path ("jack.server")));
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
    m_director.start (base::config::self  ().child ("psychosynth"),
		      get_config_path ());
}
    
void psynth_app::close_synth ()
{
    m_director.stop();
}

int psynth_app::run (int argc, const char* argv[])
{   
    int ret_val = 0;

    try {
	base::conf_node& conf = base::config::self ().child ("psychosynth");
	base::logger::self ().add_sink (base::new_log_std_sink ());
	
	if (!parse_args (argc, argv))
	    return ERR_GENERIC;

	generate_paths();

	try {
#ifdef PSYNTH_HAVE_XML
	    conf.set_backend (
		base::new_conf_backend_xml (
		    (get_config_path () / "psychosynth.xml").file_string ()));
#endif
	    conf.def_load();
	} catch (psynth::base::exception& error) {
	    error.log ();
	}
    
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

	try {
	    if (ret_val == SUCCESS)
		conf.save ();
	} catch (psynth::base::exception& error) {
	    error.log ();
	}
    } catch (psynth::base::exception& error) {
	error.log ();
	ret_val = -1;
    }
    
    return ret_val;
}

} /* namespace psynth */
