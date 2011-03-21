/**
 *  Time-stamp:  <2011-03-21 17:34:41 raskolnikov>
 *
 *  @file        multi_point_envelope.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  @brief A envelope with any number of points.
 *
 *  @todo Thinking about how many classes hardcode time as a float,
 *  maybe we should consider making that generic too.
 *
 *  @todo I have the feeling that many of these classes should substitute
 *  update by operator () and just use xxx_frames algorithms. 
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
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

#ifndef PSYNTH_SYNTH_MULTI_POINT_ENVELOPE_H
#define PSYNTH_SYNTH_MULTI_POINT_ENVELOPE_H

#include <vector>
#include <psynth/sound/forwards.hpp>
#include <psynth/sound/algorithm.hpp>
#include <psynth/synth/envelope.hpp>

namespace psynth
{
namespace synth
{

template <class Sample>
struct envelope_point
{
    float  dt;
    Sample val;
};

/**
 * @todo Maybe it is coller to have envelope_values to store certain Frames
 * and not Samples as points for multi-channel envelopes. However for
 * convenience this should be better done when there is better
 * arithmetic support for Frames.
 */
template <class Sample>
class envelope_values
{
    template <class R, class Vp>
    friend class multi_point_envelope;

public:
    typedef envelope_point<Sample> point;
    
    envelope_values ()
        : _sustain (1)
        , _factor (1.0f)
	{}
    
    void set_asr (point a, point s, point r);
    void set_adsr (point a, point d, point s, point r);

    void set_factor (float factor)
    { _factor = factor; }

    float factor () const
    { return _factor; }
    
    void set_sustain (std::size_t sustain)
    { _sustain = sustain; }
    
    void set (const std::vector<point>& p, std::size_t sustain)
    {
	_points  = p;
	_sustain = sustain;
    }
    
    point& get (std::size_t index)
    { return _points [index]; }

    const point& get (std::size_t index) const
    { return _points [index]; }

    point& operator [] (int index)
    { return _points [index]; }

    const point& operator[] (int index) const
    { return _points [index]; }

    std::size_t size () const
    { return _points.size (); }

    void resize (std::size_t size)
    { _points.resize (size); }

private:
    std::vector<point> _points;
    std::size_t        _sustain;
    float              _factor;
};

template <class Range,
          typename ValuesPtr =
          envelope_values<typename sound::sample_type<Range>::type>* >
class multi_point_envelope : public envelope<Range>
{
public:
    typedef Range                      range;
    typedef typename Range::value_type value_type;
    typedef ValuesPtr                  values_ptr;
    
    multi_point_envelope (values_ptr val)
	: _val (val)
	, _cur_point (0)
	, _time (0.0f)
	, _pressed (false)
	{}

    void set_values (values_ptr vals)
    { _val = vals; }

    values_ptr get_values ()
    { return _val; }

    float time () const
    { return _time; }

    void set_time (float time)
    { _time = time; }

    void restart ()
    {
	_time = 0;
	_cur_point = 0;
    }
    
    void press ()
    {
	if (multi_point_envelope::finished ())
	    multi_point_envelope::restart ();
	_pressed = true;
    }

    void release ()
    { _pressed = false; }

    PSYNTH_FORCEINLINE value_type update ()
    { return multi_point_envelope::update (1); }
    
    value_type update (std::size_t sample);

    void update (const range& samples)
    {
        generate_frames (samples, [&] () -> value_type {
                return this->multi_point_envelope::update ();
            });
    }

    bool finished ()
    { return _cur_point >= _val->size() - 1; }

private:
    values_ptr    _val;
    std::size_t   _cur_point;
    float         _time;
    bool          _pressed;
};

} /* namespace synth */
} /* namespace psynth */

#include <psynth/synth/multi_point_envelope.tpp>

#endif /* PSYNTH_SYNTH_MULTI_POINT_ENVELOPE_H */
