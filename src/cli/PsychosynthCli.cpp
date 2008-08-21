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

#include <libpsynth/net/osc_client.h>
#include <libpsynth/net/osc_server.h>
#include <libpsynth/net/osc_client_logger.h>
#include <libpsynth/net/osc_server_logger.h>
#include <libpsynth/common/timer.h>
#include <libpsynth/common/arg_parser.h>

#include "cli/PsychosynthCli.h"

using namespace std;
using namespace psynth;

#define TIME_OUT 20

void PsychosynthCli::print_help()
{
    cout <<
	"PsychosynthCli (c) 2007-2008 Juan Pedro Bolívar Puente\n"
	"\n"
	"Usage:\n"
	"  psynth [options]\n"
	"\n"
	"Base options:\n";
    print_base_options (cout);
    cout <<
	"\n"
	"OSC options:\n"
	"  -S, --server          Run a server.\n"
	"  -C, --client <host>   Connect to the specified port.\n"
	"  -p, --port            Use this server port instead of the default.\n"
	"  -P, --client-port     Use this client port instead of the default.\n";
}

void PsychosynthCli::print_version()
{
    cout << "PsychosynthCli " << VERSION << endl;
}

void PsychosynthCli::prepare (psynth::arg_parser& ap)
{
    init ();
    
    ap.add ('S', "server", &m_run_server);
    ap.add ('C', "client", &m_host);
    ap.add ('p', "port", &m_server_port);
    ap.add ('P', "client-port", &m_client_port);
}

int PsychosynthCli::execute ()
{
    int ret_val = -1;
    
    if (m_host.empty() && !m_run_server)
	cout << "Not enough parameters. Use -h or --help." << endl;
    else {
	setup_synth ();
	if (m_run_server)
	    ret_val = runServer();
	else if (!m_host.empty())
	    ret_val = runClient();
	close_synth ();
    }
    
    return ret_val;
}

void PsychosynthCli::init ()
{
    m_run_server = false;
    m_client_port = PSYNTH_DEFAULT_CLIENT_PORT_STR;
    m_server_port = PSYNTH_DEFAULT_SERVER_PORT_STR;
    m_host.clear();
}

int PsychosynthCli::runClient()
{
    timer timer;
    osc_client client;
    osc_client_logger logger;
    lo_address add;
    int ret_val = 0;
    
    add = lo_address_new (m_host.c_str(), m_server_port.c_str());

    client.set_table (get_table());
    client.activate ();
    
    client.add_listener (&logger);
    client.connect (add, m_client_port.c_str());
    
    timer.update();
    while (ret_val == 0) {
	timer.update();
	
	while (client.receive (TIME_OUT));
	client.update (timer.delta_ticks());

	get_table ()->update ();
	
	if (client.get_state () == osc_client::IDLE)
	    ret_val = -1;
    }

    return ret_val;
}

int PsychosynthCli::runServer()
{
    timer timer;
    osc_server server;
    osc_server_logger logger;
    int ret_val = 0;
    
    server.add_listener (&logger);
    server.listen (m_server_port.c_str());

    server.set_table (get_table ());
    server.activate();
    
    timer.update();
    while (ret_val == 0) {
	timer.update ();

	while (server.receive (TIME_OUT));
	server.update (timer.delta_ticks ());

	get_table ()->update ();
	
	if (server.get_state () == osc_server::IDLE)
	    ret_val = -1;
    }

    return ret_val;
}
