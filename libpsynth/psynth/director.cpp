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

#include <cstring>
#include <algorithm>
#include "psynth/director.h"
#include "psynth/defaults.h"
#include "table/PatcherDynamic.h"

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
	m_output->get_output()->close();
	m_table->detachOutput(m_output->get_output());
	
	m_output->stop();   
	
	delete m_output;
	m_output = NULL;
    }
}

void director::start_output ()
{
    std::string out_name;

    m_config->get_child ("output").get(out_name);
    
    odf_map::iterator i = m_outdir.find(out_name);
    if (i != m_outdir.end()) {
	m_output = i->second->create_output_director();
	m_output->start(m_config->get_child (i->second->get_name()));

	m_table->attachOutput(m_output->get_output());

	m_output->get_output()->setInfo(m_info);
	m_output->get_output()->open();
	m_output->get_output()->start();
    } else {
	m_config->get_child ("output").set(DEFAULT_OUTPUT);
    }

    m_old_output = out_name;
}

void director::register_config()
{
    m_config->get_child ("sample_rate") .def(DEFAULT_SAMPLE_RATE);
    m_config->get_child ("block_size")  .def(DEFAULT_BLOCK_SIZE);
    m_config->get_child ("num_channels").def(DEFAULT_NUM_CHANNELS);
    m_config->get_child ("output")      .def(DEFAULT_OUTPUT);

    m_config->add_nudge_event (MakeDelegate(this, &director::on_config_nudge));

#if 0
    m_config->get_child ("sample_rate").addChangeEvent(MakeDelegate(this, &director::on_sample_rate_change));
    m_config->get_child ("block_size").addChangeEvent(MakeDelegate(this, &director::on_block_Size_change));
    m_config->get_child ("num_channels").addChangeEvent(MakeDelegate(this, &director::on_num_channels_change));
    m_config->get_child ("output").addChangeEvent(MakeDelegate(this, &director::on_output_change));
#endif
}

void director::unregister_config()
{
    m_config->delete_nudge_event (MakeDelegate(this, &director::on_config_nudge));

#if 0
    m_config->get_child ("sample_rate").deleteChangeEvent(MakeDelegate(this, &director::on_sample_rate_change));
    m_config->get_child ("block_size").deleteChangeEvent(MakeDelegate(this, &director::on_block_Size_change));
    m_config->get_child ("num_channels").deleteChangeEvent(MakeDelegate(this, &director::on_num_channels_change));
    m_config->get_child ("output").deleteChangeEvent(MakeDelegate(this, &director::on_output_change));    
#endif
}

void director::start (conf_node& conf, const std::string& home_path)
{
    m_config = &conf;

    m_filemgr.start(conf.get_child ("file_manager"),
		    home_path);
    
    /* A bit dirty... */
    for (odf_map::iterator i = m_outdir.begin(); i != m_outdir.end(); ++i) {
	output_director* od = i->second->create_output_director();
	od->defaults(m_config->get_child (i->first));
	delete od;
    }
    
    register_config();

    conf.get_child ("sample_rate").get(m_info.sample_rate);
    conf.get_child ("num_channels").get(m_info.num_channels);
    conf.get_child ("block_size").get(m_info.block_size);

    m_table = new Table(m_info);
    m_table->attachPatcher(new PatcherDynamic); /* FIXME: */
    
    start_output ();
}

void director::stop()
{
    unregister_config();
    
    stop_output ();
    delete m_table;

    m_filemgr.stop();
    
    m_table = NULL;
    m_output = NULL;
    m_config = NULL;
}

void director::update_info ()
{
    m_table->setInfo(m_info);

    Output::State old_state;
    old_state = m_output->get_output()->getState();
    
    m_output->get_output()->gotoState(Output::NOTINIT);
    m_output->get_output()->setInfo(m_info);
    m_output->get_output()->gotoState(old_state);
}

bool director::on_config_nudge(conf_node& node)
{
    string out;
    
    node.get_child ("sample_rate").get(m_info.sample_rate);
    node.get_child ("block_size").get(m_info.block_size);
    node.get_child ("num_channels").get(m_info.num_channels);
    node.get_child ("output").get(out);

    m_table->setInfo(m_info);

    if (m_output && out == m_old_output) {    
	Output::State old_state;
	old_state = m_output->get_output()->getState();
	m_output->get_output()->gotoState(Output::NOTINIT);
	m_output->get_output()->setInfo(m_info);
	m_output->get_output()->gotoState(old_state);
    } else {
	stop_output ();
	start_output ();
    }
    
    return false;
}

#if 0
bool director::on_sample_rate_change(conf_node& node)
{
    cout << "SAMPLE_RATE_CHANGE!\n";
    node.get(m_info.sample_rate);
    update_info ();
    
    return false;
}

bool director::on_block_Size_change (conf_node& node)
{
    node.get(m_info.block_size);
    update_info ();
    
    return false;
}

bool director::on_num_channels_change (conf_node& node)
{
    node.get(m_info.num_channels);
    update_info ();
    
    return false;
}

bool director::on_output_change (conf_node& node)
{
    stop_output ();
    start_output ();
    return false;
}
#endif

} /* namespace psynth */
