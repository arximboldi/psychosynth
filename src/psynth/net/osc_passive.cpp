/**
 *  Time-stamp:  <2011-09-15 22:47:17 raskolnikov>
 *
 *  @file        osc_passive_client.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Sep 14 20:48:44 2011
 *
 *  @brief Passice osc client implementation.
 */

/*
 *  Copyright (C) 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "base/throw.hpp"
#include "osc_passive.hpp"

#define PSYNTH_MODULE_NAME "psynth.net.osc"

namespace psynth
{
namespace net
{

PSYNTH_DEFINE_ERROR(osc_passive_error);

osc_passive::osc_passive (const char* port)
    : osc_controller (false, false)
{
    _server = lo_server_new_with_proto (port, LO_UDP, NULL);
    if (!_server)
        PSYNTH_THROW(osc_passive_error)
            << "Could not create server on port: " << port;

    add_methods (_server);
}

osc_passive::~osc_passive()
{
    lo_server_free (_server);
}

int osc_passive::receive(int time_out)
{
    int n_recv = 0;

    if (time_out >= 0)
        n_recv = lo_server_recv_noblock(_server, time_out);
    else
        n_recv = lo_server_recv(_server);

    return n_recv;
}

} /* namespace net */
} /* namespace psynth */
