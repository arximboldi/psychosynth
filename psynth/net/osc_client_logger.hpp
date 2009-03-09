/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007                          *
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
	logger::self () ("oscclient", log::INFO, "Connecting...");
	return false;
    }
    
    virtual bool handle_client_disconnect(osc_client* client, osc_client_error err) {
	switch(err) {
	case CE_NONE:
	    logger::self () ("oscclient", log::INFO, "Disconnected.");
	    break;

	case CE_PORT_BINDING:
	    logger::self () ("oscclient", log::ERROR, "Could not bind port.");
	    break;

	case CE_SERVER_DROP:
	    logger::self () ("oscclient", log::ERROR, "Connection dropped by server.");
	    break;

	case CE_SERVER_TIMEOUT:
	    logger::self () ("oscclient", log::ERROR, "Connection timeout");
	    break;

	default:
	    break;
	}
	return false;
    }
    
    virtual bool handle_client_accept (osc_client* client) {
	logger::self () ("oscclient", log::INFO, "Accepted.");
	return false;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OSCCLIENTLOGGER_H */
