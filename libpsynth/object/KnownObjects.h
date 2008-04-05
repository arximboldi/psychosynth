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

#ifndef PSYNTH_KNOWNOBJECTS_H
#define PSYNTH_KNOWNOBJECTS_H

namespace psynth
{

enum KnownObjectType
{
    OBJ_OUTPUT = 0,
    OBJ_MIXER,
    OBJ_CONTROLMIXER,
    OBJ_OSCILLATOR,
    OBJ_LFO,
    OBJ_FILTER,
    OBJ_SAMPLER,
    OBJ_STEPSEQ,
    OBJ_AUDIONOISE,
    OBJ_CONTROLNOISE,
    OBJ_ECHO,
    OBJ_DELAY,
    N_OBJECTS
};

} /* namespace psynth */

#endif /* PSYNTH_KNOWNOBJECTS_H */
