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

#include "version.h"
#include "common/misc.h"
#include "input/file_reader_any.h"

#ifdef PSYNTH_HAVE_PCM
#include "input/file_reader_wave.h"
#endif
#ifdef PSYNTH_HAVE_OGG
#include "input/file_reader_ogg.h"
#endif

#include <iostream>

using namespace std;
namespace psynth
{

void file_reader_any::open (const char* file)
{
    if (!is_open()) {
	const char* ext = get_extension (file);

	m_the_reader = NULL;
	
	if (0) {}
#ifdef PSYNTH_HAVE_PCM
	else if (!strcmp_i("wav", ext) ||
		 !strcmp_i("aiff", ext) ||
		 !strcmp_i("au", ext))
	    m_the_reader = new file_reader_wave;
#endif
#ifdef PSYNTH_HAVE_OGG
	else if (!strcmp_i("ogg", ext))
	    m_the_reader = new file_reader_ogg;
#endif

	if (m_the_reader) {
	    m_the_reader->open (file);
	    if (m_the_reader->is_open()) {
		set_is_open (true);
		set_info (m_the_reader->get_info());
	    } else {
		delete m_the_reader;
		m_the_reader = NULL;
	    }
	}
    }
}

} /* namespace psynth */
