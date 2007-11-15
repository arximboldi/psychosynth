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

#include <iostream>

#include "psychosynth.h"

#include "table/Table.h"
#include "table/Patcher.h"
#include "common/CmdLineOpts.h"
#include "gui3d/PsychoSynth3D.h"
#include "output/OutputAlsa.h"
#include "output/OutputOss.h"
#include "object/ObjectOscillator.h"
#include "object/ObjectMixer.h"
#include "object/ObjectOutput.h"
#include "object/ObjectManager.h"

using namespace std;

void showVersion() {
    
};

int main(int argc, const char *argv[])
{	
#if 1
    PsychoSynth3D mainApp;

    mainApp.run(argc, argv);
#endif

#if 0
    AudioInfo info(DEFAULT_SAMPLE_RATE, DEFAULT_BUFFER_SIZE, DEFAULT_NUM_CHANNEL);
    Table table(info);
    Output* output(new OutputAlsa(info, "default"));

    table.attachOutput(output);
    TableObject obj = table.addObject(OBJ_OSCILLATOR);
    table.activateObject(obj);
    table.update();
    cout << "Opening...\n";
    output->open();
    cout << "Starting...\n";
    output->start();
    cout << "Waiting... \n";

    int delta = 20;
    int total = 1000;
    int type = ObjectOscillator::OSC_SINE;	
    do {
	obj.setParam(ObjectOscillator::PARAM_WAVE, type);
	int n = total;
	float freq = 30;
	do {
	    obj.setParam(ObjectOscillator::PARAM_FREQUENCY, freq);
	    freq += delta;
	    usleep(delta * 1000);
	} while ((n -= delta) > 0);
	n = 1000;
	do {
	    obj.setParam(ObjectOscillator::PARAM_FREQUENCY, freq);
	    freq -= delta;
	    usleep(delta * 1000);
	} while ((n -= delta) > 0);
    } while (++type < ObjectOscillator::N_OSC_TYPES);
    obj.deleteMe();
    sleep(1);
#endif
	
    return 0;
}
