/**
 *  Time-stamp:  <2011-03-19 14:00:48 raskolnikov>
 *
 *  @file        envelope.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  @brief Envelope interface.
 */

/*
 *  Copyright (C) 2008, 2011 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *   
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef PSYNTH_SYNTH_ENVELOPE_H
#define PSYNTH_SYNTH_ENVELOPE_H

namespace psynth
{
namespace synth
{

/**
 * Basic envelope interface.
 */
template <class Range>
class envelope
{
public:
    typedef Range range;
    typedef typename Range::value_type value_type;
    
    /**
     * Updates the envelope for one sample.
     * @return The value of the envelope.
     */
    virtual value_type update () = 0;

    /**
     * Updates the envelope for several samples.
     * @param samples the number of samples to advance.
     * @return The value of the envelope after.
     */
    virtual value_type update (std::size_t samples) = 0;

    /**
     * Fills a buffer with samples from the envelope.
     * @param samples The buffer to fill.
     * @param n_samples The number of samples to fill.
     */
    virtual void update (const range& samples) = 0;

    /**
     * Start the envelope effect.
     */
    virtual void press () = 0;

    /**
     * Start finishing the envelope.
     */
    virtual void release () = 0;

    /**
     * Returns wether the envelope has finished.
     * @return @c true if the envelope has finished and @c false otherwise.
     */
    virtual bool finished () = 0;
};

} /* namespace synth */
} /* namespace psynth */

#endif /* PSYNTH_ENVELOPE_H */
