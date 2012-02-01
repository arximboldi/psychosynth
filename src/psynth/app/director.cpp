/**
 *  Time-stamp:  <2012-02-01 23:44:34 raskolnikov>
 *
 *  @file        director.cpp
 *  @author      Juan Pedro Bolívar Puente <raskolnikov@es.gnu.org>
 *  @date        2008
 *
 *  Controller/facade for the whole synth system.
 */

/*
 *  Copyright (C) 2008, 2009 Juan Pedro Bolívar Puente
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

#include <cstring>
#include <algorithm>
#include "app/director.hpp"
#include "app/defaults.hpp"
#include "world/patcher_dynamic.hpp"

using namespace std;

namespace psynth
{

director::~director ()
{
    for (odf_map::iterator i = m_outdir.begin(); i != m_outdir.end(); ++i)
	delete i->second;
}

void director::attach_output_director_factory (output_director_factory* fact)
{
    m_outdir[ fact->get_name () ] = fact;
}

void director::stop_output ()
{
    if (m_output) {
        m_world->detach_output (m_output->get_output());
        delete m_output;
	m_output = 0;
    }
}

void director::start_output ()
{
    try
    {
        std::string out_name;

        m_config->child ("output").get (out_name);
    
        odf_map::iterator i = m_outdir.find(out_name);
        if (i != m_outdir.end()) {
            m_output = i->second->create_output_director();
            m_output->start (m_config->child (i->second->get_name()));
            m_world->attach_output (m_output->get_output());
            m_output->get_output()->start();
        } else {
            m_config->child ("output").set (string (PSYNTH_DEFAULT_OUTPUT));
        }

    }
    catch (base::exception& err)
    {
        err.log();
    }
}

void director::register_config ()
{
    m_config->child ("sample_rate") .def (int (PSYNTH_DEFAULT_SAMPLE_RATE));
    m_config->child ("block_size")  .def (int (PSYNTH_DEFAULT_BLOCK_SIZE));
    m_config->child ("num_channels").def (int (PSYNTH_DEFAULT_NUM_CHANNELS));
    m_config->child ("output")      .def (string (PSYNTH_DEFAULT_OUTPUT));

    m_on_output_change_slot =
        m_config->on_nudge.connect (boost::bind (&director::on_config_nudge, this, _1));
}

void director::unregister_config()
{
    m_config->on_nudge.disconnect (m_on_output_change_slot);
}

void director::start (base::conf_node& conf, const boost::filesystem::path& home_path)
{
    m_config = &conf;

    m_filemgr.start (conf.child ("file_manager"), home_path);
    
    for (auto i = m_outdir.begin(); i != m_outdir.end(); ++i)
    {
        std::unique_ptr<output_director> od (i->second->create_output_director());
	od->defaults(m_config->child (i->first));
    }
    
    register_config();

    m_info.sample_rate = conf.child ("sample_rate").get<int> ();
    m_info.num_channels = conf.child ("num_channels").get<int> ();
    m_info.block_size = conf.child ("block_size").get<int> ();

    m_world = new world (m_info);
    m_world->set_patcher (base::manage (new patcher_dynamic));
    
    start_output ();
}

void director::stop()
{
    unregister_config();
    
    stop_output ();
    delete m_world;

    m_filemgr.stop();
    
    m_world = NULL;
    m_output = NULL;
    m_config = NULL;
}

void director::on_config_nudge (base::conf_node& node)
{
    string out;
    
    m_info.sample_rate  = node.child ("sample_rate").get<int> ();
    m_info.block_size   = node.child ("block_size").get<int> ();
    m_info.num_channels = node.child ("num_channels").get<int> ();
    node.child ("output").get (out);
    
    m_world->set_info (m_info);

    stop_output ();
    start_output ();
}


} /* namespace psynth */
