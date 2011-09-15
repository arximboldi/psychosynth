/**
 *  Time-stamp:  <2011-09-15 21:49:28 raskolnikov>
 *
 *  @file        osc_passive_client.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        Wed Sep 14 20:35:54 2011
 *
 *  @brief A client that does not require a server.
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

#ifndef PSYNTH_NET_OSC_PASSIVE_CLIENT_HPP_
#define PSYNTH_NET_OSC_PASSIVE_CLIENT_HPP_

#include <psynth/net/exception.hpp>
#include <psynth/net/osc_controller.hpp>

#define PSYNTH_DEFAULT_PASSIVE_PORT      8191
#define PSYNTH_DEFAULT_PASSIVE_PORT_STR "8191"

namespace psynth
{
namespace net
{

PSYNTH_DECLARE_ERROR (error, osc_passive_error);

class osc_passive : public osc_controller
{
public:
    osc_passive (const char* port);
    ~osc_passive ();
    int receive (int time_out = 0);
    
private:
    lo_server _server;
};

} /* namespace net */
} /* namespace psynth */

#endif /* PSYNTH_NET_OSC_PASSIVE_CLIENT_HPP_ */

