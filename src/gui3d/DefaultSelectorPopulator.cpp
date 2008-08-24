/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2008                          *
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

#include <cstring>
#include <algorithm>
#include <list>
#include <string>
#include <dirent.h>

#include <libpsynth/version.h>
#include <libpsynth/common/misc.h>
#include <libpsynth/common/file_manager.h>
#include <libpsynth/world/world_node_creator.h>

#include <libpsynth/node/node_mixer.h>
#include <libpsynth/node/node_oscillator.h>
#include <libpsynth/node/node_filter.h>
#include <libpsynth/node/node_step_seq.h>
#include <libpsynth/node/node_noise.h>
#include "gui3d/SelectorWindow.h"
#include "gui3d/DefaultSelectorPopulator.h"

using namespace psynth;
using namespace std;

bool DefaultSelectorPopulator::onSamplesConfNudge(psynth::conf_node& conf)
{
    SelectorWindow::Category* cat = m_selector->findCategory("Samples");

    if (cat) {
	cat->clearButtons();
	populateSamples(cat);
    }
    
    return true;
}

void DefaultSelectorPopulator::populateSamples(SelectorWindow::Category* cat)
{
    world_node_creator creat;
    
    creat.set_name ("sampler");
    
    list<string> files;
    list<string> valid_ext;

#ifdef PSYNTH_HAVE_PCM
    valid_ext.push_back("au");
    valid_ext.push_back("wav");
    valid_ext.push_back("aiff");
#endif
#ifdef PSYNTH_HAVE_OGG    
    valid_ext.push_back("ogg");
#endif
    
    file_manager::instance ()
	.get_path ("psychosynth/samples")
	.find_if (make_has_extension(valid_ext.begin(), valid_ext.end()), files);

    for (list<string>::iterator it = files.begin();
	 it != files.end (); ++it) {
	creat.set_param ("file", string(basename(it->c_str())));
	cat->addButton (string(basename(it->c_str())), creat);
    }
}

void DefaultSelectorPopulator::populate(SelectorWindow* sel)
{
    m_selector = sel;
    config::instance()
	.get_path ("psychosynth/file_manager/samples")
	.add_nudge_event (MakeDelegate(this, &DefaultSelectorPopulator::onSamplesConfNudge));
    
    SelectorWindow::Category* cat = NULL;
    world_node_creator creat;

    cat = sel->addCategory("Generator");
    creat.set_name ("oscillator");
    creat.set_param ("frequency", (float) 110.0f);
    creat.set_param ("amplitude", (float) 0.3f);
    
    creat.set_param ("wave", (int) node_oscillator::OSC_SINE);
    cat->addButton("Sine", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SQUARE);
    cat->addButton("Square", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SAWTOOTH);
    cat->addButton("Sawtooth", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_TRIANGLE);
    cat->addButton("Triangle", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_MOOGSAW);
    cat->addButton("Moogsaw", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_EXP);
    cat->addButton("Exponential", creat);

    creat.clear();
    creat.set_name ("audio_noise");
    creat.set_param ("type", (int) node_noise::NOISE_PINK);
    cat->addButton("Pink Noise", creat);
    creat.set_param ("type", (int) node_noise::NOISE_WHITE);
    cat->addButton("White Noise", creat);
    
    creat.clear();
    cat = sel->addCategory("Control");
    creat.set_name ("lfo");
    creat.set_param ("frequency", 1.0f);
    creat.set_param ("amplitude", 0.7f);
    
    creat.set_param ("wave", (int) node_oscillator::OSC_SINE);
    cat->addButton("Sine", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SQUARE);
    cat->addButton("Square", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_SAWTOOTH);
    cat->addButton("Sawtooth", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_TRIANGLE);
    cat->addButton("Triangle", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_MOOGSAW);
    cat->addButton("Moogsaw", creat);
    creat.set_param ("wave", (int) node_oscillator::OSC_EXP);
    cat->addButton("Exponential", creat);

    creat.clear();
    creat.set_name ("control_noise");
    creat.set_param ("type", (int) node_noise::NOISE_PINK);
    cat->addButton("Pink Noise", creat);
    creat.set_param ("type", (int) node_noise::NOISE_WHITE);
    cat->addButton("White Noise", creat);
    
    creat.clear();
    cat = sel->addCategory("Filter");
    creat.set_name ("filter");
    creat.set_param ("resonance", (float) 0.8f);
	
    creat.set_param ("type", (int) node_filter::FILTER_LOWPASS);
    cat->addButton("Low-pass", creat);
    creat.set_param ("type", (int) node_filter::FILTER_HIGHPASS);
    cat->addButton("Hi-pass", creat);
    creat.set_param ("type", (int) node_filter::FILTER_BANDPASS_CSG);
    cat->addButton("Band-pass CSG", creat);
    creat.set_param ("type", (int) node_filter::FILTER_BANDPASS_CZPG);
    cat->addButton("Band-pass CZPG", creat);
    creat.set_param ("type", (int) node_filter::FILTER_NOTCH);
    cat->addButton("Notch", creat);
    creat.set_param ("type", (int) node_filter::FILTER_MOOG);
    cat->addButton("Moog", creat);

    creat.clear();
    cat = sel->addCategory("Effects");
    creat.set_name ("echo");
    cat->addButton("Echo", creat);

    creat.set_name ("delay");
    cat->addButton("Delay", creat);
    
    creat.clear();
    cat = sel->addCategory("Mixers");
    creat.set_name ("audio_mixer");

    creat.set_param ("mixop", (int) node_mixer::MIX_SUM);
    cat->addButton("Audio Sum", creat);
    creat.set_param ("mixop", (int) node_mixer::MIX_PRODUCT);
    cat->addButton("Audio Mul", creat);

    creat.set_name ("control_mixer");

    creat.set_param ("mixop", (int) node_mixer::MIX_SUM);
    cat->addButton("Control Sum", creat);
    creat.set_param ("mixop", (int) node_mixer::MIX_PRODUCT);
    cat->addButton("Control Mul", creat);

    creat.clear();
    cat = sel->addCategory("Sequencing");
    creat.set_name ("stepseq");
    creat.set_param ("shape", (int) node_step_seq::SHAPE_SQUARE);
    cat->addButton("Step Square", creat);
    creat.set_param ("shape", (int) node_step_seq::SHAPE_TRIANGLE);
    cat->addButton("Step Triangle", creat);
    creat.set_param ("shape", (int) node_step_seq::SHAPE_FWSAWTOOTH);
    cat->addButton("Step FW Saw", creat);
    creat.set_param ("shape", (int) node_step_seq::SHAPE_BWSAWTOOTH);
    cat->addButton("Step BW Saw", creat);
    
    cat = sel->addCategory("Samples");
    populateSamples(cat);
}
