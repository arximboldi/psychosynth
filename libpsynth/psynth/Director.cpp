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
#include "psynth/Director.h"
#include "psynth/Defaults.h"
#include "table/PatcherDynamic.h"

using namespace std;

namespace psynth
{

Director::~Director()
{
    for (ODFMap::iterator i = m_outdir.begin(); i != m_outdir.end(); ++i)
	delete i->second;
}

void Director::attachOutputDirectorFactory(OutputDirectorFactory* fact)
{
    m_outdir[fact->getName()] = fact;
}

void Director::stopOutput()
{
    if (m_output) {
	m_output->getOutput()->close();
	m_table->detachOutput(m_output->getOutput());
	
	m_output->stop();   
	
	delete m_output;
	m_output = NULL;
    }
}

void Director::startOutput()
{
    std::string out_name;

    m_config->get_child ("output").get(out_name);
    
    ODFMap::iterator i = m_outdir.find(out_name);
    if (i != m_outdir.end()) {
	m_output = i->second->createOutputDirector();
	m_output->start(m_config->get_child (i->second->getName()));

	m_table->attachOutput(m_output->getOutput());

	m_output->getOutput()->setInfo(m_info);
	m_output->getOutput()->open();
	m_output->getOutput()->start();
    } else {
	m_config->get_child ("output").set(DEFAULT_OUTPUT);
    }

    m_old_output = out_name;
}

void Director::registerConfig()
{
    m_config->get_child ("sample_rate") .def(DEFAULT_SAMPLE_RATE);
    m_config->get_child ("block_size")  .def(DEFAULT_BLOCK_SIZE);
    m_config->get_child ("num_channels").def(DEFAULT_NUM_CHANNELS);
    m_config->get_child ("output")      .def(DEFAULT_OUTPUT);

    m_config->add_nudge_event (MakeDelegate(this, &Director::onConfigNudge));

#if 0
    m_config->get_child ("sample_rate").addChangeEvent(MakeDelegate(this, &Director::onSampleRateChange));
    m_config->get_child ("block_size").addChangeEvent(MakeDelegate(this, &Director::onBlockSizeChange));
    m_config->get_child ("num_channels").addChangeEvent(MakeDelegate(this, &Director::onNumChannelsChange));
    m_config->get_child ("output").addChangeEvent(MakeDelegate(this, &Director::onOutputChange));
#endif
}

void Director::unregisterConfig()
{
    m_config->delete_nudge_event (MakeDelegate(this, &Director::onConfigNudge));

#if 0
    m_config->get_child ("sample_rate").deleteChangeEvent(MakeDelegate(this, &Director::onSampleRateChange));
    m_config->get_child ("block_size").deleteChangeEvent(MakeDelegate(this, &Director::onBlockSizeChange));
    m_config->get_child ("num_channels").deleteChangeEvent(MakeDelegate(this, &Director::onNumChannelsChange));
    m_config->get_child ("output").deleteChangeEvent(MakeDelegate(this, &Director::onOutputChange));    
#endif
}

void Director::start (conf_node& conf, const std::string& home_path)
{
    m_config = &conf;

    m_filemgr.start(conf.get_child ("file_manager"),
		    home_path);
    
    /* A bit dirty... */
    for (ODFMap::iterator i = m_outdir.begin(); i != m_outdir.end(); ++i) {
	OutputDirector* od = i->second->createOutputDirector();
	od->defaults(m_config->get_child (i->first));
	delete od;
    }
    
    registerConfig();

    conf.get_child ("sample_rate").get(m_info.sample_rate);
    conf.get_child ("num_channels").get(m_info.num_channels);
    conf.get_child ("block_size").get(m_info.block_size);

    m_table = new Table(m_info);
    m_table->attachPatcher(new PatcherDynamic); /* FIXME: */
    
    startOutput();
}

void Director::stop()
{
    unregisterConfig();
    
    stopOutput();
    delete m_table;

    m_filemgr.stop();
    
    m_table = NULL;
    m_output = NULL;
    m_config = NULL;
}

void Director::updateInfo()
{
    m_table->setInfo(m_info);

    Output::State old_state;
    old_state = m_output->getOutput()->getState();
    
    m_output->getOutput()->gotoState(Output::NOTINIT);
    m_output->getOutput()->setInfo(m_info);
    m_output->getOutput()->gotoState(old_state);
}

bool Director::onConfigNudge(conf_node& node)
{
    string out;
    
    node.get_child ("sample_rate").get(m_info.sample_rate);
    node.get_child ("block_size").get(m_info.block_size);
    node.get_child ("num_channels").get(m_info.num_channels);
    node.get_child ("output").get(out);

    m_table->setInfo(m_info);

    if (m_output && out == m_old_output) {    
	Output::State old_state;
	old_state = m_output->getOutput()->getState();
	m_output->getOutput()->gotoState(Output::NOTINIT);
	m_output->getOutput()->setInfo(m_info);
	m_output->getOutput()->gotoState(old_state);
    } else {
	stopOutput();
	startOutput();
    }
    
    return false;
}

#if 0
bool Director::onSampleRateChange(conf_node& node)
{
    cout << "SAMPLE_RATE_CHANGE!\n";
    node.get(m_info.sample_rate);
    updateInfo();
    
    return false;
}

bool Director::onBlockSizeChange(conf_node& node)
{
    node.get(m_info.block_size);
    updateInfo();
    
    return false;
}

bool Director::onNumChannelsChange(conf_node& node)
{
    node.get(m_info.num_channels);
    updateInfo();
    
    return false;
}

bool Director::onOutputChange(conf_node& node)
{
    stopOutput();
    startOutput();
    return false;
}
#endif

} /* namespace psynth */
