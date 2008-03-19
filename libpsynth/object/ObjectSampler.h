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

#ifndef PSYNTH_OBJECTSAMPLER_H
#define PSYNTH_OBJECTSAMPLER_H

#include <libpsynth/common/RingAudioBuffer.h>
#include <libpsynth/object/Object.h>
#include <libpsynth/input/FileReaderAny.h>
#include <libpsynth/object/ObjectFactory.h>
#include <libpsynth/common/ScalerST.h>

namespace psynth
{

class Mutex;

class ObjectSampler : public Object
{
public:
    enum InAudioSocketID {
	N_IN_A_SOCKETS
    };
	
    enum InControlSocketID {
	IN_C_RATE,
	N_IN_C_SOCKETS
    };

    enum ParamID {
	PARAM_FILE = Object::N_COMMON_PARAMS,
	PARAM_AMPLITUDE,
	PARAM_RATE,
	PARAM_TEMPO,
	PARAM_PITCH,
	N_PARAM
    };
    
    enum OutAudioSocketID {
	OUT_A_OUTPUT,
	N_OUT_A_SOCKETS
    };

    enum OutControlSocketID {
	N_OUT_C_SOCKETS
    };
    
private:
    FileReaderAny m_reader;
    RingAudioBuffer m_buffer;
    RingAudioBuffer::ReadPtr m_read_ptr;
    AudioBuffer m_inbuf;
    ScalerST m_scaler;
    int m_read_pos;
    int m_end_pos;
    float m_ctrl_pos;
    
    float m_param_ampl;
    float m_param_rate;
    float m_param_tempo;
    float m_param_pitch;
    
    std::string m_param_file;
    
    Mutex m_update_lock;
    
    void onFileChange(ObjParam& par);
	
    void doUpdate(const Object* caller, int caller_port_type, int caller_port);
    void doAdvance();
    void onInfoChange();
    
public:
    ObjectSampler(const AudioInfo& info);   
};

PSYNTH_DECLARE_OBJECT_FACTORY(ObjectSampler, "sampler");

} /* namespace psynth */

#endif /* PSYNTH_OBJECTSAMPLER_H */
