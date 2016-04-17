/**
 *  Time-stamp:  <2010-10-18 02:43:07 raskolnikov>
 *
 *  @file        osc_client_logger.hpp
 *  @author      Juan Pedro Bolivar Puente <raskolnikov@es.gnu.org>
 *  @date        Mon Oct 18 02:41:37 2010
 *
 *  OSC client logger.
 */

/*
 *  Copyright (C) 2007, 2010 Juan Pedro Bolivar Puente
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

#ifndef PSYNTH_OSCCLIENTLOGGER_H
#define PSYNTH_OSCCLIENTLOGGER_H

#include <psynth/base/logger.hpp>
#include <psynth/net/osc_client.hpp>

namespace psynth
{

class osc_client_logger : public osc_client_listener
{
public:
    virtual bool handle_client_connect (osc_client* client) {
	base::logger::self () ("oscclient",
			       base::log::info, "Connecting...");
	return false;
    }

    virtual bool handle_client_disconnect(osc_client* client, osc_client_error err) {
	switch(err) {
	case CE_NONE:
	    base::logger::self () ("oscclient", base::log::info, "Disconnected.");
	    break;

	case CE_PORT_BINDING:
	    base::logger::self () ("oscclient", base::log::error, "Could not bind port.");
	    break;

	case CE_SERVER_DROP:
	    base::logger::self () ("oscclient", base::log::error, "Connection dropped by server.");
	    break;

	case CE_SERVER_TIMEOUT:
	    base::logger::self () ("oscclient", base::log::error, "Connection timeout");
	    break;

	default:
	    break;
	}
	return false;
    }

    virtual bool handle_client_accept (osc_client* client) {
	base::logger::self () ("oscclient", base::log::info, "Accepted.");
	return false;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OSCCLIENTLOGGER_H */
