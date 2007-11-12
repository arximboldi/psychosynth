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

#include "common/AudioBuffer.h"
#include "common/Error.h"

enum {
    OUT_NONE,
    OUT_ALSA,
    OUT_OSS,
    OUT_TYPES,
};

enum {
    OUT_NOTINIT,
    OUT_IDLE,
    OUT_RUNNING,
    OUT_STATES
};

class Output {
protected:
    int out_state;
    AudioInfo out_info;
	
    void *out_cbdata;
    void (*out_callback) (int,void*);

public:
    Output(AudioInfo info = AudioInfo(), void (*callback)(int,void*) = NULL, void* data = NULL) :
	out_info(info),
	out_cbdata(data),
	out_callback(callback)
	{}

    virtual ~Output() {};
	
    virtual bool open() = 0;
    virtual bool close()  = 0;
    virtual bool put(const Real* rbuf, int nframes) = 0;
    virtual bool put(const AudioBuffer& buf) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
	
    bool setInfo(const AudioInfo& info) {
	if (out_state == OUT_NOTINIT) {
	    out_info = info;
	    return true;
	} else {
	    WARNING("Cannot change parameters of output device once initialized.");
	    return false;
	}
    }
	
    bool setCallback(void(*callback)(int,void*), void* data) {
	if (out_state == OUT_NOTINIT) {
	    out_cbdata = data;
	    out_callback = callback;
	    return true;
	} else {
	    WARNING("Cannot change parameters of output device once initialized.");
	    return false;
	}
    }
	
    const AudioInfo& getInfo() const {
	return out_info;
    }
};

#endif
