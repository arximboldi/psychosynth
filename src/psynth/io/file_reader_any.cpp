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

#include <boost/filesystem.hpp>

#include "version.hpp"
#include "base/misc.hpp"
#include "base/logger.hpp"
#include "io/file_reader_any.hpp"

#ifdef PSYNTH_HAVE_PCM
#include "io/file_reader_wave.hpp"
#endif
#ifdef PSYNTH_HAVE_OGG
#include "io/file_reader_ogg.hpp"
#endif

#include <iostream>

using namespace std;
using namespace psynth::base;

namespace psynth
{

void file_reader_any::open (const std::string& file)
{
    if (!is_open ()) {
	std::string ext = boost::filesystem::extension (file);

	m_the_reader = 0;
	
	if (0) {}
#ifdef PSYNTH_HAVE_PCM
	else if ((".wav"  == ext) ||
		 (".aiff" == ext) ||
		 (".au"  == ext))
	    m_the_reader = new file_reader_wave;
#endif
#ifdef PSYNTH_HAVE_OGG
	else if (".ogg" == ext)
	    m_the_reader = new file_reader_ogg;
#endif
	else
	    logger::self () ("input", log::error,
			     "Could not open file: " + file +
			     ". Unsupported file extension");
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
