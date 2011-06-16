/**
 *  Time-stamp:  <2011-06-16 22:26:54 raskolnikov>
 *
 *  @file        oscillator.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  @brief Static oscillator implementation.
 *  @todo  Make more generic?
 */

#ifndef PSYNTH_SYNTH_OSCILLATOR_H
#define PSYNTH_SYNTH_OSCILLATOR_H

#include <cmath>
#include <psynth/base/misc.hpp>
#include <psynth/sound/typedefs.hpp>
#include <psynth/synth/wave_table.hpp>

namespace psynth
{
namespace synth
{

/* TODO: Find a way to avoid this. */
class envelope_simple;

struct sine_generator
{
    PSYNTH_FORCEINLINE sound::bits32sf
    operator () (float x)
    { return std::sin (2 * M_PI * x); }
};

struct square_generator
{
    PSYNTH_FORCEINLINE sound::bits32sf
    operator () (float x)
    { return base::phase (x) > 0.5f ? -1.0 : 1.0; }
};

struct triangle_generator
{
    PSYNTH_FORCEINLINE sound::bits32sf
    operator () (float x)
    {
        float p = base::phase (x);
	if (p <= 0.25f)
	    return p * 4.0f;
	else if (p <= 0.75f)
	    return 2.0f - p * 4.0f;
	return p * 4.0f - 4.0f;
    }
};

struct sawtooth_generator
{
    PSYNTH_FORCEINLINE sound::bits32sf
    operator () (float x)
    {
        return -1.0f + base::phase(x) * 2.0f;
    }
};

struct moogsaw_generator
{
    PSYNTH_FORCEINLINE sound::bits32sf
    operator () (float x)
    {
        float p = base::phase (x);
	if(p < 0.5f )
	    return -1.0f + p * 4.0f;
	return 1.0f - 2.0f * p;
    }
};

struct exp_generator
{
    PSYNTH_FORCEINLINE sound::bits32sf
    operator () (float x)
    {
        float p = base::phase (x);
	if(p > 0.5f)
	    p = 1.0f - p;
	return -1.0f + 8.0f * p * p;
    }
};


template <class Generator = sine_generator>
class oscillator
{
public:
    /** @todo Parametrize? */
    static const std::size_t default_table_size = 1 << 12;
    
    oscillator (std::size_t frame_rate,
                float       freq       = 220.0f,
		float       ampl       = 1.0f,
		float       phase      = 0.0f,
		bool        wave_table = true)
        : _frame_rate (frame_rate)
        , _speed (freq / frame_rate)
        , _x (phase)
        , _freq (freq)
        , _ampl (ampl)
        , _phase (phase)
        , _wave_table (wave_table)
    {
	if (wave_table && !s_table_init)
	    initialize_table ();
    }

    void restart ()
    { _x = 0.0f; }

    void set_frame_rate (std::size_t frame_rate)
    { _frame_rate = frame_rate; _speed = _freq / frame_rate; }

    void set_frequency (float freq)
    { _freq = freq; _speed = freq / _frame_rate; }

    void set_amplitude (float ampl)
    { _ampl = ampl; }

    void set_phase (float phase)
    { _x += phase - _phase; _phase = phase; }
    
    void set_wave_table (bool wave_table)
    {
	if (wave_table && !s_table_init)
	    initialize_table ();
	_wave_table = wave_table;
    }
    
    template <class Range1>
    void update (const Range1& out_buf);

    template <class Range1, class Range2>
    void update_fm (const Range1& out_buf, const Range2& mod_buf);

    template <class Range1, class Range2>
    void update_pm (const Range1& out_buf, const Range2& mod_buf);

    template <class Range1, class Range2>
    void update_am (const Range1& out_buf, const Range2& mod_buf);
    
private:
    static wave_table<sound::mono32sf_buffer> s_wave_table;
    static bool                               s_table_init;
    
    Generator   _gen;
    std::size_t _frame_rate;
    float       _speed;
    float       _x;
    float       _freq;
    float       _ampl;
    float       _phase;
    bool        _wave_table;
    
    void initialize_table ();
};

} /* namespace synth */
} /* namespace psynth */

#include <psynth/synth/oscillator.tpp>

#endif /* PSYNTH_OSCILLATOR_H */
