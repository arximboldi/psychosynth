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

#ifndef PSYCHOSYNTH_CLI_H
#define PSYCHOSYNTH_CLI_H

#include <psynth/app/psynth_app.hpp>
#include <psynth/version.hpp>

class psychosynth_cli : public psynth::psynth_app
{
public:
    psychosynth_cli () {};

private:
    bool m_run_server;
    std::string m_client_port;
    std::string m_server_port;
    std::string m_host;
    std::string m_osc_port;
    
    void print_help ();
    void print_version ();
    void prepare (psynth::base::arg_parser& arg_parser);
    void init ();

    int execute ();
    int run_server ();
    int run_client ();
    int run_osc ();
};

#endif /* PSYCHOSYNTH_CLI_H */
