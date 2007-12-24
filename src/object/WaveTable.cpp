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

#include <cmath>
#include "object/WaveTable.h"

#define NINT(f) ((f)-floor(f) < ceil(f)-(f) ? floor(f) : ceil(f))

using namespace std;

void WaveTable::initializeSine(Sample* tab, int size)
{
    for (int i = 0; i < size; i++)
	*tab++ = sin(i * 2*M_PI/size);
}

void WaveTable::initializeTriangle(Sample* tab, int size)
{
    int i;
    for (i = 0; i < size/4; i++)
	*tab++ = (Sample)i / (size/4);
    for (i = size/4; i > -size/4; i--)
	*tab++ = (Sample)i / (size/4);
    for (i = -size/4; i < 0; i++)
	*tab++ = (Sample)i / (size/4);
}

void WaveTable::initializeSquare(Sample* tab, int size)
{
    for (int i = 0; i < size/2; i++)
	*tab++ = 0;
    for (int i = 0; i < size/2; i++)
	*tab++ = 1;
}

void WaveTable::initializeSawtooth(Sample* tab, int size)
{
    for (int i = -size/2; i < size/2; i++)
	*tab++ = (Sample)i / (size/2);
}

WaveTable::WaveTable()
{
    initializeSine(m_table[SINE], TABLE_SIZE);
    initializeSquare(m_table[SQUARE], TABLE_SIZE);
    initializeTriangle(m_table[TRIANGLE], TABLE_SIZE);
    initializeSawtooth(m_table[SAWTOOTH], TABLE_SIZE);
}

WaveTable::~WaveTable()
{
}


