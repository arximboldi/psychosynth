/***************************************************************************
 *                                                                         *
 *   PSYCHOSYNTH                                                           *
 *   ===========                                                           *
 *                                                                         *
 *   Copyright (C) Juan Pedro Bolivar Puente 2007, 2008                    *
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

#ifndef PSYNTH_DEFAULTS_H
#define PSYNTH_DEFAULTS_H

#include <libpsynth/version.h>

#ifdef PSYNTH_HAVE_ALSA
#define DEFAULT_OUTPUT        "alsa"
#else
# ifdef PSYNTH_HAVE_JACK
# define DEFAULT_OUTPUT       "jack"
# else
#  ifdef PSYNTH_HAVE_OSS
#  define DEFAULT_OUTPUT      "oss"
#  else
#  define DEFAULT_OPUTPUT     "dummy"
#  endif
# endif
#endif

#define DEFAULT_NUM_CHANNELS  2
#define DEFAULT_BLOCK_SIZE    256
#define DEFAULT_SAMPLE_RATE   44100

#endif /* PSYNTH_DEFAULTS_H */
