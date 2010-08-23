/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) 2007 by Juan Pedro Bolivar Puente                       *
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

#ifndef PSYNTH_OUTPUT_H
#define PSYNTH_OUTPUT_H

#include <psynth/synth/audio_buffer.hpp>

namespace psynth
{

class output_processor
{
public:
    virtual void process_output (int n_frames) = 0;
};

class output
{
public:
    enum state {
	NOTINIT,
	IDLE,
	RUNNING,
	N_STATES
    };
    
    typedef void (* callback_t) (int,void*);

private:
    audio_info m_info;
    state m_state;
	
    void* m_cbdata;
    callback_t m_callback;

protected:
    void process(int n_frames) {
	if (m_callback)
	    m_callback(n_frames, m_cbdata);
    }

    void set_state(state state) {
	m_state = state;
    }
    
    callback_t get_callback() {
	return m_callback;
    }

    void* get_callback_data() {
	return m_cbdata;
    }
    
public:
    
    output (audio_info info = audio_info(),
	    void (*callback)(int,void*) = NULL,
	    void* data = NULL)
	: m_info(info)
	, m_state(NOTINIT)
	, m_cbdata(data)
	, m_callback(callback)
	{}

    virtual bool open () = 0;
    virtual bool close ()  = 0;
    virtual bool put (const audio_buffer& buf, size_t nframes) = 0;
    virtual bool start () = 0;
    virtual bool stop () = 0;

    virtual ~output() {};
    
    bool put (const audio_buffer& buf) {
	return put(buf, buf.size());
    }
    
    bool set_info (const audio_info& info) {
	if (m_state == NOTINIT) {
	    m_info = info;
	    return true;
	} else {
	    /*
	      WARNING("Cannot change parameters of output device once initialized.");
	    */
	    return false;
	}
    }
	
    bool set_callback (callback_t callback, void* data) {
	if (m_state == NOTINIT) {
	    m_cbdata = data;
	    m_callback = callback;
	    return true;
	} else {
	    /*
	      WARNING("Cannot change parameters of output device once initialized.");
	    */
	    return false;
	}
    }
	
    const audio_info& get_info () const {
	return m_info;
    }

    /* maybe-TODO: A StepMachine class for this kind of things? */
    state get_state () const {
	return m_state;
    }

    bool goto_state (state target) {
	if (target > m_state) {
	    switch(m_state) {
	    case NOTINIT:
		return open () && goto_state (target);
	    case IDLE:
		return start () && goto_state (target);
	    default:
		break;
	    }
	}
	
	if (target < m_state) {
	    switch(m_state) {
	    case IDLE:
		return close () && goto_state (target);
	    case RUNNING:
		return stop () && goto_state (target);
	    default:
		break;
	    }
	}
	
	return true;
    }
};

} /* namespace psynth */

#endif /* PSYNTH_OUTPUT_H */

