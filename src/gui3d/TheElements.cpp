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

#include "gui3d/TheElements.h"

#include "gui3d/ElemMainComponent.h"
#include "gui3d/ElemSecondComponent.h"
#include "gui3d/ElemTogglerComponent.h"

#include <libpsynth/node/node_mixer.h>
#include <libpsynth/node/node_oscillator.h>
#include <libpsynth/node/node_filter.h>
#include <libpsynth/node/node_sampler.h>
#include <libpsynth/node/node_step_seq.h>
#include <libpsynth/node/node_noise.h>
#include <libpsynth/node/node_echo.h>
#include <libpsynth/node/node_delay.h>

using namespace std;
using namespace Ogre;
using namespace psynth;

ElementMixer::ElementMixer(TableObject& obj,
			   Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{

    static const char* mesh_names[node_mixer::N_MIXOPS] =
	{"mixer.mesh", "mixer_ring.mesh"};

    static const char* mixop_names[node_mixer::N_MIXOPS] =
	{"Sum", "Product"};

    addComponent(new ElemMultiMainComponent(
		     node_mixer::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     node_mixer::PARAM_MIXOP,
		     mesh_names));
	
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_mixer::PARAM_MIXOP,
					node_mixer::N_MIXOPS,
					mixop_names,
					"Operation"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_mixer::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementControlMixer::ElementControlMixer(TableObject& obj,
			   Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{

    static const char* mesh_names[node_mixer::N_MIXOPS] =
	{"ctrl_mixer.mesh", "ctrl_mixer_ring.mesh"};

    static const char* mixop_names[node_mixer::N_MIXOPS] =
	{"Sum", "Product"};

    addComponent(new ElemMultiMainComponent(
		     node_mixer::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     node_mixer::PARAM_MIXOP,
		     mesh_names));
	
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_mixer::PARAM_MIXOP,
					node_mixer::N_MIXOPS,
					mixop_names,
					"Operation"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_mixer::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementOscillator::ElementOscillator(TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[node_oscillator::N_OSC_TYPES] =
	{"osc_sine.mesh", "osc_square.mesh", "osc_triangle.mesh",
	 "osc_sawtooth.mesh", "osc_moogsaw.mesh", "osc_exp.mesh"};

    static const char* wave_names[node_oscillator::N_OSC_TYPES] =
	{"Sine", "Square", "Triangle", "Sawtooth", "Moogsaw", "Exp."};

    static const char* mod_names[node_oscillator::N_MOD_TYPES] =
	{"Frequency", "Amplitude", "Phase"};
    
    addComponent(new ElemMultiMainComponent(
		     node_oscillator::PARAM_FREQUENCY,
		     20.0f, 5000.0f,
		     node_oscillator::PARAM_WAVE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     node_oscillator::PARAM_AMPLITUDE,
		     0.0f, 1.0f));
    
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_oscillator::PARAM_WAVE,
					node_oscillator::N_OSC_TYPES,
					wave_names,
					"Wave"));

    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_oscillator::PARAM_MODULATOR,
					node_oscillator::N_MOD_TYPES,
					mod_names,
					"Modulator"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_oscillator::PARAM_FREQUENCY,
					20.0f, 5000.0f,
					"Frequency"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_oscillator::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementLFO::ElementLFO(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[node_oscillator::N_OSC_TYPES] =
	{"lfo_sine.mesh", "lfo_square.mesh", "lfo_triangle.mesh",
	 "lfo_sawtooth.mesh", "lfo_moogsaw.mesh", "lfo_exp.mesh"};

    static const char* wave_names[node_oscillator::N_OSC_TYPES] =
	{"Sine", "Square", "Triangle", "Sawtooth", "Moogsaw", "Exp."};

    static const char* mod_names[node_oscillator::N_MOD_TYPES] =
	{"Frequency", "Amplitude", "Phase"};
    
    addComponent(new ElemMultiMainComponent(
		     node_oscillator::PARAM_FREQUENCY,
		     0.01f, 30.0f,
		     node_oscillator::PARAM_WAVE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     node_oscillator::PARAM_AMPLITUDE,
		     0.0f, 1.0f));

    
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_oscillator::PARAM_WAVE,
					node_oscillator::N_OSC_TYPES,
					wave_names,
					"Wave"));

    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_oscillator::PARAM_MODULATOR,
					node_oscillator::N_MOD_TYPES,
					mod_names,
					"Modulator"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_oscillator::PARAM_FREQUENCY,
					0.01f, 30.0f,
					"Frequency"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_oscillator::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementFilter::ElementFilter(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{	    
    static const char* mesh_names[node_filter::N_FILTER_TYPES] =
	{"filter_lowpass.mesh", "filter_highpass.mesh",
	 "filter_bandpass.mesh", "filter_bandpass.mesh",
	 "filter_notch.mesh", "filter_moog.mesh"};

    static const char* filter_names[node_filter::N_FILTER_TYPES] =
	{"Lowpass", "Highpass",
	 "Bandpass CSG", "Bandpass CZPG",
	 "Notch", "Moog"};
    
    addComponent(new ElemMultiMainComponent(
		     node_filter::PARAM_CUTOFF,
		     20.0f, 5000.0f,
		     node_filter::PARAM_TYPE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     node_filter::PARAM_RESONANCE,
		     0.0f, 1.0f));

    
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_filter::PARAM_TYPE,
					node_filter::N_FILTER_TYPES,
					filter_names,
					"Filter"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_filter::PARAM_CUTOFF,
					20.0f, 5000.0f,
					"Cut-off Freq."));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_filter::PARAM_RESONANCE,
					0.0f, 1.0f,
					"Resonance"));
}


ElementSampler::ElementSampler(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{	    
    static const char* mesh_name = "sampler.mesh";
    
    addComponent(new ElemMainComponent(
		     string(mesh_name), 
		     node_sampler::PARAM_RATE,
		     -3.0f, 3.0f));
    
    addComponent(new ElemSecondComponent(
		     node_sampler::PARAM_AMPLITUDE,
		     0.0f, 1.0f));


    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_sampler::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_sampler::PARAM_RATE,
					-3.0f, 3.0f,
					"Rate"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_sampler::PARAM_TEMPO,
					0.2f, 3.0f,
					"Tempo"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_sampler::PARAM_PITCH,
					0.2f, 3.0f,
					"Pitch"));
}

ElementStepSeq::ElementStepSeq(TableObject& obj,
			       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[node_oscillator::N_OSC_TYPES] =
	{"stepseq_square.mesh", "stepseq_triangle.mesh",
	 "stepseq_sawtooth.mesh", "stepseq_bwsawtooth.mesh"};

    static const char* shape_names[node_oscillator::N_OSC_TYPES] =
	{"Square", "Triangle", "FW Sawtooth", "BW Sawtooth"};
    

    addComponent(new ElemMultiMainComponent(
		     node_step_seq::PARAM_BPM,
		     40.0f, 400.0f,
		     node_step_seq::PARAM_SHAPE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     node_step_seq::PARAM_HIGH,
		     0.1f, 1.0f));

    addComponent(new ElemTogglerComponent(
		     node_step_seq::PARAM_NUM_STEPS,
		     node_step_seq::PARAM_STEP_0,
		     node_step_seq::PARAM_CURRENT_STEP));

    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_step_seq::PARAM_SHAPE,
					node_step_seq::N_SHAPES,
					shape_names,
					"Shape"));
    getGUIProperties().addParameter(new ElemGuiParamInt(
					node_step_seq::PARAM_NUM_STEPS,
					1, 32,
					"Steps"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_step_seq::PARAM_BPM,
					40.0f, 400.0f,
					"BPM"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_step_seq::PARAM_HIGH,
					0.1f, 1.0f,
					"Hold"));
}

ElementAudioNoise::ElementAudioNoise(TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[node_noise::N_TYPES] =
	{"audio_noise_white.mesh", "audio_noise_pink.mesh"};

    static const char* type_names[node_oscillator::N_OSC_TYPES] =
	{"White", "Pink"};
    
    addComponent(new ElemMultiMainComponent(
		     node_noise::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     node_noise::PARAM_TYPE,
		     mesh_names));
        
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_noise::PARAM_TYPE,
					node_noise::N_TYPES,
					type_names,
					"Type"));

    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_noise::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementControlNoise::ElementControlNoise(TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[node_noise::N_TYPES] =
	{"control_noise_white.mesh", "control_noise_pink.mesh"};

    static const char* type_names[node_oscillator::N_OSC_TYPES] =
	{"White", "Pink"};
    
    addComponent(new ElemMultiMainComponent(
		     node_noise::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     node_noise::PARAM_TYPE,
		     mesh_names));
        
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					node_noise::PARAM_TYPE,
					node_noise::N_TYPES,
					type_names,
					"Type"));

    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_noise::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementEcho::ElementEcho(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{	    
    static const char* mesh_name = "echo.mesh";
    
    addComponent(new ElemMainComponent(
		     string(mesh_name), 
		     node_echo::PARAM_DELAY,
		     0.0f, 1.0f));
    
    addComponent(new ElemSecondComponent(
		     node_echo::PARAM_FEEDBACK,
		     0.0f, 1.0f));


    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_echo::PARAM_DELAY,
					0.0f, 1.0f,
					"Delay"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_echo::PARAM_FEEDBACK,
					0.0f, 1.0f,
					"Feedback"));
}

ElementDelay::ElementDelay(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{	    
    static const char* mesh_name = "delay.mesh";
    
    addComponent(new ElemMainComponent(
		     string(mesh_name), 
		     node_delay::PARAM_DELAY,
		     0.0f, 0.01f));
    
    addComponent(new ElemSecondComponent(
		     node_delay::PARAM_DEPTH,
		     0.0f, 1.0f));


    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_delay::PARAM_DELAY,
					0.0f, 0.01f,
					"Delay"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					node_delay::PARAM_DEPTH,
					0.0f, 1.0f,
					"Depth"));
}
