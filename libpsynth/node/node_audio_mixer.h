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

#ifndef PSYNTH_OBJECTAUDIOMIXER_H
#define PSYNTH_OBJECTAUDIOMIXER_H

#include <libpsynth/node/node_types.h>
#include <libpsynth/node/node_mixer.h>
#include <libpsynth/node/node_factory.h>

namespace psynth
{

class node_audio_mixer : public node_mixer
{
public:
    enum out_audio_socket_id {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

    enum out_control_socket_id {
	N_OUT_C_SOCKETS
    };
    
private:
    void do_update (const node* caller, int caller_port_type, int caller_port);
    void do_advance () {}
    void on_info_change () {}
    
public:  
    node_audio_mixer (const audio_info& prop, int n_chan = 2) :
	node_mixer (prop,
		    NODE_MIXER,
		    "audio_mixer",
		    N_OUT_A_SOCKETS,
		    N_OUT_C_SOCKETS,
		    n_chan)
	{}
};

PSYNTH_DECLARE_NODE_FACTORY (node_audio_mixer, "audio_mixer");

} /* namespace psynth */

#endif /* PSYNTH_OBJECTAUDIOMIXER_H */
