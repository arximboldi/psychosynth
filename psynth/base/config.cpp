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

#include "base/config.hpp"

using namespace std;

namespace psynth
{

PSYNTH_DEFINE_ERROR_WHERE (config_error, "config");
PSYNTH_DEFINE_ERROR_WHAT  (config_type_error, "Config node type mismatch.");
PSYNTH_DEFINE_ERROR_WHAT  (config_backend_error, "Config node has no backend.");

void conf_node::save ()
{
    lock lock (this);
    
    if (m_backend)
	m_backend->save (*this);
    else
	throw config_backend_error ();
}

void conf_node::load ()
{
    lock lock (this);
    
    if (m_backend)
	m_backend->load (*this);
    else
	throw config_backend_error ();
}

void conf_node::def_load ()
{
    lock lock (this);
    
    if (m_backend)
	m_backend->def_load (*this);
    else
	throw config_backend_error ();
}

void conf_node::attach_backend (conf_backend* backend)
{
    lock lock (this);
    
    if (m_backend)
	datach_backend ();
    m_backend = backend;
    m_backend->attach (*this);
}

void conf_node::datach_backend ()
{
    lock lock (this);
    
    if (m_backend) {
	m_backend->datach (*this);
	m_backend = NULL;
    } else
	throw config_backend_error ();
}

} /* namespace psynth */
