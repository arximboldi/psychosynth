/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 Juan Pedro Bolivar Puente                          *
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

#ifndef PSYNTH_NODE_TYPES_H
#define PSYNTH_NODE_TYPES_H

namespace psynth
{

enum node_types
{
    NODE_OUTPUT = 0,
    NODE_MIXER,
    NODE_CONTROLMIXER,
    NODE_OSCILLATOR,
    NODE_LFO,
    NODE_FILTER,
    NODE_SAMPLER,
    NODE_STEPSEQ,
    NODE_AUDIONOISE,
    NODE_CONTROLNOISE,
    NODE_ECHO,
    NODE_DELAY,
    NODE_DOUBLE_SAMPLER,
    N_NODES
};

} /* namespace psynth */

#endif /* PSYNTH_NODE_TYPES_H */
