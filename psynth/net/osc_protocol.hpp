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

#ifndef PSYNTH_OSCPROTOCOL_H
#define PSYNTH_OSCPROTOCOL_H

/* Server-Client connection abstraction. */
#define PSYNTH_OSC_MSG_ACCEPT      "/ps/accept"
#define PSYNTH_OSC_MSG_CONNECT     "/ps/connect"
#define PSYNTH_OSC_MSG_ALIVE       "/ps/alive"
#define PSYNTH_OSC_MSG_DISCONNECT  "/ps/disconnect"
#define PSYNTH_OSC_MSG_DROP        "/ps/drop"
#define PSYNTH_OSC_MSG_GET_STATE   "/ps/get_state"

/* World constrolling. */
#define PSYNTH_OSC_MSG_PARAM       "/ps/param"
#define PSYNTH_OSC_MSG_ACTIVATE    "/ps/activate"
#define PSYNTH_OSC_MSG_DEACTIVATE  "/ps/deactivate"
#define PSYNTH_OSC_MSG_ADD         "/ps/add"
#define PSYNTH_OSC_MSG_DELETE      "/ps/delete"

#endif /* PSYNTH_OSCPROTOCOL_H */
