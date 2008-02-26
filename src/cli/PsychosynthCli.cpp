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

#include <config.h>

#include <libpsynth/net/OSCClient.h>
#include <libpsynth/net/OSCServer.h>
#include <libpsynth/net/OSCClientLogger.h>
#include <libpsynth/net/OSCServerLogger.h>
#include <libpsynth/common/Timer.h>

#include "cli/PsychosynthCli.h"

using namespace std;
using namespace psynth;

#define TIME_OUT 1000

void PsychosynthCli::printHelp()
{
    cout <<
	"PsychosynthCli (c) 2007-2008 Juan Pedro Bolívar Puente\n"
	"\n"
	"Usage:\n"
	"  psynth [options]\n"
	"\n"
	"Base options:\n";
    printBaseOptions(cout);
    cout <<
	"\n"
	"OSC options:\n"
	"  -S, --server          Run a server.\n"
	"  -C, --client <host>   Connect to the specified port.\n"
	"  -p, --port            Use this server port instead of the default.\n"
	"  -P, --client-port     Use this client port instead of the default.\n";
}

void PsychosynthCli::printVersion()
{
    cout << "PsychosynthCli " << VERSION << endl;
}

void PsychosynthCli::prepare(psynth::ArgParser& arg_parser)
{
    init();
    
    arg_parser.add('S', "server", &m_run_server);
    arg_parser.add('C', "client", &m_host);
    arg_parser.add('p', "port", &m_server_port);
    arg_parser.add('P', "client-port", &m_client_port);
}

int PsychosynthCli::execute()
{
    int ret_val = -1;
    
    if (m_host.empty() && !m_run_server)
	cout << "Not enough parameters. Use -h or --help." << endl;
    else {
	setupSynth();
	if (m_run_server)
	    ret_val = runServer();
	else if (!m_host.empty())
	    ret_val = runClient();
	closeSynth();
    }
    
    return ret_val;
}

void PsychosynthCli::init()
{
    m_run_server = false;
    m_client_port = PSYNTH_DEFAULT_CLIENT_PORT_STR;
    m_server_port = PSYNTH_DEFAULT_SERVER_PORT_STR;
    m_host.clear();
}

int PsychosynthCli::runClient()
{
    Timer timer;
    OSCClient client;
    OSCClientLogger logger;
    lo_address add;
    int ret_val = 0;
    
    add = lo_address_new(m_host.c_str(), m_server_port.c_str());

    client.setTable(getTable());
    client.activate();
    
    client.addListener(&logger);
    client.connect(add, m_client_port.c_str());
    
    timer.update();
    while(ret_val == 0) {
	timer.update();
	client.update(timer.deltaticks(), TIME_OUT);

	if (client.getState() == OSCClient::IDLE)
	    ret_val = -1;
    }

    return ret_val;
}

int PsychosynthCli::runServer()
{
    Timer timer;
    OSCServer server;
    OSCServerLogger logger;
    int ret_val = 0;
    
    server.addListener(&logger);
    server.listen(m_server_port.c_str());

    server.setTable(getTable());
    server.activate();
    
    timer.update();
    while(ret_val == 0) {
	timer.update();
	server.update(timer.deltaticks(), TIME_OUT);

	if (server.getState() == OSCServer::IDLE)
	    ret_val = -1;
    }

    return ret_val;
}
