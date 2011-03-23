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

#include "gui3d/the_elements.hpp"

#include "gui3d/elem_main_component.hpp"
#include "gui3d/elem_second_component.hpp"
#include "gui3d/elem_toggler_component.hpp"

#include <psynth/graph/node_mixer.hpp>
#include <psynth/graph/node_oscillator.hpp>
#include <psynth/graph/node_filter.hpp>
#include <psynth/graph/node_sampler.hpp>
#include <psynth/graph/node_step_seq.hpp>
#include <psynth/graph/node_noise.hpp>
#include <psynth/graph/node_echo.hpp>
#include <psynth/graph/node_delay.hpp>
#include <psynth/graph/node_double_sampler.hpp>

using namespace std;
using namespace Ogre;
using namespace psynth;

element_mixer::element_mixer(world_node& obj,
			     Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{

    static const char* mesh_names[graph::node_mixer::N_MIXOPS] =
	{"mixer.mesh", "mixer_ring.mesh"};

    static const char* mixop_names[graph::node_mixer::N_MIXOPS] =
	{"Sum", "Product"};

    add_component (new elem_multi_main_component(
		       graph::node_mixer::PARAM_AMPLITUDE,
		       0.0f, 1.0f,
		       graph::node_mixer::PARAM_MIXOP,
		       mesh_names));
	
    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_mixer::PARAM_MIXOP,
					graph::node_mixer::N_MIXOPS,
					mixop_names,
					"Operation"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_mixer::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

element_control_mixer::element_control_mixer(world_node& obj,
					   Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{

    static const char* mesh_names[graph::node_mixer::N_MIXOPS] =
	{"ctrl_mixer.mesh", "ctrl_mixer_ring.mesh"};

    static const char* mixop_names[graph::node_mixer::N_MIXOPS] =
	{"Sum", "Product"};

    add_component (new elem_multi_main_component(
		       graph::node_mixer::PARAM_AMPLITUDE,
		       0.0f, 1.0f,
		       graph::node_mixer::PARAM_MIXOP,
		       mesh_names));
	
    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_mixer::PARAM_MIXOP,
					graph::node_mixer::N_MIXOPS,
					mixop_names,
					"Operation"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_mixer::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

element_oscillator::element_oscillator(world_node& obj,
				       Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{
    static const char* mesh_names[graph::node_oscillator::N_OSC_TYPES] =
	{"osc_sine.mesh", "osc_square.mesh", "osc_triangle.mesh",
	 "osc_sawtooth.mesh", "osc_moogsaw.mesh", "osc_exp.mesh"};

    static const char* wave_names[graph::node_oscillator::N_OSC_TYPES] =
	{"Sine", "Square", "Triangle", "Sawtooth", "Moogsaw", "Exp."};

    static const char* mod_names[graph::node_oscillator::N_MOD_TYPES] =
	{"Frequency", "Amplitude", "Phase"};
    
    add_component (new elem_multi_main_component(
		       graph::node_oscillator::PARAM_FREQUENCY,
		       20.0f, 5000.0f,
		       graph::node_oscillator::PARAM_WAVE,
		       mesh_names));
    
    add_component (new elem_second_component(
		       graph::node_oscillator::PARAM_AMPLITUDE,
		       0.0f, 1.0f));
    
    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_oscillator::PARAM_WAVE,
					graph::node_oscillator::N_OSC_TYPES,
					wave_names,
					"Wave"));

    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_oscillator::PARAM_MODULATOR,
					graph::node_oscillator::N_MOD_TYPES,
					mod_names,
					"Modulator"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_oscillator::PARAM_FREQUENCY,
					20.0f, 5000.0f,
					"Frequency"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_oscillator::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

element_lfo::element_lfo (world_node& obj,
			 Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{
    static const char* mesh_names[graph::node_oscillator::N_OSC_TYPES] =
	{"lfo_sine.mesh", "lfo_square.mesh", "lfo_triangle.mesh",
	 "lfo_sawtooth.mesh", "lfo_moogsaw.mesh", "lfo_exp.mesh"};

    static const char* wave_names[graph::node_oscillator::N_OSC_TYPES] =
	{"Sine", "Square", "Triangle", "Sawtooth", "Moogsaw", "Exp."};

    static const char* mod_names[graph::node_oscillator::N_MOD_TYPES] =
	{"Frequency", "Amplitude", "Phase"};
    
    add_component (new elem_multi_main_component(
		       graph::node_oscillator::PARAM_FREQUENCY,
		       0.01f, 30.0f,
		       graph::node_oscillator::PARAM_WAVE,
		       mesh_names));
    
    add_component (new elem_second_component(
		       graph::node_oscillator::PARAM_AMPLITUDE,
		       0.0f, 1.0f));

    
    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_oscillator::PARAM_WAVE,
					graph::node_oscillator::N_OSC_TYPES,
					wave_names,
					"Wave"));

    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_oscillator::PARAM_MODULATOR,
					graph::node_oscillator::N_MOD_TYPES,
					mod_names,
					"Modulator"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_oscillator::PARAM_FREQUENCY,
					0.01f, 30.0f,
					"Frequency"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_oscillator::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

element_filter::element_filter (world_node& obj,
			       Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{	    
    static const char* mesh_names[graph::node_filter::N_FILTER_TYPES] =
	{"filter_lowpass.mesh", "filter_highpass.mesh",
	 "filter_bandpass.mesh", "filter_bandpass.mesh",
	 "filter_notch.mesh", "filter_moog.mesh"};

    static const char* filter_names[graph::node_filter::N_FILTER_TYPES] =
	{"Lowpass", "Highpass",
	 "Bandpass CSG", "Bandpass CZPG",
	 "Notch", "Moog"};
    
    add_component (new elem_multi_main_component(
		       graph::node_filter::PARAM_CUTOFF,
		       20.0f, 5000.0f,
		       graph::node_filter::PARAM_TYPE,
		       mesh_names));
    
    add_component (new elem_second_component(
		       graph::node_filter::PARAM_RESONANCE,
		       0.0f, 1.0f));

    
    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_filter::PARAM_TYPE,
					graph::node_filter::N_FILTER_TYPES,
					filter_names,
					"Filter"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_filter::PARAM_CUTOFF,
					20.0f, 5000.0f,
					"Cut-off Freq."));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_filter::PARAM_RESONANCE,
					0.0f, 1.0f,
					"Resonance"));
}


element_sampler::element_sampler (world_node& obj,
				 Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{	    
    static const char* mesh_name = "sampler.mesh";
    
    add_component (new elem_main_component(
		       string(mesh_name), 
		       graph::node_sampler::PARAM_RATE,
		       -3.0f, 3.0f));
    
    add_component (new elem_second_component(
		       graph::node_sampler::PARAM_AMPLITUDE,
		       0.0f, 1.0f));


    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_sampler::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_sampler::PARAM_RATE,
					-3.0f, 3.0f,
					"Rate"));
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_sampler::PARAM_TEMPO,
					0.2f, 3.0f,
					"Tempo"));
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_sampler::PARAM_PITCH,
					0.2f, 3.0f,
					"Pitch"));
}

element_step_seq::element_step_seq (world_node& obj,
				    Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{
    static const char* mesh_names[graph::node_oscillator::N_OSC_TYPES] =
	{"stepseq_square.mesh", "stepseq_triangle.mesh",
	 "stepseq_sawtooth.mesh", "stepseq_bwsawtooth.mesh"};

    static const char* shape_names[graph::node_oscillator::N_OSC_TYPES] =
	{"Square", "Triangle", "FW Sawtooth", "BW Sawtooth"};
    

    add_component (new elem_multi_main_component(
		       graph::node_step_seq::PARAM_BPM,
		       40.0f, 400.0f,
		       graph::node_step_seq::PARAM_SHAPE,
		       mesh_names));
    
    add_component (new elem_second_component(
		       graph::node_step_seq::PARAM_HIGH,
		       0.1f, 1.0f));

    add_component (new elem_toggler_component(
		       graph::node_step_seq::PARAM_NUM_STEPS,
		       graph::node_step_seq::PARAM_STEP_0,
		       graph::node_step_seq::PARAM_CURRENT_STEP));

    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_step_seq::PARAM_SHAPE,
					graph::node_step_seq::N_SHAPES,
					shape_names,
					"Shape"));
    get_gui_properties().add_parameter (new elem_gui_param_int(
					graph::node_step_seq::PARAM_NUM_STEPS,
					1, 32,
					"Steps"));
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_step_seq::PARAM_BPM,
					40.0f, 400.0f,
					"BPM"));
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_step_seq::PARAM_HIGH,
					0.1f, 1.0f,
					"Hold"));
}

element_audio_noise::element_audio_noise (world_node& obj,
					 Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{
    static const char* mesh_names[graph::node_noise::N_TYPES] =
	{"audio_noise_white.mesh", "audio_noise_pink.mesh"};

    static const char* type_names[graph::node_oscillator::N_OSC_TYPES] =
	{"White", "Pink"};
    
    add_component (new elem_multi_main_component(
		       graph::node_noise::PARAM_AMPLITUDE,
		       0.0f, 1.0f,
		       graph::node_noise::PARAM_TYPE,
		       mesh_names));
        
    get_gui_properties().add_parameter  (new elem_gui_param_multi(
					    graph::node_noise::PARAM_TYPE,
					    graph::node_noise::N_TYPES,
					    type_names,
					    "Type"));

    get_gui_properties().add_parameter  (new elem_gui_param_float(
					    graph::node_noise::PARAM_AMPLITUDE,
					    0.0f, 1.0f,
					    "Amplitude"));
}

element_control_noise::element_control_noise (world_node& obj,
					     Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{
    static const char* mesh_names[graph::node_noise::N_TYPES] =
	{"control_noise_white.mesh", "control_noise_pink.mesh"};

    static const char* type_names[graph::node_oscillator::N_OSC_TYPES] =
	{"White", "Pink"};
    
    add_component (new elem_multi_main_component(
		       graph::node_noise::PARAM_AMPLITUDE,
		       0.0f, 1.0f,
		       graph::node_noise::PARAM_TYPE,
		       mesh_names));
        
    get_gui_properties().add_parameter (new elem_gui_param_multi(
					graph::node_noise::PARAM_TYPE,
					graph::node_noise::N_TYPES,
					type_names,
					"Type"));

    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_noise::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
}

element_echo::element_echo (world_node& obj,
			   Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{	    
    static const char* mesh_name = "echo.mesh";
    
    add_component (new elem_main_component(
		       string(mesh_name), 
		       graph::node_echo::PARAM_DELAY,
		       0.0f, 1.0f));
    
    add_component (new elem_second_component(
		       graph::node_echo::PARAM_FEEDBACK,
		       0.0f, 1.0f));


    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_echo::PARAM_DELAY,
					0.0f, 1.0f,
					"Delay"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_echo::PARAM_FEEDBACK,
					0.0f, 1.0f,
					"Feedback"));
}

element_delay::element_delay (world_node& obj,
			      Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{	    
    static const char* mesh_name = "delay.mesh";
    
    add_component (new elem_main_component (
		       string(mesh_name), 
		       graph::node_delay::PARAM_DELAY,
		       0.0f, 0.01f));
    
    add_component (new elem_second_component(
		       graph::node_delay::PARAM_DEPTH,
		       0.0f, 1.0f));


    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_delay::PARAM_DELAY,
					0.0f, 0.01f,
					"Delay"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_delay::PARAM_DEPTH,
					0.0f, 1.0f,
					"Depth"));
}

element_double_sampler::element_double_sampler (world_node& obj,
						Ogre::SceneManager* m_scene) :
    element (obj, m_scene)
{	    
    static const char* mesh_name = "double_sampler.mesh";
    
    add_component (new elem_main_component(
		       string(mesh_name), 
		       graph::node_double_sampler::PARAM_BLEND,
		       0.0f, 1.0f));
    
    add_component (new elem_second_component(
		       graph::node_double_sampler::PARAM_AMPLITUDE,
		       0.0f, 1.0f));


    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_double_sampler::PARAM_AMPLITUDE,
					0.0f, 1.0f,
					"Amplitude"));
    
    get_gui_properties().add_parameter (new elem_gui_param_float(
					graph::node_double_sampler::PARAM_BLEND,
					0.0f, 1.0f,
					"Blend"));
}
