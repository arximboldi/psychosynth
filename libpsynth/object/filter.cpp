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

#include "object/filter.h"

namespace psynth
{

/*
 * Most of this code has been taken from LMMS.
 */

#define tMax(a,b) ((a) > (b) ? (a) : (b))
#define tLimit(x,a,b) ((x) < (a) ? (a) : ((x) > (b)? (b) : (x)))

void filter_values::calculate (float freq)
{
    freq = tMax(freq, 0.01f);
    m_freq = freq;

    calculate();
}

void filter_values::calculate (type type, float freq, float res, float srate)
{    
    /* Adapt the values to avoid strange noise */
    freq = tMax(freq, 0.01f);
    res = tMax(res, 0.01f);

    m_type = type;
    m_freq = freq;
    m_res = res;
    m_srate = srate;

    calculate();
}

void filter_values::calculate ()
{   
    if(m_type == MOOG) {
	// [0, 0.5]
	const float f = m_freq / m_srate;

        // Empirical tunning
	m_p = ( 3.6f - 3.2f * f ) * f;
	m_k = 2.0f * m_p - 1;
	m_r = m_res * powf( M_E, ( 1 - m_p ) * 1.386249f );
	return;
    }

    // other filters
    const float omega = 2 * M_PI * m_freq / m_srate;
    const float tsin = sinf( omega );
    const float tcos = cosf( omega );

    /*
      float alpha;
      if (q_is_bandwidth)
      alpha = tsin*sinhf(logf(2.0f)/2.0f*q*omega/
      tsin);
      else
    */
    const float alpha = 0.5f * tsin / m_res;

    const float a0 = 1.0f / ( 1.0f + alpha );

    m_a1a0 = -2.0f * tcos * a0;
    m_a2a0 = ( 1.0f - alpha ) * a0;

    switch(m_type)
    {
    case LOWPASS:
	m_b1a0 = ( 1.0f - tcos ) * a0;
	m_b0a0 = m_b1a0 * 0.5f;
	m_b2a0 = m_b0a0; //((1.0f-tcos)/2.0f)*a0;
	break;
    case HIPASS:
	m_b1a0 = ( -1.0f - tcos ) * a0;
	m_b0a0 = m_b1a0 * -0.5f;
	m_b2a0 = m_b0a0; //((1.0f+tcos)/2.0f)*a0;
	break;
    case BANDPASS_CSG:
	m_b1a0 = 0.0f;
	m_b0a0 = tsin * 0.5f * a0;
	m_b2a0 = -m_b0a0;
	break;
    case BANDPASS_CZPG:
	m_b1a0 = 0.0f;
	m_b0a0 = alpha * a0;
	m_b2a0 = -m_b0a0;
	break;
    case NOTCH:
	m_b1a0 = m_a1a0;
	m_b0a0 = a0;
	m_b2a0 = a0;
	break;
    default:
	break;
    }
}

#define m_b0a0 m_coef->m_b0a0
#define m_b1a0 m_coef->m_b1a0
#define m_b2a0 m_coef->m_b2a0
#define m_a1a0 m_coef->m_a1a0
#define m_a2a0 m_coef->m_a2a0
#define m_r m_coef->m_r
#define m_p m_coef->m_p
#define m_k m_coef->m_k

sample filter::update (sample _in0)
{
    sample out;

    switch(m_coef->m_type)
    {
    case filter_values::MOOG: {
	sample x = _in0 - m_r*m_y4;

	// four cascaded onepole filters
	// (bilinear transform)
	m_y1 = tLimit(( x + m_oldx ) * m_p - m_k * m_y1,
		      -10.0f, 10.0f );
	m_y2 = tLimit(( m_y1 + m_oldy1 ) * m_p - m_k * m_y2,
		      -10.0f, 10.0f );
	m_y3 = tLimit(( m_y2 + m_oldy2 ) * m_p - m_k * m_y3,
		      -10.0f, 10.0f );
	m_y4 = tLimit(( m_y3 + m_oldy3 ) * m_p - m_k * m_y4,
		      -10.0f, 10.0f );

	m_oldx = x;
	m_oldy1 = m_y1;
	m_oldy2 = m_y2;
	m_oldy3 = m_y3;

	out = m_y4 - m_y4 * m_y4 *
	    m_y4 * ( 1.0f / 6.0f );

	break;
    }
	
    default:
	// filter
	out = m_b0a0 * _in0 +
	    m_b1a0 * m_in1 +
	    m_b2a0 * m_in2 -
	    m_a1a0 * m_ou1 -
	    m_a2a0 * m_ou2;
   
	// push in/out buffers
	m_in2 = m_in1;
	m_in1 = _in0;
	m_ou2 = m_ou1;

	m_ou1 = out;
	break;
    }

    return out;
}

} /* namespace psynth */
