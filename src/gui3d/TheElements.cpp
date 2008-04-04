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

#include <libpsynth/object/ObjectMixer.h>
#include <libpsynth/object/ObjectOscillator.h>
#include <libpsynth/object/ObjectLFO.h>
#include <libpsynth/object/ObjectFilter.h>
#include <libpsynth/object/ObjectSampler.h>
#include <libpsynth/object/ObjectStepSeq.h>
#include <libpsynth/object/ObjectNoise.h>

using namespace std;
using namespace Ogre;
using namespace psynth;

ElementMixer::ElementMixer(TableObject& obj,
			   Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{

    static const char* mesh_names[ObjectMixer::N_MIXOPS] =
	{"mixer.mesh", "mixer_ring.mesh"};

    static const char* mixop_names[ObjectMixer::N_MIXOPS] =
	{"Sum", "Product"};

    addComponent(new ElemMultiMainComponent(
		     ObjectMixer::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     ObjectMixer::PARAM_MIXOP,
		     mesh_names));
	
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectMixer::PARAM_MIXOP,
					ObjectMixer::N_MIXOPS,
					mixop_names,
					"Operation"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectMixer::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementControlMixer::ElementControlMixer(TableObject& obj,
			   Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{

    static const char* mesh_names[ObjectMixer::N_MIXOPS] =
	{"ctrl_mixer.mesh", "ctrl_mixer_ring.mesh"};

    static const char* mixop_names[ObjectMixer::N_MIXOPS] =
	{"Sum", "Product"};

    addComponent(new ElemMultiMainComponent(
		     ObjectMixer::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     ObjectMixer::PARAM_MIXOP,
		     mesh_names));
	
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectMixer::PARAM_MIXOP,
					ObjectMixer::N_MIXOPS,
					mixop_names,
					"Operation"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectMixer::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementOscillator::ElementOscillator(TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[ObjectOscillator::N_OSC_TYPES] =
	{"osc_sine.mesh", "osc_square.mesh", "osc_triangle.mesh",
	 "osc_sawtooth.mesh", "osc_moogsaw.mesh", "osc_exp.mesh"};

    static const char* wave_names[ObjectOscillator::N_OSC_TYPES] =
	{"Sine", "Square", "Triangle", "Sawtooth", "Moogsaw", "Exp."};

    static const char* mod_names[ObjectOscillator::N_MOD_TYPES] =
	{"Frequency", "Amplitude", "Phase"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectOscillator::PARAM_FREQUENCY,
		     20.0f, 5000.0f,
		     ObjectOscillator::PARAM_WAVE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     ObjectOscillator::PARAM_AMPLITUDE,
		     0.0f, 1.0f));
    
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectOscillator::PARAM_WAVE,
					ObjectOscillator::N_OSC_TYPES,
					wave_names,
					"Wave"));

    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectOscillator::PARAM_MODULATOR,
					ObjectOscillator::N_MOD_TYPES,
					mod_names,
					"Modulator"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectOscillator::PARAM_FREQUENCY,
					20.0f, 5000.0f,
					"Frequency"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectOscillator::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementLFO::ElementLFO(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[ObjectOscillator::N_OSC_TYPES] =
	{"lfo_sine.mesh", "lfo_square.mesh", "lfo_triangle.mesh",
	 "lfo_sawtooth.mesh", "lfo_moogsaw.mesh", "lfo_exp.mesh"};

    static const char* wave_names[ObjectOscillator::N_OSC_TYPES] =
	{"Sine", "Square", "Triangle", "Sawtooth", "Moogsaw", "Exp."};

    static const char* mod_names[ObjectOscillator::N_MOD_TYPES] =
	{"Frequency", "Amplitude", "Phase"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectOscillator::PARAM_FREQUENCY,
		     0.01f, 30.0f,
		     ObjectOscillator::PARAM_WAVE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     ObjectOscillator::PARAM_AMPLITUDE,
		     0.0f, 1.0f));

    
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectOscillator::PARAM_WAVE,
					ObjectOscillator::N_OSC_TYPES,
					wave_names,
					"Wave"));

    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectOscillator::PARAM_MODULATOR,
					ObjectOscillator::N_MOD_TYPES,
					mod_names,
					"Modulator"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectOscillator::PARAM_FREQUENCY,
					0.01f, 30.0f,
					"Frequency"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectOscillator::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementFilter::ElementFilter(TableObject& obj,
		       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{	    
    static const char* mesh_names[ObjectFilter::N_FILTER_TYPES] =
	{"filter_lowpass.mesh", "filter_highpass.mesh",
	 "filter_bandpass.mesh", "filter_bandpass.mesh",
	 "filter_notch.mesh", "filter_moog.mesh"};

    static const char* filter_names[ObjectFilter::N_FILTER_TYPES] =
	{"Lowpass", "Highpass",
	 "Bandpass CSG", "Bandpass CZPG",
	 "Notch", "Moog"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectFilter::PARAM_CUTOFF,
		     20.0f, 5000.0f,
		     ObjectFilter::PARAM_TYPE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     ObjectFilter::PARAM_RESONANCE,
		     0.0f, 1.0f));

    
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectFilter::PARAM_TYPE,
					ObjectFilter::N_FILTER_TYPES,
					filter_names,
					"Filter"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectFilter::PARAM_CUTOFF,
					20.0f, 5000.0f,
					"Cut-off Freq."));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectFilter::PARAM_RESONANCE,
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
		     ObjectSampler::PARAM_RATE,
		     -3.0f, 3.0f));
    
    addComponent(new ElemSecondComponent(
		     ObjectSampler::PARAM_AMPLITUDE,
		     0.0f, 1.0f));


    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectSampler::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
    
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectSampler::PARAM_RATE,
					-3.0f, 3.0f,
					"Rate"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectSampler::PARAM_TEMPO,
					0.2f, 3.0f,
					"Tempo"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectSampler::PARAM_PITCH,
					0.2f, 3.0f,
					"Pitch"));
}

ElementStepSeq::ElementStepSeq(TableObject& obj,
			       Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[ObjectOscillator::N_OSC_TYPES] =
	{"stepseq_square.mesh", "stepseq_triangle.mesh",
	 "stepseq_sawtooth.mesh", "stepseq_bwsawtooth.mesh"};

    static const char* shape_names[ObjectOscillator::N_OSC_TYPES] =
	{"Square", "Triangle", "FW Sawtooth", "BW Sawtooth"};
    

    addComponent(new ElemMultiMainComponent(
		     ObjectStepSeq::PARAM_BPM,
		     40.0f, 400.0f,
		     ObjectStepSeq::PARAM_SHAPE,
		     mesh_names));
    
    addComponent(new ElemSecondComponent(
		     ObjectStepSeq::PARAM_HIGH,
		     0.1f, 1.0f));

    addComponent(new ElemTogglerComponent(
		     ObjectStepSeq::PARAM_NUM_STEPS,
		     ObjectStepSeq::PARAM_STEP_0,
		     ObjectStepSeq::PARAM_CURRENT_STEP));

    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectStepSeq::PARAM_SHAPE,
					ObjectStepSeq::N_SHAPES,
					shape_names,
					"Shape"));
    getGUIProperties().addParameter(new ElemGuiParamInt(
					ObjectStepSeq::PARAM_NUM_STEPS,
					1, 32,
					"Steps"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectStepSeq::PARAM_BPM,
					40.0f, 400.0f,
					"BPM"));
    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectStepSeq::PARAM_HIGH,
					0.1f, 1.0f,
					"Hold"));
}

ElementAudioNoise::ElementAudioNoise(TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[ObjectNoise::N_TYPES] =
	{"audio_noise_white.mesh", "audio_noise_pink.mesh"};

    static const char* type_names[ObjectOscillator::N_OSC_TYPES] =
	{"White", "Pink"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectNoise::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     ObjectNoise::PARAM_TYPE,
		     mesh_names));
        
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectNoise::PARAM_TYPE,
					ObjectNoise::N_TYPES,
					type_names,
					"Type"));

    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectNoise::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

ElementControlNoise::ElementControlNoise(TableObject& obj,
				     Ogre::SceneManager* m_scene) :
    Element(obj, m_scene)
{
    static const char* mesh_names[ObjectNoise::N_TYPES] =
	{"control_noise_white.mesh", "control_noise_pink.mesh"};

    static const char* type_names[ObjectOscillator::N_OSC_TYPES] =
	{"White", "Pink"};
    
    addComponent(new ElemMultiMainComponent(
		     ObjectNoise::PARAM_AMPLITUDE,
		     0.0f, 1.0f,
		     ObjectNoise::PARAM_TYPE,
		     mesh_names));
        
    getGUIProperties().addParameter(new ElemGuiParamMulti(
					ObjectNoise::PARAM_TYPE,
					ObjectNoise::N_TYPES,
					type_names,
					"Type"));

    getGUIProperties().addParameter(new ElemGuiParamFloat(
					ObjectNoise::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}
