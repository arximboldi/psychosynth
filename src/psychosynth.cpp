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

#include "common/Logger.h"

#define PSYCHOSYNTH_APP

#ifdef PSYCHOSYNTH_3D
# include "gui3d/PsychoSynth3D.h"
#endif
#ifdef PSYCHOSYNTH_APP
# include "psynth/PsychosynthApp.h"
#endif

using namespace std;

int main(int argc, const char *argv[])
{
    Logger::instance().attachSink(new LogDefaultSink);

#ifdef PSYCHOSYNTH_3D
    PsychoSynth3D main_app;
    main_app.run(argc, argv);
#endif
#ifdef PSYCHOSYNTH_APP
    PsychosynthApp main_app;
    main_app.run(argc, argv);
#endif
    
    return 0;
}
