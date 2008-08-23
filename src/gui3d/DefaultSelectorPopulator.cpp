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
#include <libpsynth/table/TableObjectCreator.h>

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
    TableObjectCreator creat;
    
    creat.setName("sampler");
    
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
    
    file_manager::instance()
	.get_path ("psychosynth/samples")
	.find_if (make_has_extension(valid_ext.begin(), valid_ext.end()), files);

    for (list<string>::iterator it = files.begin();
	 it != files.end(); ++it) {
	creat.setParam("file", string(basename(it->c_str())));
	cat->addButton(string(basename(it->c_str())), creat);
    }
}

void DefaultSelectorPopulator::populate(SelectorWindow* sel)
{
    m_selector = sel;
    config::instance()
	.get_path ("psychosynth/file_manager/samples")
	.add_nudge_event (MakeDelegate(this, &DefaultSelectorPopulator::onSamplesConfNudge));
    
    SelectorWindow::Category* cat = NULL;
    TableObjectCreator creat;

    cat = sel->addCategory("Generator");
    creat.setName("oscillator");
    creat.setParam("frequency", (float) 110.0f);
    creat.setParam("amplitude", (float) 0.3f);
    
    creat.setParam("wave", (int) node_oscillator::OSC_SINE);
    cat->addButton("Sine", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_SQUARE);
    cat->addButton("Square", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_SAWTOOTH);
    cat->addButton("Sawtooth", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_TRIANGLE);
    cat->addButton("Triangle", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_MOOGSAW);
    cat->addButton("Moogsaw", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_EXP);
    cat->addButton("Exponential", creat);

    creat.clear();
    creat.setName("audio_noise");
    creat.setParam("type", (int) node_noise::NOISE_PINK);
    cat->addButton("Pink Noise", creat);
    creat.setParam("type", (int) node_noise::NOISE_WHITE);
    cat->addButton("White Noise", creat);
    
    creat.clear();
    cat = sel->addCategory("Control");
    creat.setName("lfo");
    creat.setParam("frequency", 1.0f);
    creat.setParam("amplitude", 0.7f);
    
    creat.setParam("wave", (int) node_oscillator::OSC_SINE);
    cat->addButton("Sine", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_SQUARE);
    cat->addButton("Square", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_SAWTOOTH);
    cat->addButton("Sawtooth", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_TRIANGLE);
    cat->addButton("Triangle", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_MOOGSAW);
    cat->addButton("Moogsaw", creat);
    creat.setParam("wave", (int) node_oscillator::OSC_EXP);
    cat->addButton("Exponential", creat);

    creat.clear();
    creat.setName("control_noise");
    creat.setParam("type", (int) node_noise::NOISE_PINK);
    cat->addButton("Pink Noise", creat);
    creat.setParam("type", (int) node_noise::NOISE_WHITE);
    cat->addButton("White Noise", creat);
    
    creat.clear();
    cat = sel->addCategory("Filter");
    creat.setName("filter");
    creat.setParam("resonance", (float) 0.8f);
	
    creat.setParam("type", (int) node_filter::FILTER_LOWPASS);
    cat->addButton("Low-pass", creat);
    creat.setParam("type", (int) node_filter::FILTER_HIGHPASS);
    cat->addButton("Hi-pass", creat);
    creat.setParam("type", (int) node_filter::FILTER_BANDPASS_CSG);
    cat->addButton("Band-pass CSG", creat);
    creat.setParam("type", (int) node_filter::FILTER_BANDPASS_CZPG);
    cat->addButton("Band-pass CZPG", creat);
    creat.setParam("type", (int) node_filter::FILTER_NOTCH);
    cat->addButton("Notch", creat);
    creat.setParam("type", (int) node_filter::FILTER_MOOG);
    cat->addButton("Moog", creat);

    creat.clear();
    cat = sel->addCategory("Effects");
    creat.setName("echo");
    cat->addButton("Echo", creat);

    creat.setName("delay");
    cat->addButton("Delay", creat);
    
    creat.clear();
    cat = sel->addCategory("Mixers");
    creat.setName("audio_mixer");

    creat.setParam("mixop", (int) node_mixer::MIX_SUM);
    cat->addButton("Audio Sum", creat);
    creat.setParam("mixop", (int) node_mixer::MIX_PRODUCT);
    cat->addButton("Audio Mul", creat);

    creat.setName("control_mixer");

    creat.setParam("mixop", (int) node_mixer::MIX_SUM);
    cat->addButton("Control Sum", creat);
    creat.setParam("mixop", (int) node_mixer::MIX_PRODUCT);
    cat->addButton("Control Mul", creat);

    creat.clear();
    cat = sel->addCategory("Sequencing");
    creat.setName("stepseq");
    creat.setParam("shape", (int) node_step_seq::SHAPE_SQUARE);
    cat->addButton("Step Square", creat);
    creat.setParam("shape", (int) node_step_seq::SHAPE_TRIANGLE);
    cat->addButton("Step Triangle", creat);
    creat.setParam("shape", (int) node_step_seq::SHAPE_FWSAWTOOTH);
    cat->addButton("Step FW Saw", creat);
    creat.setParam("shape", (int) node_step_seq::SHAPE_BWSAWTOOTH);
    cat->addButton("Step BW Saw", creat);
    
    cat = sel->addCategory("Samples");
    populateSamples(cat);
}
