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

#include <list>
#include <string>
#include <dirent.h>

#include <libpsynth/common/Misc.h>
#include <libpsynth/common/FileManager.h>
#include <libpsynth/table/TableObjectCreator.h>

#include <libpsynth/object/ObjectMixer.h>
#include <libpsynth/object/ObjectOscillator.h>
#include <libpsynth/object/ObjectLFO.h>
#include <libpsynth/object/ObjectFilter.h>
#include <libpsynth/object/ObjectSampler.h>
#include <libpsynth/object/ObjectStepSeq.h>

#include "gui3d/SelectorWindow.h"
#include "gui3d/DefaultSelectorPopulator.h"

using namespace psynth;
using namespace std;

void DefaultSelectorPopulator::populate(const std::string& data_path,
					SelectorWindow* sel)
{
    SelectorWindow::Category* cat = NULL;
    TableObjectCreator creat;

    cat = sel->addCategory("Oscillator");
    creat.setName("oscillator");
    
    creat.setParam("wave", (int) ObjectOscillator::OSC_SINE);
    cat->addButton("Sine", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_SQUARE);
    cat->addButton("Square", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_SAWTOOTH);
    cat->addButton("Sawtooth", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_TRIANGLE);
    cat->addButton("Triangle", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_MOOGSAW);
    cat->addButton("Moogsaw", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_EXP);
    cat->addButton("Exponential", creat);

    creat.clear();
    cat = sel->addCategory("LFO");
    creat.setName("lfo");
    creat.setParam("frequency", 1.0f);
    
    creat.setParam("wave", (int) ObjectOscillator::OSC_SINE);
    cat->addButton("Sine", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_SQUARE);
    cat->addButton("Square", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_SAWTOOTH);
    cat->addButton("Sawtooth", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_TRIANGLE);
    cat->addButton("Triangle", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_MOOGSAW);
    cat->addButton("Moogsaw", creat);
    creat.setParam("wave", (int) ObjectOscillator::OSC_EXP);
    cat->addButton("Exponential", creat);

    creat.clear();
    cat = sel->addCategory("Filter");
    creat.setName("filter");
    
    creat.setParam("type", (int) ObjectFilter::FILTER_LOWPASS);
    cat->addButton("Low-pass", creat);
    creat.setParam("type", (int) ObjectFilter::FILTER_HIGHPASS);
    cat->addButton("Hi-pass", creat);
    creat.setParam("type", (int) ObjectFilter::FILTER_BANDPASS_CSG);
    cat->addButton("Band-pass CSG", creat);
    creat.setParam("type", (int) ObjectFilter::FILTER_BANDPASS_CZPG);
    cat->addButton("Band-pass CZPG", creat);
    creat.setParam("type", (int) ObjectFilter::FILTER_NOTCH);
    cat->addButton("Notch", creat);
    creat.setParam("type", (int) ObjectFilter::FILTER_MOOG);
    cat->addButton("Moog", creat);

    creat.clear();
    cat = sel->addCategory("Mixers");
    creat.setName("audio_mixer");

    creat.setParam("mixop", (int) ObjectMixer::MIX_SUM);
    cat->addButton("Audio Sum", creat);
    creat.setParam("mixop", (int) ObjectMixer::MIX_PRODUCT);
    cat->addButton("Audio Mul", creat);

    creat.setName("control_mixer");

    creat.setParam("mixop", (int) ObjectMixer::MIX_SUM);
    cat->addButton("Control Sum", creat);
    creat.setParam("mixop", (int) ObjectMixer::MIX_PRODUCT);
    cat->addButton("Control Mul", creat);

    creat.clear();
    cat = sel->addCategory("Sequencing");
    creat.setName("stepseq");
    creat.setParam("shape", (int) ObjectStepSeq::SHAPE_SQUARE);
    cat->addButton("Step Square", creat);
    creat.setParam("shape", (int) ObjectStepSeq::SHAPE_TRIANGLE);
    cat->addButton("Step Triangle", creat);
    creat.setParam("shape", (int) ObjectStepSeq::SHAPE_FWSAWTOOTH);
    cat->addButton("Step FW Saw", creat);
    creat.setParam("shape", (int) ObjectStepSeq::SHAPE_BWSAWTOOTH);
    cat->addButton("Step BW Saw", creat);
    
    creat.clear();
    cat = sel->addCategory("Samples");
    creat.setName("sampler");
    
    list<string> files;
    list<string> valid_ext;

    valid_ext.push_back("au");
    valid_ext.push_back("wav");
    valid_ext.push_back("ogg");
    valid_ext.push_back("aiff");
    
    FileManager::instance()
	.getPath("psychosynth/samples")
	.findIf(MakeHasExtension(valid_ext.begin(), valid_ext.end()), files);

    for (list<string>::iterator it = files.begin();
	 it != files.end(); ++it) {
	creat.setParam("file", string(basename(it->c_str())));
	cout << basename(it->c_str()) << endl;
	cat->addButton(string(basename(it->c_str())), creat);
    }
    
}
