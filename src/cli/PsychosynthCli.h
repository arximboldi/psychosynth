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

#ifndef PSYCHOSYNTHCLI_H
#define PSYCHOSYNTHCLI_H

#include <libpsynth/psynth/psynth_app.h>
#include <libpsynth/version.h>

class PsychosynthCli : public psynth::psynth_app
{
    bool m_run_server;
    std::string m_client_port;
    std::string m_server_port;
    std::string m_host;
    
    void print_help ();
    void print_version ();
    void prepare(psynth::arg_parser& arg_parser);
    void init();
    
    int execute();
    int runServer();
    int runClient();
public:
    PsychosynthCli() {};
};

#endif /* PSYCHOSYNTHCLI_H */
