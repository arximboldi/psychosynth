/**
 *  Time-stamp:  <2010-10-18 02:50:35 raskolnikov>
 *
 *  @file        default_selector_populator.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  Populates the node selector.
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <cstring>
#include <algorithm>
#include <list>
#include <string>
#include <dirent.h>

#include <psynth/version.hpp>
#include <psynth/base/misc.hpp>
#include <psynth/base/file_manager.hpp>
#include <psynth/world/world_node_creator.hpp>

#include <psynth/node/node_mixer.hpp>
#include <psynth/node/node_oscillator.hpp>
#include <psynth/node/node_filter.hpp>
#include <psynth/node/node_step_seq.hpp>
#include <psynth/node/node_noise.hpp>

#include "gui3d/selector_window.hpp"
#include "gui3d/default_selector_populator.hpp"

using namespace std;
using namespace psynth;
using namespace psynth::base;

void default_selector_populator::on_samples_conf_nudge (psynth::base::conf_node& conf)
{
    selector_window::category* cat = m_selector->find_category ("Samples");

    if (cat) {
	cat->clear_buttons ();
	populate_samples (cat);
    }
}

void default_selector_populator::populate_samples (selector_window::category* cat)
{
    world_node_creator creat;
    
    creat.set_name ("sampler");
    
    base::file_mgr_node::path_list files;
    list<string> valid_ext;

#ifdef PSYNTH_HAVE_PCM
    valid_ext.push_back (".au");
    valid_ext.push_back (".wav");
    valid_ext.push_back (".aiff");
#endif
#ifdef PSYNTH_HAVE_OGG    
    valid_ext.push_back (".ogg");
#endif
    
    base::file_manager::self  ()
	.get_path ("psychosynth.samples")
	.find_if (make_has_extension (valid_ext.begin(), valid_ext.end()), files);

    files.sort ();
    
    for (file_mgr_node::path_list::iterator it = files.begin();
	 it != files.end (); ++it)
    {
	creat.set_param ("file", it->leaf ());
	cat->add_button (it->leaf (), creat);
    }
}

void default_selector_populator::populate(selector_window* sel)
{
    m_selector = sel;
    config::self ()
	.get_path ("psychosynth.file_manager.samples").on_nudge.connect
	(sigc::mem_fun (*this, &default_selector_populator::on_samples_conf_nudge));
    
    selector_window::category* cat = NULL;
    world_node_creator creat;

    cat = sel->add_category ("Generator");
    creat.set_name ("oscillator");
    creat.set_param ("frequency", (float) 110.0f);
    creat.set_param ("amplitude", (float) 0.3f);
    
    creat.set_param ("wave", (int) node_oscillator::OSC_SINE);
    cat->add_button ("Sine", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SQUARE);
    cat->add_button ("Square", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SAWTOOTH);
    cat->add_button ("Sawtooth", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_TRIANGLE);
    cat->add_button ("Triangle", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_MOOGSAW);
    cat->add_button ("Moogsaw", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_EXP);
    cat->add_button ("Exponential", creat);

    creat.clear();
    creat.set_name ("audio_noise");
    creat.set_param ("type", (int) node_noise::NOISE_PINK);
    cat->add_button ("Pink Noise", creat);
    creat.set_param ("type", (int) node_noise::NOISE_WHITE);
    cat->add_button ("White Noise", creat);
    
    creat.clear();
    cat = sel->add_category ("Control");
    creat.set_name ("lfo");
    creat.set_param ("frequency", 1.0f);
    creat.set_param ("amplitude", 0.7f);
    
    creat.set_param ("wave", (int) node_oscillator::OSC_SINE);
    cat->add_button ("Sine", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SQUARE);
    cat->add_button ("Square", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SAWTOOTH);
    cat->add_button ("Sawtooth", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_TRIANGLE);
    cat->add_button ("Triangle", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_MOOGSAW);
    cat->add_button ("Moogsaw", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_EXP);
    cat->add_button ("Exponential", creat);

    creat.clear();
    creat.set_name ("control_noise");
    creat.set_param ("type", (int) node_noise::NOISE_PINK);
    cat->add_button ("Pink Noise", creat);
    creat.set_param ("type", (int) node_noise::NOISE_WHITE);
    cat->add_button ("White Noise", creat);
    
    creat.clear();
    cat = sel->add_category ("Filter");
    creat.set_name ("filter");
    creat.set_param ("resonance", (float) 0.8f);
	
    creat.set_param ("type", (int) node_filter::FILTER_LOWPASS);
    cat->add_button ("Low-pass", creat);
    creat.set_param ("type", (int) node_filter::FILTER_HIGHPASS);
    cat->add_button ("Hi-pass", creat);
    creat.set_param ("type", (int) node_filter::FILTER_BANDPASS_CSG);
    cat->add_button ("Band-pass CSG", creat);
    creat.set_param ("type", (int) node_filter::FILTER_BANDPASS_CZPG);
    cat->add_button ("Band-pass CZPG", creat);
    creat.set_param ("type", (int) node_filter::FILTER_NOTCH);
    cat->add_button ("Notch", creat);
    creat.set_param ("type", (int) node_filter::FILTER_MOOG);
    cat->add_button ("Moog", creat);

    creat.clear();
    cat = sel->add_category ("Effects");
    creat.set_name ("echo");
    cat->add_button ("Echo", creat);

    creat.set_name ("delay");
    cat->add_button ("Delay", creat);
    
    creat.clear();
    cat = sel->add_category ("Mixers");
    creat.set_name ("audio_mixer");

    creat.set_param ("mixop", (int) node_mixer::MIX_SUM);
    cat->add_button ("Audio Sum", creat);
    creat.set_param ("mixop", (int) node_mixer::MIX_PRODUCT);
    cat->add_button ("Audio Mul", creat);

    creat.set_name ("control_mixer");

    creat.set_param ("mixop", (int) node_mixer::MIX_SUM);
    cat->add_button ("Control Sum", creat);
    creat.set_param ("mixop", (int) node_mixer::MIX_PRODUCT);
    cat->add_button ("Control Mul", creat);

    creat.clear();
    cat = sel->add_category ("Sequencing");
    creat.set_name ("stepseq");
    creat.set_param ("shape", (int) node_step_seq::SHAPE_SQUARE);
    cat->add_button ("Step Square", creat);
    creat.set_param ("shape", (int) node_step_seq::SHAPE_TRIANGLE);
    cat->add_button ("Step Triangle", creat);
    creat.set_param ("shape", (int) node_step_seq::SHAPE_FWSAWTOOTH);
    cat->add_button ("Step FW Saw", creat);
    creat.set_param ("shape", (int) node_step_seq::SHAPE_BWSAWTOOTH);
    cat->add_button ("Step BW Saw", creat);
    
    cat = sel->add_category ("Samples");
    /* FIXME: populate_samples (cat); */
    config::self ().get_path ("psychosynth.file_manager.samples").nudge ();
    //config::self ().get_path ("psychosynth.file_manager.samples").nudge ();
    
    creat.clear ();
    cat = sel->add_category ("Bi-Samples");
    creat.set_name ("double_sampler");
    creat.set_param ("file_one", std::string ("pmas1.ogg"));
    creat.set_param ("file_two", std::string ("pmas3.ogg"));
    cat->add_button ("Dub-Step 1-3", creat);
    creat.set_param ("file_one", std::string ("a1.ogg"));
    creat.set_param ("file_two", std::string ("b1.ogg"));
    cat->add_button ("ab1", creat);
    creat.set_param ("file_one", std::string ("a2.ogg"));
    creat.set_param ("file_two", std::string ("b2.ogg"));
    cat->add_button ("ab2", creat);

}
