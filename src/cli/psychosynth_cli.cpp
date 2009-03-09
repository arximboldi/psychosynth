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

#include <algorithm>
#include <config.h>

#include <psynth/net/osc_client.hpp>
#include <psynth/net/osc_server.hpp>
#include <psynth/net/osc_client_logger.hpp>
#include <psynth/net/osc_server_logger.hpp>
#include <psynth/base/timer.hpp>
#include <psynth/base/arg_parser.hpp>

#include "cli/psychosynth_cli.hpp"

using namespace std;
using namespace psynth;

#define TIME_OUT 20

void psychosynth_cli::print_help()
{
    cout <<
	"GNU psynth is a standalone command line Psychosynth server and client.\n"
	"\n"
	"Usage: psynth [options]\n"
	"\n"
	"Options:\n";
    print_base_options (cout);
    cout <<
	"\n"
	"OSC options:\n"
	"  -S, --server          Run a server.\n"
	"  -C, --client <host>   Connect to the specified port.\n"
	"  -p, --port            Use this server port instead of the default.\n"
	"  -P, --client-port     Use this client port instead of the default.\n";
}

void psychosynth_cli::print_version()
{
  cout << "GNU psynth " << VERSION << endl <<
    "Copyright (C) 2007-2009 Juan Pedro Bolivar Puente\n"
    "This is free software; see the source for copying conditions.  There is NO\n"
    "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n"

    "\nWritten by Juan Pedro Bolivar Puente";
    
}

void psychosynth_cli::prepare (psynth::arg_parser& ap)
{
    init ();
    
    ap.add ('S', "server", &m_run_server);
    ap.add ('C', "client", &m_host);
    ap.add ('p', "port", &m_server_port);
    ap.add ('P', "client-port", &m_client_port);
}

int psychosynth_cli::execute ()
{
    int ret_val = -1;
    
    if (m_host.empty() && !m_run_server)
	cout << "Not enough parameters. Use -h or --help." << endl;
    else {
	setup_synth ();
	if (m_run_server)
	    ret_val = run_server ();
	else if (!m_host.empty())
	    ret_val = run_client ();
	close_synth ();
    }
    
    return ret_val;
}

void psychosynth_cli::init ()
{
    m_run_server = false;
    m_client_port = PSYNTH_DEFAULT_CLIENT_PORT_STR;
    m_server_port = PSYNTH_DEFAULT_SERVER_PORT_STR;
    m_host.clear();
}

int psychosynth_cli::run_client ()
{
    timer timer;
    osc_client client;
    osc_client_logger logger;
    lo_address add;
    int ret_val = 0;
    
    add = lo_address_new (m_host.c_str(), m_server_port.c_str());

    client.set_world (get_world());
    client.activate ();
    
    client.add_listener (&logger);
    client.connect (add, m_client_port.c_str());
    
    timer.update();
    while (ret_val == 0) {
	timer.update();
	
	while (client.receive (TIME_OUT));
	client.update (timer.delta_ticks());

	get_world ()->update ();
	
	if (client.get_state () == osc_client::IDLE)
	    ret_val = -1;
    }

    return ret_val;
}

int psychosynth_cli::run_server ()
{
    timer timer;
    osc_server server;
    osc_server_logger logger;
    int ret_val = 0;
    
    server.add_listener (&logger);
    server.listen (m_server_port.c_str());

    server.set_world (get_world ());
    server.activate();
    
    timer.update();
    while (ret_val == 0) {
	timer.update ();

	while (server.receive (TIME_OUT));
	server.update (timer.delta_ticks ());

	get_world ()->update ();
	
	if (server.get_state () == osc_server::IDLE)
	    ret_val = -1;
    }

    return ret_val;
}
