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

#ifndef ELEMENTTYPES_H
#define ELEMENTTYPES_H

enum ElementType
{
    ELEM_OSC_SINE,
    ELEM_OSC_SQUARE,
    ELEM_OSC_SAWTOOTH,
    ELEM_OSC_TRIANGLE,

    ELEM_LFO_SINE,
    ELEM_LFO_SQUARE,
    ELEM_LFO_SAWTOOTH,
    ELEM_LFO_TRIANGLE,

    ELEM_FILTER_LOWPASS,
    ELEM_FILTER_HIGHPASS,
    ELEM_FILTER_BANDPASS_CSG,
    ELEM_FILTER_BANDPASS_CZPG,
    ELEM_FILTER_NOTCH,
    ELEM_FILTER_MOOG,

    ELEM_MIXER,
    ELEM_RINGMOD
};

#endif /* ELEMENTTYPES_H */
