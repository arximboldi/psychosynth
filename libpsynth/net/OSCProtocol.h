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
#define MSG_ACCEPT      "/ps/accept"
#define MSG_CONNECT     "/ps/connect"
#define MSG_ALIVE       "/ps/alive"
#define MSG_DISCONNECT  "/ps/disconnect"
#define MSG_DROP        "/ps/drop"
#define MSG_GET_STATE   "/ps/get_state"

/* Table constrolling. */
#define MSG_PARAM        "/ps/param"
#define MSG_ACTIVATE     "/ps/activate"
#define MSG_DEACTIVATE   "/ps/deactivate"
#define MSG_ADD          "/ps/add"
#define MSG_DELETE       "/ps/delete"

#endif /* PSYNTH_OSCPROTOCOL_H */
