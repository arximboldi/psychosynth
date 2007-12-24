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

#ifndef WAVETABLE_H
#define WAVETABLE_H

#include <common/AudioBuffer.h>
#include <common/Singleton.h>

/*
 * TODO: This class can be highly improved.
 */
class WaveTable : public Singleton<WaveTable>
{
public:
    enum WaveTypes {
	SINE,
	SQUARE,
	TRIANGLE,
	SAWTOOTH,
	N_TYPES
    };

    static const size_t TABLE_SIZE = 1<<12;
    
private:
    Sample m_table[N_TYPES][TABLE_SIZE];

    void initializeSine(Sample* tab, int size);
    void initializeTriangle(Sample* tab, int size);
    void initializeSquare(Sample* tab, int size);
    void initializeSawtooth(Sample* tab, int size);
public:

    WaveTable();
    ~WaveTable();

    const Sample* operator[](size_t id) {
	return m_table[id];
    }
};

#endif /* WAVETABLE_H */
