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

#ifndef OUTPUT_H
#define OUTPUT_H

#include <libpsynth/common/AudioBuffer.h>
#include <libpsynth/common/Error.h>

enum {
    OUT_NONE,
    OUT_ALSA,
    OUT_OSS,
    OUT_TYPES,
};

class OutputProcessor
{
public:
    virtual void processOutput(int n_frames) = 0;
};

class Output
{
public:
    enum State {
	NOTINIT,
	IDLE,
	RUNNING,
	N_STATES
    };
    
    typedef void(*callback_t)(int,void*);

private:
    AudioInfo m_info;
    State m_state;
	
    void* m_cbdata;
    callback_t m_callback;

protected:
    void process(int n_frames) {
	if (m_callback)
	    m_callback(n_frames, m_cbdata);
    }

    void setState(State state) {
	m_state = state;
    }
    
    callback_t getCallback() {
	return m_callback;
    }

    void* getCallbackData() {
	return m_cbdata;
    }
    
public:
    
    Output(AudioInfo info = AudioInfo(), void (*callback)(int,void*) = NULL, void* data = NULL) :
	m_info(info),
	m_state(NOTINIT),
	m_cbdata(data),
	m_callback(callback)
	{}

    virtual bool open() = 0;
    virtual bool close()  = 0;
    virtual bool put(const AudioBuffer& buf, size_t nframes) = 0;
    virtual bool start() = 0;
    virtual bool stop() = 0;

    virtual ~Output() {};
    
    bool put(const AudioBuffer& buf) {
	return put(buf, buf.size());
    }
    
    bool setInfo(const AudioInfo& info) {
	if (m_state == NOTINIT) {
	    m_info = info;
	    return true;
	} else {
	    WARNING("Cannot change parameters of output device once initialized.");
	    return false;
	}
    }
	
    bool setCallback(callback_t callback, void* data) {
	if (m_state == NOTINIT) {
	    m_cbdata = data;
	    m_callback = callback;
	    return true;
	} else {
	    WARNING("Cannot change parameters of output device once initialized.");
	    return false;
	}
    }
	
    const AudioInfo& getInfo() const {
	return m_info;
    }

    /* maybe-TODO: A StepMachine class for this kind of things? */
    State getState() const {
	return m_state;
    }

    bool gotoState(State target) {
	if (target > m_state) {
	    switch(m_state) {
	    case NOTINIT:
		return open() && gotoState(target);
	    case IDLE:
		return start() && gotoState(target);
	    default:
		break;
	    }
	}
	
	if (target < m_state) {
	    switch(m_state) {
	    case IDLE:
		return close() && gotoState(target);
	    case RUNNING:
		return stop() && gotoState(target);
	    default:
		break;
	    }
	}
	
	return true;
    }
};

#endif /* OUTPUT_H */

