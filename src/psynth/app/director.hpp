/**
 *  @file        director.hpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2007
 *
 *  Controller/facade for most synth components.
 */

/*
 *  Copyright (C) 2007, 2008, 2009, 2016 Juan Pedro Bolívar Puente
 *
 *  This file is part of Psychosynth.
 *
 *  Psychosynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Psychosynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


#ifndef PSYNTH_DIRECTOR_H
#define PSYNTH_DIRECTOR_H

#include <psynth/base/config.hpp>
#include <psynth/base/util.hpp>
#include <psynth/world/world.hpp>
#include <psynth/app/output_director.hpp>
#include <psynth/app/file_manager_director.hpp>

namespace psynth
{

class director : private boost::noncopyable
{
    typedef std::map<std::string, output_director_factory*> odf_map;
    odf_map m_outdir;

    file_manager_director m_filemgr;
    output_director* m_output;
    world* m_world;

    base::conf_node* m_config;
    audio_info m_info;

    void on_config_nudge (base::conf_node& node);
        
    void register_config();
    void unregister_config();
    
    void start_output();
    void stop_output();

    boost::signals2::connection m_on_output_change_slot;

public:
    director()
	: m_output(NULL)
	, m_world(NULL) {}

    ~director();
    
    void attach_output_director_factory (output_director_factory* fact);
    void start (base::conf_node& conf, const boost::filesystem::path& home_path);
    void stop ();
    
    world* get_world () {
	return m_world;
    }

    graph::audio_async_output_ptr get_output() {
	if (m_output)
	    return m_output->get_output ();
	return {};
    }
};

} /* namespace psynth */

#endif /* PSYNTH_DIRECTOR_H */

