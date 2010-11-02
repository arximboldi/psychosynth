/**
 *  Time-stamp:  <2010-11-02 22:44:06 raskolnikov>
 *
 *  @file        config.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Config system implementation.
 */

/*
 *  Copyright (C) 2007, 2009 Juan Pedro Bolívar Puente
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

#define PSYNTH_MODULE_NAME "psynth.base.config"

#include "config.hpp"

using namespace std;

namespace psynth
{
namespace base
{

PSYNTH_DEFINE_ERROR (config_error);
PSYNTH_DEFINE_ERROR_WHAT  (config_type_error, "Config node type mismatch.");
PSYNTH_DEFINE_ERROR_WHAT  (config_backend_error, "Config node has no backend.");

void conf_subject::add_listener (conf_listener& l)
{
    on_change.connect (sigc::mem_fun (l, &conf_listener::handle_conf_change));
    on_nudge.connect (sigc::mem_fun (l, &conf_listener::handle_conf_nudge));
    on_add_child.connect (sigc::mem_fun (l, &conf_listener::handle_conf_add_child));
    on_del_child.connect (sigc::mem_fun (l, &conf_listener::handle_conf_del_child));
}

void conf_subject::del_listener (conf_listener& l)
{
    remove_slot (on_change, sigc::mem_fun (l, &conf_listener::handle_conf_change));
    remove_slot (on_nudge, sigc::mem_fun (l, &conf_listener::handle_conf_nudge));
    remove_slot (on_add_child, sigc::mem_fun (l, &conf_listener::handle_conf_add_child));
    remove_slot (on_del_child, sigc::mem_fun (l, &conf_listener::handle_conf_del_child));
}

void conf_node::save ()
{
    lock lock (this);
    
    if (_backend)
	_backend->save (*this);
    else
	throw config_backend_error ();
}

void conf_node::load ()
{
    lock lock (this);
    
    if (_backend)
	_backend->load (*this);
    else
	throw config_backend_error ();
}

void conf_node::def_load ()
{
    lock lock (this);
    
    if (_backend)
	_backend->def_load (*this);
    else
	throw config_backend_error ();
}

void conf_node::set_backend (conf_backend_ptr backend)
{
    lock lock (this);
    
    if (_backend)
	unset_backend ();
    _backend = backend;
    _backend->attach (*this);
}

void conf_node::unset_backend ()
{
    lock lock (this);
    
    if (_backend) {
	_backend->datach (*this);
	_backend.reset ();
    } else
	throw config_backend_error ();
}

} /* namespace base */
} /* namespace psynth */
