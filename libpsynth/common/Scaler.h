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

/**
 * Basic scaler interface.
 * 
 * A scaler is an object that processes audio data
 * in a way that may cause some stretching or enlargement of the signal, and
 * and usually refers to a sample rate converter but also to a pitch or tempo
 * changer.
 *
 * Note that all params are defined as a factor where 1.0f means no change. For
 * example 2.0f means twice as fast, twice higher tone, and such.
 */
class Scaler
{
public:
    /**
     * Change the tempo factor.
     * @param tempo New tempo factor.
     */
    virtual void setTempo(float tempo) = 0;

    /**
     * Change the rate factor.
     * @param rate New rate factor.
     */
    virtual void setRate(float rate) = 0;

    /**
     * Change the pitch factor.
     * @param pitch New pitch factor.
     */
    virtual void setPitch(float pitch) = 0;

    /**
     * Set the number of channels.
     */
    virtual void setChannels(int chan) = 0;

    /**
     * Returns the tempo factor.
     */
    virtual float getTempo() = 0;

    /**
     * Returns the rate factor.
     */
    virtual float getRate() = 0;

    /**
     * Returns the pitch factor.
     */
    virtual float getPitch() = 0;

    /**
     * Returns the number of channels.
     */
    virtual int getChannels() = 0;

    /**
     * Returns the number of availible frames.
     */
    virtual int availible() = 0;

    /**
     * Updates the scaler with some data. The data must be interleaved.
     * @param data The data to inject to the scaler.
     * @param frames The number of frames to inject.
     */
    virtual void update(float* data, int frames) = 0;

    /**
     * Pops some data form the scaler. The data will be loaded interleaved.
     * @param data A buffer where to store the data.
     * @param frames The maximun number of frames to pop.
     * @return The actual number of frames received.
     */
    virtual int receive(float* data, int frames) = 0;

    /**
     * Clears all the data left in the scaler.
     */
    virtual void clear() = 0;
};

} /* namespace psynth */

#endif /* PSYNTH_SCALER_H */
