/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#ifndef PSYNTH_SCALER_H
#define PSYNTH_SCALER_H

namespace psynth
{

class Scaler
{
public:
    virtual void setTempo(float tempo) = 0;
    virtual void setRate(float rate) = 0;
    virtual void setPitch(float rate) = 0;
    virtual void setChannels(int chan) = 0;
    
    virtual float getTempo() = 0;
    virtual float getRate() = 0;
    virtual float getPitch() = 0;
    virtual int getChannels() = 0;

    virtual int availible() = 0;
    virtual void update(float* data, int samples) = 0;
    virtual int receive(float* data, int samples) = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_SCALER_H */
